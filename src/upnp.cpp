
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
    "urn:schemas-wifialliance-org:service:WFAWLANConfig:1",
    "urn:schemas-wifialliance-org:service:WFAWLANConfig:2",
    "urn:schemas-upnp-org:device:InternetGatewayDevice:1",
    "urn:schemas-upnp-org:device:InternetGatewayDevice:2",
    "urn:schemas-wifialliance-org:device:WFADevice:1",
    "urn:schemas-wifialliance-org:device:WFADevice:2",
    "urn:schemas-upnp-org:service:WANIPConnection:1",
    "urn:schemas-upnp-org:service:WANIPConnection:2",
    "urn:schemas-upnp-org:service:WANPPPConnection:1",
    "urn:schemas-upnp-org:service:WANPPPConnection:2",
    "upnp:rootdevice",
};

QHostAddress UPNP::externalAddress;
QMap<qint32, bool> UPNP::permFwd;
bool UPNP::isTunneled{ false };
UPNP* UPNP::upnp{ nullptr };

UPNP::UPNP(QObject* parent )
    : QObject( parent )
{
    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &UPNP::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );

    localAddress.setIp( QHostAddress( Helper::getPrivateIP() ) );
    localAddress.setBroadcast( QHostAddress::Broadcast );

    httpSocket = new QNetworkAccessManager();
    udpSocket = new QUdpSocket();
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
    {
        upnp = new UPNP();

        QString msg{ "Creating UPNP Object." };
        emit upnp->insertLogSignal( "UPNP", msg, LKeys::UPNPLog, true, true );
    }

    return upnp;
}

bool UPNP::getIsTunneled()
{
    return isTunneled;
}

void UPNP::setIsTunneled(bool value)
{
    isTunneled = value;
}

void UPNP::makeTunnel(const QString& privateIP)
{
    QNetworkAddressEntry localAddr;
    if ( privateIP.isEmpty() )
        localAddr = localAddress;
    else
        localAddr.setIp( QHostAddress( privateIP ) );

    localAddr.setBroadcast( QHostAddress::Broadcast );

    QString discover{ "M-SEARCH * HTTP/1.1\r\n"
                      "HOST:239.255.255.250:1900\r\n"
                      "ST: upnp:rootdevice\r\n"
                      //"GatewayDevice:1\r\n"/*upnp:rootdevice*/
                      "Man:\"ssdp:discover\"\r\n"
                      "MX:3\r\n\r\n" };

    QObject::connect( udpSocket, &QUdpSocket::readyRead, this, &UPNP::getUdpSlot, Qt::UniqueConnection );
    udpSocket->bind( QHostAddress::Any, 0 );
    udpSocket->writeDatagram( discover.toLatin1(),
                              discover.size(),
                              localAddr.broadcast(),
                              1900 );

    QString logMsg{ "Sent UDP Query Response [ %1 ]." };
            logMsg = logMsg.arg( discover.trimmed() );

    emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
}

void UPNP::closeTunnel()
{
    if ( udpSocket != nullptr )
    {
        udpSocket->flush();
        udpSocket->close();
        udpSocket->deleteLater();
    }
    this->setIsTunneled( false );
    udpSocket = new QUdpSocket();
}

