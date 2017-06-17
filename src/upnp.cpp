
#include "upnp.hpp"
#include "includes.hpp"

QHostAddress UPNP::getExternalAddress() const
{
    return externalAddress;
}

UPNP::UPNP(QHostAddress localip, QObject* parent )
    : QObject( parent )
{
    localAddress.setIp( localip );
    localAddress.setBroadcast( QHostAddress::Broadcast );

    httpSocket = new QNetworkAccessManager();
    connState = State::NotOpened;
    udpSocket = new QUdpSocket();
    timer = new QTimer( this );

    httpReply = NULL;

    udpSocket->bind( localAddress.ip(), 1900 );

    //When the UPNP Tunnel is initialized, begin refreshing the Tunnel
    //every 30 minutes as it time's out.
    refreshTunnel = new QTimer( this );
    refreshTunnel->setInterval( UPNP_TIME_OUT_MS );
    QObject::connect( refreshTunnel, &QTimer::timeout, [=]()
    {
        this->setTunnel();
    });
}

UPNP::~UPNP()
{
    httpSocket->disconnect();
    httpSocket->deleteLater();

    httpReply->disconnect();
    httpReply->deleteLater();

    udpSocket->disconnect();
    udpSocket->deleteLater();

    timer->disconnect();
    timer->deleteLater();
}

void UPNP::makeTunnel( int internal, int external, QString protocol, QString text )
{
    if ( ( protocol == "TCP" )||( protocol == "UDP" ) )
    {
        QObject::connect( udpSocket, &QUdpSocket::readyRead,
                          this, &UPNP::getUdp );

        QObject::connect( timer, &QTimer::timeout,
                          this, &UPNP::timeExpired );

        externalPort = external;
        internalPort = internal;
        info = text;
        pcol = protocol;
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
    }
    else
    {
        emit this->error( "Invalid protocol" );
    }
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

        datagram.resize( udpSocket->pendingDatagramSize() );
        udpSocket->readDatagram( datagram.data(),
                                  datagram.size(),
                                  &sender,
                                  &senderPort );

        QHostAddress st_addr( sender.toIPv4Address() );
        if ( st_addr != localAddress.ip() )
        {
            timer->stop();
            gateway = st_addr;
            //qDebug() << gateway;
            emit this->stageSucceded( QString( "%1:%2;\n%3\n" )
                                         .arg( st_addr.toString() )
                                         .arg( senderPort )
                                         .arg( datagram.data() ) );

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
                            if ( reader.readElementText() == QString( "urn:schemas-upnp-org:ser"
                                                                      "vice:WANIPConnection:1" ) )
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
                                    //qDebug() << gatewayCtrlUrl;
                                    break;
                                }
                            }
                            else
                                reader.readNext();
                        }
                        else
                            reader.readNext();
                    }
                    this->getExternalIP();

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
    udpSocket->disconnect();
    timer->disconnect();

    timer->stop();
    emit this->error( "Time expired!" );
}

void UPNP::getExternalIP()
{
    QString message{ "<?xml version=\"1.0\"?>"
                     "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                     "SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                     "<SOAP-ENV:Body><m:GetExternalIPAddress xmlns:m=\"urn:schemas-upnp-org:service:WANIPConnection:1\"/>"
                     "</SOAP-ENV:Body></SOAP-ENV:Envelope>" };

    this->postSOAP( "GetExternalIPAddress", message );
}

