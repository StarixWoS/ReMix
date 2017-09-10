
#include "upnp.hpp"
#include "includes.hpp"

QVector<qint32> UPNP::ports;
bool UPNP::tunneled{ false };
UPNP* UPNP::upnp{ nullptr };
QString UPNP::upnpLog{ "logs/upnpLog.txt" };

UPNP* UPNP::getUpnp()
{
    if ( upnp == nullptr )
        upnp = new UPNP();

    if ( Settings::getLogFiles() )
    {
        QString msg{ "Getting UPNP Object." };
        Helper::logToFile( upnpLog, msg, true, true );
    }
    return upnp;
}

bool UPNP::getTunneled()
{
    return tunneled;
}

void UPNP::setTunneled(bool value)
{
    tunneled = value;
}

UPNP::UPNP(QObject* parent )
    : QObject( parent )
{
    localIP = QHostAddress( Helper::getPrivateIP() );

    //We have a broadcast IP address. We are unable to port forward.
    if ( localIP != QHostAddress::AnyIPv4 )
    {
        localAddress.setIp( localIP );
        localAddress.setBroadcast( QHostAddress::Broadcast );

        httpSocket = new QNetworkAccessManager();
        udpSocket = new QUdpSocket();
        timer = new QTimer( this );

        udpSocket->bind( localAddress.ip(), 0 );

        //When the UPNP Tunnel is initialized, begin refreshing the Tunnel
        //every 30 minutes as it time's out.
        refreshTunnel = new QTimer( this );
        refreshTunnel->setInterval( UPNP_TIME_OUT_MS );
        QObject::connect( refreshTunnel, &QTimer::timeout, refreshTunnel,
                          [=]()
        {
            for (  qint32 port : ports )
            {
                if ( port != 0 )
                {
                    this->checkPortForward( "TCP", port );
                    this->checkPortForward( "UDP", port );
                }
            }
        });

        if ( Settings::getLogFiles() )
        {
            QString msg{ "Creating UPNP Object." };
            msg = msg.arg( localIP.toString() );
            Helper::logToFile( upnpLog, msg, true, true );
        }
    }
    else    //Delete our object at the next oppertunity.
    {
        this->disconnect();
        this->deleteLater();
    }
}

UPNP::~UPNP()
{
    httpSocket->disconnect();
    httpSocket->deleteLater();

    udpSocket->disconnect();
    udpSocket->deleteLater();

    timer->disconnect();
    timer->deleteLater();

    upnp = nullptr;
}

void UPNP::makeTunnel( int internal, int external )
{
    QObject::connect( udpSocket, &QUdpSocket::readyRead,
                      this, &UPNP::getUdp );

    QObject::connect( timer, &QTimer::timeout,
                      this, &UPNP::timeExpired );

    externalPort = external;
    internalPort = internal;
    QString discover = QString( "M-SEARCH * HTTP/1.1\r\n"
                                "HOST:239.255.255.250:1900\r\n"
                                "ST:urn:schemas-upnp-org:device:Internet"
                                "GatewayDevice:1\r\n"/*upnp:rootdevice*/
                                "Man:\"ssdp:discover\"\r\n"
                                "MX:3\r\n\r\n" );

    udpSocket->writeDatagram( discover.toLatin1(),
                              discover.size(),
                              localAddress.broadcast(),
                              1900 );
    timer->start( 1000 );
    this->setTunneled( true );
}

