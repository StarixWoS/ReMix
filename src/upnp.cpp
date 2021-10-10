
//Class includes.
#include "upnp.hpp"

//ReMix includes.
#include "settings.hpp"
#include "logger.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QNetworkAccessManager>
#include <QHostAddress>
#include <QUdpSocket>
#include <QString>
#include <QObject>
#include <QtCore>

QStringList UPNP::schemas =
{
    "urn:schemas-upnp-org:device:InternetGatewayDevice:2",
    "urn:schemas-upnp-org:service:WANIPConnection:2",
    "urn:schemas-upnp-org:device:InternetGatewayDevice:1",
    "urn:schemas-upnp-org:service:WANIPConnection:1",
    "urn:schemas-upnp-org:service:WANPPPConnection:1",
    "urn:schemas-wifialliance-org:service:WFAWLANConfig:1",
    "urn:schemas-wifialliance-org:device:WFADevice:1",
    "upnp:rootdevice",
};

QHostAddress UPNP::externalAddress;
QMap<qint32, bool> UPNP::permFwd;
bool UPNP::tunneled{ false };
UPNP* UPNP::upnp{ nullptr };

UPNP::UPNP(QObject* parent )
    : QObject( parent )
{
    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &UPNP::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );
    localIP = QHostAddress( Helper::getPrivateIP() );

    //We have a broadcast IP address. We are unable to port forward.
    if ( localIP != QHostAddress::AnyIPv4 )
    {
        localAddress.setIp( localIP );
        localAddress.setBroadcast( QHostAddress::Broadcast );

        httpSocket = new QNetworkAccessManager();
        udpSocket = new QUdpSocket();

        QString logMsg{ "Creating UPNP Object." };
        emit this->insertLogSignal( "UPNP", logMsg, LogTypes::UPNP, true, true );
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

    upnp = nullptr;
}