void UPNP::postSOAP( QString action, QString message )
{
    emit this->stageSucceded( QString( "POST: \nAction: " ) + action + QString( "\nMessage: " ) + message + QString( "\n\n" ) );

    QNetworkRequest req( gatewayCtrlUrl );
                    req.setRawHeader( QByteArray( "Cache-Control" ), "no-cache" );
                    req.setRawHeader( QByteArray( "Connection" ), "Close" );
                    req.setRawHeader( QByteArray( "Pragma" ), "no-cache" );
                    req.setRawHeader( QByteArray( "Content-Type" ), "text/xml; charset=\"utf-8\"" );
                    req.setRawHeader( QByteArray( "User-Agent" ), "User-Agent: Microsoft-Windows/10.0 UPnP/1.0" );
                    req.setRawHeader( QByteArray( "Content-Length" ), QString::number( message.size() ).toLatin1() );
                    req.setRawHeader( QByteArray( "SOAPAction" ), ( QString( "\"urn:schemas-upnp-org:service:WANIPConnection:1#" ) + action + QString( "\"" ) ).toLatin1() );
                    req.setRawHeader( QByteArray( "Host" ), QString( gateway.toString() + QString( ":" ) + ctrlPort ).toLatin1() );

    httpReply = httpSocket->post( req, message.toLatin1() );
    QObject::connect( httpReply, &QNetworkReply::readyRead, [=]()
    {
        QString reply = httpReply->readAll();
        if ( !reply.contains( "UPnPError" ) )
        {
            if ( reply.contains( "<NewExternalIPAddress>" ) )
                this->extractExternalIP( reply );

            if ( reply.contains( "DeletePortMappingResponse" ) )
            {
                connState = State::Closed;
                emit this->removedTunnel();
            }

            if ( reply.contains( "AddPortMappingResponse" ) )
            {
                connState = State::Opened;
                refreshTunnel->start();
                emit this->success();
            }
        }
        else
            this->extractError( reply );

        emit this->stageSucceded( reply + "\n" );
    });

    QObject::connect( httpReply, &QNetworkReply::readyRead, [=]()
    {
        emit this->error( httpReply->errorString() );
    });
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
    {
        externalAddress = QHostAddress( reader.readElementText() );
    }
    emit this->addressExtracted( externalAddress );
    this->checkTunnels();
}

void UPNP::extractError( QString message )
{
    QXmlStreamReader reader( message );
    reader.readNext();
    while ( reader.name().toString() != QString( "errorDescription" ) )
    {
        reader.readNext();
    }

    if ( reader.name().toString() == QString( "errorDescription" ) )
    {
        if ( reader.readElementText() == QString( "NoSuchEntryInArray" ) )
            this->setTunnel();
        else
            emit this->error( reader.readElementText() );
    }
}

void UPNP::checkTunnels()
{
    QString message( "<?xml version=\"1.0\"?>"
                     "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                     "<SOAP-ENV:Body><m:GetSpecificPortMappingEntry xmlns:m=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
                     "<NewRemoteHost xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\"></NewRemoteHost>"
                     "<NewExternalPort xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui2\">%1</NewExternalPort>"
                     "<NewProtocol xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">%2</NewProtocol>"
                     "</m:GetSpecificPortMappingEntry></SOAP-ENV:Body></SOAP-ENV:Envelope>" );

    message = message.arg( QString::number( externalPort ) )
                     .arg( pcol );

    this->postSOAP( "GetSpecificPortMappingEntry", message );
}

void UPNP::setTunnel()
{
    QString message{ "<?xml version=\"1.0\"?>"
                     "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                     "<SOAP-ENV:Body><m:AddPortMapping xmlns:m=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
                     "<NewRemoteHost xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\"></NewRemoteHost>"
                     "<NewExternalPort xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui2\">%1</NewExternalPort>"
                     "<NewProtocol xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">%2</NewProtocol>"
                     "<NewInternalPort xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui2\">%3</NewInternalPort>"
                     "<NewInternalClient xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">%4</NewInternalClient>"
                     "<NewEnabled xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"boolean\">1</NewEnabled>"
                     "<NewPortMappingDescription xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">%5</NewPortMappingDescription>"
                     "<NewLeaseDuration xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui4\">%6</NewLeaseDuration></m:AddPortMapping>"
                     "</SOAP-ENV:Body></SOAP-ENV:Envelope>" };

    message = message.arg( QString::number( externalPort ) )
                     .arg( pcol )
                     .arg( internalPort )
                     .arg( localAddress.ip().toString() )
                     .arg( info )
                     .arg( QString::number( UPNP_TIME_OUT_S ) );

    this->postSOAP( "AddPortMapping", message );
}

void UPNP::removeTunnel()
{
    QString message( "<?xml version=\"1.0\"?>"
                     "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                     "<SOAP-ENV:Body><m:DeletePortMapping xmlns:m=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
                     "<NewRemoteHost xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\"></NewRemoteHost>"
                     "<NewExternalPort xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui2\">%1</NewExternalPort>"
                     "<NewProtocol xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">%2</NewProtocol>"
                     "</m:DeletePortMapping></SOAP-ENV:Body></SOAP-ENV:Envelope>" );

    message = message.arg( QString::number( externalPort ) )
                     .arg( pcol );

    this->postSOAP( "DeletePortMapping", message );
}

UPNP::State UPNP::getState() const
{
    return connState;
}