void UPNP::getUdp()
{
    QString vs;
    while ( udpSocket->hasPendingDatagrams() )
    {
        QByteArray datagram;
        QHostAddress sender;
        quint16 senderPort;
        QString sport;

        datagram.resize( static_cast<int>( udpSocket->pendingDatagramSize() ) );
        udpSocket->readDatagram( datagram.data(),
                                 datagram.size(),
                                 &sender,
                                 &senderPort );

        QHostAddress st_addr( sender.toIPv4Address() );
        if ( st_addr != localAddress.ip() )
        {
            timer->stop();
            gateway = st_addr;

            vs = datagram.data();
            int index = vs.indexOf( "LOCATION: " );
            if ( index != -1 )
            {
                vs.remove( 0, index + 10 );
                index = 0;

                while ( vs[ index ].isPrint() )
                {
                    index++;
                }

                vs.remove( index, vs.size() - index );
                sport = vs;

                sport.remove( 0, 5 );
                index = 0;
                while ( sport[ index ] != QChar( ':' ) )
                {
                    index++;
                }

                sport.remove( 0, index+1 );
                index = 0;
                while ( sport[ index ] != QChar( '/' ) )
                {
                    index++;
                }

                sport.remove( index, sport.size() - index );

                ctrlPort = sport;
                QObject::connect( httpSocket, &QNetworkAccessManager::finished,
                                  httpSocket,
                [=](QNetworkReply *reply)
                {
                    QString response = reply->readAll();
                    int i = 0;
                    while ( i < response.size() )
                    {
                        if ( !response[ i ].isPrint() )
                            response.remove( i, 1 );
                        else
                            i++;
                    }

                    QXmlStreamReader reader( response );
                                     reader.readNext();
                    while ( !reader.atEnd() )
                    {
                        if ( reader.name().toString() == QString( "serviceType" ) )
                        {
                            rtrSchema = reader.readElementText();
                            if ( rtrSchema == QString( "urn:schemas-upnp-org:service:WANIPConnection:1" )
                              || rtrSchema == QString( "urn:schemas-upnp-org:service:WANPPPConnection:1" ) )
                            {
                                while ( ( !reader.atEnd() )
                                     && ( reader.name().toString() != "controlURL" ) )
                                {
                                    reader.readNext();
                                }

                                if ( reader.name().toString() == "controlURL" )
                                {
                                    gatewayCtrlUrl = QString( "http://%1:%2%3" )
                                                         .arg( gateway.toString() )
                                                         .arg( ctrlPort )
                                                         .arg( reader.readElementText() );
                                    emit this->success();

                                    if ( Settings::getLogFiles() )
                                    {
                                        QString logMsg{ "Successfully got ControlURL[ %1 ] with Schema[ %2 ] using Gateway[ %3 ] and Port[ %4 ]." };
                                                logMsg = logMsg.arg( gatewayCtrlUrl.toString() )
                                                               .arg( rtrSchema )
                                                               .arg( gateway.toString() )
                                                               .arg( ctrlPort );
                                        Helper::logToFile( upnpLog, logMsg, true, true );
                                    }
                                    break;
                                }
                            }
                            else
                                reader.readNext();
                        }
                        else
                            reader.readNext();
                    }
                    //this->getExternalIP();

                    httpSocket->disconnect();
                });
                httpSocket->get( QNetworkRequest( QUrl( vs ) ) );
                udpSocket->close();
            }
        }
    }
}

void UPNP::timeExpired()
{
    //udpSocket->disconnect();
    timer->disconnect();

    timer->stop();
    emit this->error( "Time expired!" );

    if ( Settings::getLogFiles() )
    {
        QString logMsg{ "UPNP Object Timed Out!" };
        Helper::logToFile( upnpLog, logMsg, true, true );
    }
}

void UPNP::getExternalIP()
{
    QString message{ "<?xml version=\"1.0\"?>"
                     "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                     "SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                     "<SOAP-ENV:Body><m:GetExternalIPAddress xmlns:m=\"%1\"/>"
                     "</SOAP-ENV:Body></SOAP-ENV:Envelope>" };

    message = message.arg( rtrSchema );
    this->postSOAP( "GetExternalIPAddress", message, "" );

    if ( Settings::getLogFiles() )
    {
        QString logMsg{ "Getting ExternalIP with SOAP[ %1 ]." };
                logMsg = logMsg.arg( message );
        Helper::logToFile( upnpLog, logMsg, true, true );
    }
}