UPNP* UPNP::getInstance()
{
    if ( upnp == nullptr )
        upnp = new UPNP();

    QString msg{ "Getting UPNP Object." };
    emit upnp->insertLogSignal( "UPNP", msg, LogTypes::UPNP, true, true );

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

void UPNP::makeTunnel()
{
    QString discover{ "M-SEARCH * HTTP/1.1\r\n"
                      "HOST:239.255.255.250:1900\r\n"
                      "ST: upnp:rootdevice\r\n"
                      //"GatewayDevice:1\r\n"/*upnp:rootdevice*/
                      "Man:\"ssdp:discover\"\r\n"
                      "MX:3\r\n\r\n" };

    QObject::connect( udpSocket, &QUdpSocket::readyRead, this, &UPNP::getUdpSlot, Qt::UniqueConnection );
    udpSocket->bind( localAddress.ip(), 0 );
    udpSocket->writeDatagram( discover.toLatin1(),
                              discover.size(),
                              localAddress.broadcast(),
                              1900 );

    QString logMsg{ "Sent UDP Query Response [ %1 ]." };
            logMsg = logMsg.arg( discover.trimmed() );

    emit this->insertLogSignal( "UPNP", logMsg, LogTypes::UPNP, true, true );
}

void UPNP::getUdpSlot()
{
    if ( udpSocket->state() != QAbstractSocket::BoundState )
        return;

    QString vs;
    while ( udpSocket->hasPendingDatagrams() )
    {
        QByteArray datagram;
        QHostAddress sender;
        quint16 senderPort;
        QString sport;

        QString logMsg{ "" };

        datagram.resize( static_cast<int>( udpSocket->pendingDatagramSize() ) );
        udpSocket->readDatagram( datagram.data(), datagram.size(),
                                 &sender, &senderPort );

        QHostAddress st_addr( sender.toIPv4Address() );
        if ( st_addr != localAddress.ip() )
        {
            gateway = st_addr;

            vs = datagram.data();
            if ( !vs.isEmpty() )
            {
                logMsg = "Got UDP Query Response [ %1 ].";
                logMsg = logMsg.arg( vs.trimmed() );

                emit this->insertLogSignal( "UPNP", logMsg, LogTypes::UPNP, true, true );
            }

            int index{ Helper::getStrIndex( vs, "Location" ) };
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
                while ( ( index <sport.size() )
                     && ( sport[ index ] != QChar( ':' ) ) )
                {
                    index++;
                }

                sport.remove( 0, index+1 );
                index = 0;
                while ( ( index<sport.size() )
                     && ( sport[ index ] != QChar( '/' ) ) )
                {
                    index++;
                }

                sport.remove( index, sport.size() - index );

                ctrlPort = sport;
                QObject::connect( httpSocket, &QNetworkAccessManager::finished, httpSocket,
                [=, this](QNetworkReply* reply)
                {
                    QString response{ reply->readAll() };
                    QString logMsg{ "" };

                    int i{ 0 };
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
                        if ( Helper::cmpStrings( reader.name().toString(), "serviceType" )
                          || Helper::cmpStrings( reader.name().toString(), "ST" ) )
                        {
                            rtrSchema = reader.readElementText();
                            bool validSchema{ false };
                            for ( const QString& schema : schemas )
                            {
//                                logMsg = "Comparing Control URL[ %1 ] with [ %2 ].";
//                                logMsg = logMsg.arg( rtrSchema )
//                                               .arg( schema );

//                                emit this->insertLogSignal( "UPNP", logMsg, LogTypes::UPNP, true, true );

                                validSchema = false;
                                if ( Helper::cmpStrings( rtrSchema, schema ) )
                                {
                                    validSchema = true;
                                    break;
                                }
                            }

                            if ( validSchema )
                            {
                                while ( ( !reader.atEnd() )
                                     && ( !Helper::cmpStrings( reader.name().toString(), "controlUrl" ) ) )
                                {
                                    reader.readNext();
                                }

                                if ( Helper::cmpStrings( reader.name().toString(), "controlUrl" ) )
                                {
                                    gatewayCtrlUrl = QString( "http://%1:%2%3" )
                                                         .arg( gateway.toString() )
                                                         .arg( ctrlPort )
                                                         .arg( reader.readElementText() );

                                    logMsg = "Got ControlURL[ %1 ].";
                                    logMsg = logMsg.arg( gatewayCtrlUrl.toString() );

                                    emit this->insertLogSignal( "UPNP", logMsg, LogTypes::UPNP, true, true );

                                    this->setTunneled( true );
                                    emit this->upnpTunnelSuccessSignal();
                                    break;
                                }
                            }
                            else
                                reader.readNext();
                        }
                        else
                            reader.readNext();
                    }

                    httpSocket->disconnect();
                } );

                logMsg = "Requesting Service Information from [ %1 ].";
                logMsg = logMsg.arg( vs );

                emit this->insertLogSignal( "UPNP", logMsg, LogTypes::UPNP, true, true );

                httpSocket->get( QNetworkRequest( QUrl( vs ) ) );
                udpSocket->close();
            }
        }
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

    QString logMsg{ "Getting ExternalIP." };

    emit this->insertLogSignal( "UPNP", logMsg, LogTypes::UPNP, true, true );
}