void UPNP::getUdpSlot()
{
    if ( udpSocket->state() != QAbstractSocket::BoundState )
        return;

    QString serviceResponse;
    while ( udpSocket->hasPendingDatagrams() )
    {
        QByteArray datagram;
        QHostAddress sender;
        quint16 senderPort;
        QString servicePort;

        QString logMsg{ "" };

        datagram.resize( static_cast<int>( udpSocket->pendingDatagramSize() ) );
        udpSocket->readDatagram( datagram.data(), datagram.size(),
                                 &sender, &senderPort );

        QHostAddress st_addr( sender.toIPv4Address() );
        if ( st_addr != localAddress.ip() )
        {
            gateway = st_addr;

            serviceResponse = datagram.data();
            if ( !serviceResponse.isEmpty() )
            {
                logMsg = "Got UDP Query Response [ %1 ].";
                logMsg = logMsg.arg( serviceResponse.trimmed() );

                emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
            }

            int index{ Helper::getStrIndex( serviceResponse, "Location" ) };
            if ( index != -1 )
            {
                serviceResponse.remove( 0, index + 10 );
                index = 0;

                while ( serviceResponse[ index ].isPrint() )
                {
                    index++;
                }

                serviceResponse.remove( index, serviceResponse.size() - index );
                servicePort = serviceResponse;

                servicePort.remove( 0, 5 );
                index = 0;
                while ( ( index <servicePort.size() )
                     && ( servicePort[ index ] != QChar( ':' ) ) )
                {
                    index++;
                }

                servicePort.remove( 0, index+1 );
                index = 0;
                while ( ( index<servicePort.size() )
                     && ( servicePort[ index ] != QChar( '/' ) ) )
                {
                    index++;
                }

                servicePort.remove( index, servicePort.size() - index );

                ctrlPort = servicePort;
                QObject::connect( httpSocket, &QNetworkAccessManager::finished, httpSocket,
                [=, this]( QNetworkReply* reply )
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

                    bool validSchema{ false };
                    while ( !reader.atEnd() )
                    {
                        if ( Helper::cmpStrings( reader.name().toString(), "friendlyName" ) )
                        {
                            logMsg = "Obtained Service Name [ %1 ].";
                            logMsg = logMsg.arg( reader.readElementText() );

                            emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
                        }

                        if ( Helper::cmpStrings( reader.name().toString(), "UDN" ) )
                        {
                            serviceUUID = reader.readElementText();

                            logMsg = "Obtained Service UUID [ %1 ].";
                            logMsg = logMsg.arg( serviceUUID );

                            emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
                        }

                        if ( Helper::cmpStrings( reader.name().toString(), QStringLiteral("serviceType") )
                          || Helper::cmpStrings( reader.name().toString(), "ST" ) )
                        {
                            rtrSchema = reader.readElementText();

                            logMsg = "Validating Control Schema [ %1 ].";
                            logMsg = logMsg.arg( rtrSchema );
                            emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );

                            validSchema = schemas.contains( rtrSchema );
                            if ( validSchema )
                            {
                                logMsg = "Obtained Valid Control Schema [ %1 ].";
                                logMsg = logMsg.arg( rtrSchema ) ;
                            }
                            else
                                logMsg = "Control Schema Validation Failed.";

                            emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
                        }

                        if ( ( !rtrSchema.isEmpty() && validSchema )
                          && Helper::cmpStrings( reader.name().toString(), "controlUrl" ))
                        {
                            static const QString controlUrlFormat{ "http://%1:%2%3" };
                            QString element{ reader.readElementText() };

                            QString controlURL = controlUrlFormat.arg( gateway.toString() )
                                                                 .arg( ctrlPort )
                                                                 .arg( element );
                            gatewayCtrlUrl = controlURL;

                            logMsg = "Constructing ControlURL [ " + controlURL + " ].";
                            emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );

                            //The default ControlURL is invalid, we'll attempt to manually rebuild it.
                            if ( gatewayCtrlUrl.toString().isEmpty() )
                            {
                                logMsg = "Obtained Invalid ControlURL. Attempting Reconstruction.";
                                logMsg = logMsg.arg( gatewayCtrlUrl.toString() );

                                emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );

                                if ( !element.startsWith( "/" ) )
                                    element = "/" % element; //Ensure the ControlElement contains a fwd-Slash.

                                //if ( !Helper::strContainsStr( element, "uuid" ) )
                                //    element = "/" % serviceUUID % element;

                                //http://192.168.0.1:1234/uuid:84f2572c-59bd-aa7f-1747-bd89603cb789/control?WFAWLANConfig
                                //It's possible that the URL should exclude the uuid. As I lack the proper router to test, I can only assume it is required.
                                controlURL = controlUrlFormat.arg( gateway.toString() )
                                                             .arg( ctrlPort )
                                                             .arg( element );
                                gatewayCtrlUrl = controlURL;

                                logMsg = "Obtained Reconstructed ControlURL [ %1 ].";
                            }
                            else
                                logMsg = "Obtained Valid ControlURL [ %1 ].";

                            logMsg = logMsg.arg( controlURL );
                            emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );

                            this->setIsTunneled( true );
                            emit this->upnpTunnelSuccessSignal();
                            break;
                        }
                        reader.readNext();
                    }
                    httpSocket->disconnect();
                } );

                logMsg = "Requesting Service Information from [ %1 ].";
                logMsg = logMsg.arg( serviceResponse );

                emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );

                httpSocket->get( QNetworkRequest( QUrl( serviceResponse ) ) );
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
    this->postSOAP( "GetExternalIPAddress", message, "", "" );

    QString logMsg{ "Getting ExternalIP." };

    emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
}