void UPNP::postSOAP(QString action, QString message , QString protocol, qint32 port)
{
    QString soap{ "\"%1#%2\"" };
            soap = soap.arg( rtrSchema ).arg( action );
    QString host{ "%1:%2" };
            host = host.arg( gateway.toString() ).arg( ctrlPort );

    QNetworkRequest req( gatewayCtrlUrl );
                    req.setRawHeader( QByteArray( "Cache-Control" ), "no-cache" );
                    req.setRawHeader( QByteArray( "Connection" ), "Close" );
                    req.setRawHeader( QByteArray( "Pragma" ), "no-cache" );
                    req.setRawHeader( QByteArray( "Content-Type" ), "text/xml; charset=\"utf-8\"" );
                    req.setRawHeader( QByteArray( "User-Agent" ), "User-Agent: Microsoft-Windows/10.0 UPnP/1.0" );
                    req.setRawHeader( QByteArray( "Content-Length" ), QString::number( message.size() ).toLatin1() );
                    req.setRawHeader( QByteArray( "SOAPAction" ), soap.toLatin1() );
                    req.setRawHeader( QByteArray( "Host" ), host.toLatin1() );

    QNetworkReply* httpReply = httpSocket->post( req, message.toLatin1() );
    QObject::connect( httpReply, &QNetworkReply::readyRead, httpReply,
    [=]()
    {
        QString reply = httpReply->readAll();
        if ( !reply.contains( "UPnPError" ) )
        {
            if ( reply.contains( "<NewExternalIPAddress>" ) )
                this->extractExternalIP( reply );

            if ( reply.contains( "DeletePortMappingResponse" ) )
            {
                emit this->removedPortForward( port, protocol );
            }

            if ( reply.contains( "AddPortMappingResponse" ) )
            {
                emit this->addedPortForward( port, protocol );
                refreshTunnel->start();
            }

            if ( reply.contains( "GetSpecificPortMappingEntryResponse" ) )
            {
                //The port is valid, but we didn't add it.
                //Add the port to be refreshed.
                if ( !ports.contains( port ) )
                {
                    ports.append( port );
                }
            }

            if ( Settings::getLogFiles() )
            {
                QString logMsg{ "Got Reply for Port[ %1:%2 ] from Action[ %3 ] using SOAP[ %4 ] [ %5 ]" };
                        logMsg = logMsg.arg( protocol )
                                       .arg( port )
                                       .arg( action )
                                       .arg( soap )
                                       .arg( reply );
                Helper::logToFile( upnpLog, logMsg, true, true );
            }
        }
        else
        {
            //Some modems and routers incorrectly announce "Invalid Args" in
            //lieu of a "NoSuchEntryInArray"
            //reply to a "GetSpecificPortMapping" request.
            //Try and add port forwards if our response holds this value.
            if ( reply.contains( "Invalid Args" ) )
                this->addPortForward( protocol, port );
            else
                this->extractError( reply, port, protocol );
        }
        httpReply->disconnect();
        httpReply->deleteLater();
    });
}

void UPNP::extractError( QString message, qint32 port, QString protocol )
{
    QXmlStreamReader reader( message );
    reader.readNext();
    while ( reader.name().toString() != QString( "errorDescription" ) )
    {
        reader.readNext();
    }

    if ( reader.name().toString() == QString( "errorDescription" ) )
    {
        QString elementText{ reader.readElementText() };
        if ( (elementText == QString( "NoSuchEntryInArray") )
          || (elementText == QString( "Invalid Args" ) ))
        {
            this->addPortForward( protocol, port );
            emit this->checkedPortForward( port, protocol );
        }

        if ( reader.readElementText().compare( "Invalid Args", Qt::CaseInsensitive ) == 0 )
        {
            this->addPortForward( protocol, port );
            emit this->checkedPortForward( port, protocol );
        }
        else
            emit this->error( reader.readElementText() );
    }

    if ( Settings::getLogFiles() )
    {
        QString logMsg{ "Got Error for Port[ %1:%2 ] [ %3 ]" };
                logMsg = logMsg.arg( protocol )
                               .arg( port )
                               .arg( message );
        Helper::logToFile( upnpLog, logMsg, true, true );
    }
}

void UPNP::extractExternalIP( QString message )
{
    QXmlStreamReader reader( message );
    reader.readNext();
    while ( reader.name().toString() != QString( "NewExternalIPAddress" ) )
    {
        reader.readNext();
    }

    if ( reader.name().toString() == QString( "NewExternalIPAddress" ) )
        externalAddress = QHostAddress( reader.readElementText() );
}