void UPNP::postSOAP(const QString& action, const QString& message, const QString& protocol, const quint16& port)
{
    QString soap{ "\"%1#%2\"" };
            soap = soap.arg( rtrSchema )
                       .arg( action );
    QString host{ "%1:%2" };
            host = host.arg( gateway.toString() )
                       .arg( ctrlPort );

    QNetworkRequest req( gatewayCtrlUrl );
                    req.setRawHeader( QByteArray( "Cache-Control" ), "no-cache" );
                    req.setRawHeader( QByteArray( "Connection" ), "Close" );
                    req.setRawHeader( QByteArray( "Pragma" ), "no-cache" );
                    req.setRawHeader( QByteArray( "Content-Type" ), "text/xml; charset=\"utf-8\"" );
                    req.setRawHeader( QByteArray( "User-Agent" ), "User-Agent: Microsoft-Windows/10.0 UPnP/1.1" );
                    req.setRawHeader( QByteArray( "Content-Length" ), QString::number( message.size() ).toLatin1() );
                    req.setRawHeader( QByteArray( "SOAPAction" ), soap.toLatin1() );
                    req.setRawHeader( QByteArray( "Host" ), host.toLatin1() );

    QNetworkReply* httpReply{ httpSocket->post( req, message.toLatin1() ) };
    QObject::connect( httpReply, &QNetworkReply::readyRead, httpReply,
    [=, this]()
    {
        QString reply{ httpReply->readAll() };
        if ( !Helper::strContainsStr( reply, "UPnPError" ) )
        {
            if ( Helper::strContainsStr( reply, "NewExternalIPAddress" ) )
            {
                this->extractExternalIP( action, reply );
            }

            if ( Helper::strContainsStr( reply, "DeletePortMappingResponse" ) )
            {
                this->logActionReply( action, protocol, port );
                emit this->upnpPortRemovedSignal( port, protocol );
            }

            if ( Helper::strContainsStr( reply, "AddPortMappingResponse" ) )
            {
                this->logActionReply( action, protocol, port );
                emit this->upnpPortAddedSignal( port, protocol );
            }
        }
        else
        {
            //Some modems and routers incorrectly announce "Invalid Args" in
            //lieu of a "NoSuchEntryInArray"
            //reply to a "GetSpecificPortMapping" request.
            //Try and add port forwards if our response holds this value.
            if ( Helper::strContainsStr( reply, "Invalid Args" ) )
            {
                this->portForwardAdd( protocol, port );
            }
            else
                this->extractError( reply, port, protocol );
        }
        httpReply->disconnect();
        httpReply->deleteLater();
    } );
}

void UPNP::extractError(const QString& message, const quint16& port, const QString& protocol )
{
    QXmlStreamReader reader( message );
    reader.readNext();
    while ( !Helper::cmpStrings( reader.name().toString(), "errorDescription" ) )
    {
        reader.readNext();
    }


    QString logMsg{ "Got Error for Port[ %1:%2 ] [ %3 ]" };
    logMsg = logMsg.arg( protocol )
                   .arg( port )
                   .arg( message );

    emit this->insertLogSignal( "UPNP", logMsg, LogTypes::UPNP, true, true );

    if ( Helper::cmpStrings( reader.name().toString(), "errorDescription" ) )
    {
        QString elementText{ reader.readElementText() };
        if ( (Helper::cmpStrings( elementText, "NoSuchEntryInArray" ))
          || (Helper::cmpStrings( elementText, "Invalid Args" ) ))
        {
            this->portForwardAdd( protocol, port );
            emit this->upnpPortCheckedSignal( protocol );
        }

        if ( Helper::cmpStrings( elementText, "OnlyPermanentLeasesSupported" ) )
        {
            logMsg = "Re-attempting port forward for Port[ %1:%2 ] as a permanent lease.";
            logMsg = logMsg.arg( protocol )
                           .arg( port );

            emit this->insertLogSignal( "UPNP", logMsg, LogTypes::UPNP, true, true );
            this->portForwardAdd( protocol, port, true );
        }

        if ( Helper::cmpStrings( reader.readElementText(), "errorDescription" ) )
        {
            this->portForwardAdd( protocol, port );
            emit this->upnpPortCheckedSignal( protocol );
        }
        else
            emit this->upnpErrorSignal( reader.readElementText() );
    }
}

void UPNP::upnpPortForwardSlot(const quint16& port, const bool& insert)
{
    if ( insert )
    {
        if ( !this->getTunneled()
          && port >= 1)
        {
            this->makeTunnel();
            QObject::connect( this, &UPNP::upnpTunnelSuccessSignal, this,
            [=, this]()
            {
                this->portForwardAdd( "TCP", port );
                this->portForwardAdd( "UDP", port );
            });
        }
        else if ( this->getTunneled() )
        {
            this->portForwardAdd( "TCP", port );
            this->portForwardAdd( "UDP", port );
        }
    }
    else if ( this->getTunneled() )
    {
        this->portForwardRemove( "TCP", port );
        this->portForwardRemove( "UDP", port );
    }
}