void UPNP::postSOAP(const QString& action, const QString& message, const QString& protocol, const QString& privateIP, const quint16& port)
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
            this->extractError( reply, privateIP, port, protocol );

        httpReply->disconnect();
        httpReply->deleteLater();
    } );
}

void UPNP::extractError(const QString& message, const QString& privateIP, const quint16& port, const QString& protocol )
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

    emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );

    if ( Helper::cmpStrings( reader.name().toString(), "errorDescription" ) )
    {
        QString elementText{ reader.readElementText() };
        if ( (Helper::cmpStrings( elementText, "NoSuchEntryInArray" ))
          || (Helper::cmpStrings( elementText, "Invalid Args" ) ))
        {
            this->portForwardAdd( protocol, privateIP, port );
            emit this->upnpPortCheckedSignal( protocol );
        }

        if ( Helper::cmpStrings( elementText, "OnlyPermanentLeasesSupported" ) )
        {
            logMsg = "Re-attempting port forward for Port[ %1:%2 ] as a permanent lease.";
            logMsg = logMsg.arg( protocol )
                           .arg( port );

            emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
            this->portForwardAdd( protocol, privateIP, port, true );
        }

        if ( Helper::cmpStrings( reader.readElementText(), "errorDescription" ) )
        {
            this->portForwardAdd( protocol, privateIP, port );
            emit this->upnpPortCheckedSignal( protocol );
        }
        else
            emit this->upnpErrorSignal( reader.readElementText() );
    }
}

void UPNP::upnpPortForwardSlot(const QString& privateIP, const quint16& port, const bool& insert)
{
    if ( insert )
    {
        if ( port >= 1 )
        {
            if ( !this->getIsTunneled() )
            {
                this->makeTunnel( privateIP );

                QObject::connect( this, &UPNP::upnpTunnelSuccessSignal, this,
                [=, this]()
                {
                    this->portForwardAdd( "TCP", privateIP, port );
                    this->portForwardAdd( "UDP", privateIP, port );
                });
            }
            else
            {
                this->portForwardAdd( "TCP", privateIP, port );
                this->portForwardAdd( "UDP", privateIP, port );
            }
        }
    }
    else
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

        emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
    }
}

void UPNP::checkPortForward(const QString& protocol, const QString& privateIP, const quint16& port)
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

    this->postSOAP( "GetSpecificPortMappingEntry", message, protocol, privateIP, port );

    QString logMsg{ "Checking Port[ %1:%2 ]." };
    logMsg = logMsg.arg( protocol )
                   .arg( port );

    emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
}

void UPNP::portForwardAdd(const QString& protocol, const QString& privateIP, const quint16& port, const bool& lifetime)
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
    qint32 timeout{ *Globals::UPNP_TIME_OUT_S };
    if ( lifetime )
    {
        timeout = *Globals::UPNP_TIME_OUT_PERMA;
        permFwd.insert( port, true );
    }

    message = message.arg( rtrSchema )
                     .arg( port )
                     .arg( protocol )
                     .arg( port )
                     .arg( privateIP )
                     .arg( "ReMix_" % QString::number( port ) % protocol )
                     .arg( timeout );

    this->postSOAP( "AddPortMapping", message, protocol, privateIP, port );

    if ( !lifetime )
    {
        QString logMsg{ "Adding Port[ %1:%2 ]." };
        logMsg = logMsg.arg( protocol )
                       .arg( port );

        emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
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

    this->postSOAP( "DeletePortMapping", message, protocol, "", port );

    QString logMsg{ "Removing Port[ %1:%2 ]." };
    logMsg = logMsg.arg( protocol )
                   .arg( port );

    emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
}

void UPNP::logActionReply(const QString& action, const QString& protocol, const int& port)
{
    QString logMsg{ "Got Reply from Action[ %1 ] for Port[ %2:%3 ]" };
            logMsg = logMsg.arg( action )
                           .arg( protocol )
                           .arg( port );

    emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
}