void UPNP::checkPortForward(QString protocol, qint32 port)
{
    QString message( "<?xml version=\"1.0\"?>"
                     "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                     "<SOAP-ENV:Body><m:GetSpecificPortMappingEntry xmlns:m=\"%1\">"
                     "<NewRemoteHost xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\"></NewRemoteHost>"
                     "<NewExternalPort xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui2\">%2</NewExternalPort>"
                     "<NewProtocol xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">%3</NewProtocol>"
                     "</m:GetSpecificPortMappingEntry></SOAP-ENV:Body></SOAP-ENV:Envelope>" );

    message = message.arg( rtrSchema )
                     .arg( QString::number( port ) )
                     .arg( protocol );

    this->postSOAP( "GetSpecificPortMappingEntry", message, protocol, port );

    if ( Settings::getLogFiles() )
    {
        QString logMsg{ "Checking Port[ %1:%2 ] with SOAP[ %3 ]." };
                logMsg = logMsg.arg( protocol )
                               .arg( port )
                               .arg( message );
        Helper::logToFile( upnpLog, logMsg, true, true );
    }
}

void UPNP::addPortForward(QString protocol, qint32 port)
{
    if ( !ports.contains( port ) )
    {
        ports.append( port );
    }

    QString message{ "<?xml version=\"1.0\"?>"
                     "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                     "<SOAP-ENV:Body><m:AddPortMapping xmlns:m=\"%1\">"
                     "<NewRemoteHost xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\"></NewRemoteHost>"
                     "<NewExternalPort xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui2\">%2</NewExternalPort>"
                     "<NewProtocol xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">%3</NewProtocol>"
                     "<NewInternalPort xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui2\">%4</NewInternalPort>"
                     "<NewInternalClient xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">%5</NewInternalClient>"
                     "<NewEnabled xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"boolean\">1</NewEnabled>"
                     "<NewPortMappingDescription xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">%6</NewPortMappingDescription>"
                     "<NewLeaseDuration xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui4\">%7</NewLeaseDuration></m:AddPortMapping>"
                     "</SOAP-ENV:Body></SOAP-ENV:Envelope>" };

    message = message.arg( rtrSchema )
                     .arg( QString::number( port ) )
                     .arg( protocol )
                     .arg( QString::number( port ) )
                     .arg( localAddress.ip().toString() )
                     .arg( "ReMix_" % QString::number( port ) % protocol )
                     .arg( QString::number( UPNP_TIME_OUT_S ) );

    this->postSOAP( "AddPortMapping", message, protocol, port );

    if ( Settings::getLogFiles() )
    {
        QString logMsg{ "Adding Port[ %1:%2 ] with SOAP[ %3 ]." };
                logMsg = logMsg.arg( protocol )
                               .arg( port )
                               .arg( message );
        Helper::logToFile( upnpLog, logMsg, true, true );
    }
}

void UPNP::removePortForward(QString protocol, qint32 port)
{
    QString message( "<?xml version=\"1.0\"?>"
                     "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                     "<SOAP-ENV:Body><m:DeletePortMapping xmlns:m=\"%1\">"
                     "<NewRemoteHost xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\"></NewRemoteHost>"
                     "<NewExternalPort xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui2\">%2</NewExternalPort>"
                     "<NewProtocol xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">%3</NewProtocol>"
                     "</m:DeletePortMapping></SOAP-ENV:Body></SOAP-ENV:Envelope>" );

    message = message.arg( rtrSchema )
                     .arg( QString::number( port ) )
                     .arg( protocol );

    this->postSOAP( "DeletePortMapping", message, protocol, port );

    if ( ports.contains( port ) )
        ports.remove( ports.indexOf( port ) );

    if ( Settings::getLogFiles() )
    {
        QString logMsg{ "Removing Port[ %1:%2 ] with SOAP[ %3 ]." };
        logMsg = logMsg.arg( protocol )
                       .arg( port )
                       .arg( message );
        Helper::logToFile( upnpLog, logMsg, true, true );
    }
}