void UPNP::extractExternalIP(const QString& action, const QString& message )
{
    QXmlStreamReader reader( message );
                     reader.readNext();

    while ( !Helper::cmpStrings( reader.name().toString(), "NewExternalIPAddress" ) )
    {
        reader.readNext();
    }

    if ( Helper::cmpStrings( reader.name().toString(), "NewExternalIPAddress" ) )
    {
        externalAddress = QHostAddress( reader.readElementText() );

        QString logMsg{ "Got Reply from Action[ %1 ] for [ %2 ]" };
                logMsg = logMsg.arg( action )
                               .arg( externalAddress.toString() );

        emit this->insertLogSignal( "UPNP", logMsg, LogTypes::UPNP, true, true );
    }
}

void UPNP::checkPortForward(const QString& protocol, const quint16& port)
{
    QString message( "<?xml version=\"1.0\"?>"
                     "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                     "SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                     "<SOAP-ENV:Body><m:GetSpecificPortMappingEntry xmlns:m=\"%1\">"
                     "<NewRemoteHost xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\"></NewRemoteHost>"
                     "<NewExternalPort xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui2\">%2</NewExternalPort>"
                     "<NewProtocol xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">%3</NewProtocol>"
                     "</m:GetSpecificPortMappingEntry></SOAP-ENV:Body></SOAP-ENV:Envelope>" );

    message = message.arg( rtrSchema )
                     .arg( port )
                     .arg( protocol );

    this->postSOAP( "GetSpecificPortMappingEntry", message, protocol, port );

    QString logMsg{ "Checking Port[ %1:%2 ]." };
    logMsg = logMsg.arg( protocol )
                   .arg( port );

    emit this->insertLogSignal( "UPNP", logMsg, LogTypes::UPNP, true, true );
}

void UPNP::portForwardAdd(const QString& protocol, const quint16& port, const bool& lifetime)
{
    QString message{ "<?xml version=\"1.0\"?>"
                     "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                     "SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
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

    //Support for routers that only support lifetime leases.
    qint32 timeout{ static_cast<int>( Globals::UPNP_TIME_OUT_S ) };
    if ( lifetime )
    {
        timeout = static_cast<int>( Globals::UPNP_TIME_OUT_PERMA );
        permFwd.insert( port, true );
    }

    message = message.arg( rtrSchema )
                     .arg( port )
                     .arg( protocol )
                     .arg( port )
                     .arg( localAddress.ip().toString() )
                     .arg( "ReMix_" % QString::number( port ) % protocol )
                     .arg( timeout );

    this->postSOAP( "AddPortMapping", message, protocol, port );

    if ( !lifetime )
    {
        QString logMsg{ "Adding Port[ %1:%2 ]." };
        logMsg = logMsg.arg( protocol )
                       .arg( port );

        emit this->insertLogSignal( "UPNP", logMsg, LogTypes::UPNP, true, true );
    }
}

void UPNP::portForwardRemove(const QString& protocol, const quint16& port)
{
    QString message( "<?xml version=\"1.0\"?>"
                     "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                     "SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                     "<SOAP-ENV:Body><m:DeletePortMapping xmlns:m=\"%1\">"
                     "<NewRemoteHost xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\"></NewRemoteHost>"
                     "<NewExternalPort xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui2\">%2</NewExternalPort>"
                     "<NewProtocol xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">%3</NewProtocol>"
                     "</m:DeletePortMapping></SOAP-ENV:Body></SOAP-ENV:Envelope>" );

    message = message.arg( rtrSchema )
                     .arg( port )
                     .arg( protocol );

    this->postSOAP( "DeletePortMapping", message, protocol, port );

    QString logMsg{ "Removing Port[ %1:%2 ]." };
    logMsg = logMsg.arg( protocol )
                   .arg( port );

    emit this->insertLogSignal( "UPNP", logMsg, LogTypes::UPNP, true, true );
}

void UPNP::logActionReply(const QString& action, const QString& protocol, const int& port)
{
    QString logMsg{ "Got Reply from Action[ %1 ] for Port[ %2:%3 ]" };
            logMsg = logMsg.arg( action )
                           .arg( protocol )
                           .arg( port );

    emit this->insertLogSignal( "UPNP", logMsg, LogTypes::UPNP, true, true );
}
