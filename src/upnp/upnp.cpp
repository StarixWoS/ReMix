
//Class includes.
#include "upnp/upnp.hpp"

//ReMix includes.
#include "upnp/upnpwanservice.hpp"
#include "upnp/upnpdiscover.hpp"
#include "upnp/upnpdevice.hpp"
#include "upnp/upnpreply.hpp"
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

//QStringList UPNP::schemas =
//{
//    "urn:schemas-wifialliance-org:service:WFAWLANConfig:1",
//    "urn:schemas-wifialliance-org:service:WFAWLANConfig:2",
//    "urn:schemas-upnp-org:device:InternetGatewayDevice:1",
//    "urn:schemas-upnp-org:device:InternetGatewayDevice:2",
//    "urn:schemas-wifialliance-org:device:WFADevice:1",
//    "urn:schemas-wifialliance-org:device:WFADevice:2",
//    "urn:schemas-upnp-org:service:WANIPConnection:1",
//    "urn:schemas-upnp-org:service:WANIPConnection:2",
//    "urn:schemas-upnp-org:service:WANPPPConnection:1",
//    "urn:schemas-upnp-org:service:WANPPPConnection:2",
//    "upnp:rootdevice",
//};

bool UPNP::isTunneled{ false };
UPNP* UPNP::upnp{ nullptr };

UPnPWanService* UPNP::upnpWanService{ nullptr };
UPnPDevice* UPNP::upnpDevice{ nullptr };

UPNP::UPNP(QObject* parent )
    : QObject( parent )
{
    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &UPNP::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );
}

UPNP::~UPNP()
{
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

void UPNP::makeTunnel()
{
    static QString logMsg{ "Sending UPNP UDP Query." };;
    emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );

    UPnPDiscover* discover{ new UPnPDiscover() };
                  discover->discoverInternetGatewayDevice();

    QObject::connect( discover, &UPnPDiscover::discovered, this, [=,this]( UPnPDevice* device )
    {
        if ( device != nullptr )
        {
            upnpDevice = device;
            upnpWanService = static_cast<UPnPWanService*>( device->findService( "WANIPConnection" ) );
            if ( upnpWanService == nullptr )
                upnpWanService = static_cast<UPnPWanService*>( device->findService( "WANPPPConnection" ) );

            if ( upnpWanService != nullptr )
            {
                qDebug() << upnpWanService->getType();
                if ( !this->getIsTunneled() )
                {
                    this->getExternalIP();
                    this->setIsTunneled( true );
                    emit this->upnpTunnelSuccessSignal();
                }
            }
        }
    } );
}

void UPNP::getExternalIP()
{
    QString logMsg{ "Sending UPnP Action [ getExternalIp ]." };
    emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );

    UPnPReply* reply{ upnpWanService->getExternalIp() };
    QObject::connect( reply, &UPnPReply::finished, this, [=, this]
    {
        if ( reply->getError() == true )
        {
            this->logActionError( "getExternalIp", reply );
        }
        else
        {
            QString logMsg{ "Got Reply from UPnP Action [ getExternalIp ] for [ %2 ]" };
                    logMsg = logMsg.arg( reply->getValue().toString() );

            emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
        }
        reply->disconnect();
        reply->deleteLater();
    });
}

void UPNP::upnpPortForwardSlot(const QString& privateIP, const quint16& port, const bool& insert)
{
    if ( insert )
    {
        if ( port >= 1 )
        {
            if ( !this->getIsTunneled() )
            {
                this->makeTunnel();

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
    else if ( this->getIsTunneled() ) //Only attempt removal if tunneled.
    {
        this->portForwardRemove( "TCP", port );
        this->portForwardRemove( "UDP", port );
    }
}

void UPNP::checkPortForward(const QString& protocol, const quint16& port)
{
    QString logMsg{ "Sending UPnP Action [ getSpecificPortMappingEntry ] for Port[ %1:%2 ]." };
            logMsg = logMsg.arg( protocol )
                           .arg( port );

    emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );

    auto type{ QAbstractSocket::TcpSocket };
    if ( Helper::cmpStrings( protocol, "UDP" ) )
        type = QAbstractSocket::UdpSocket;

    UPnPReply* reply = upnpWanService->getSpecificPortMappingEntry( port, type );
    QObject::connect( reply, &UPnPReply::finished, this, [=, this]
    {
        if ( reply->getError() == true )
            this->logActionError( "getSpecificPortMappingEntry", reply );
        else
            this->logActionReply( "getSpecificPortMappingEntry", protocol, port );

        reply->disconnect();
        reply->deleteLater();
    });
}

void UPNP::portForwardAdd(const QString& protocol, const QString& privateIP, const quint16& port, const bool& lifetime)
{
    qint32 life{ *Globals::UPNP_TIME_OUT_S };
    if ( lifetime )
        life = *Globals::UPNP_TIME_OUT_PERMA;

    QString logMsg{ "Sending UPnP Action [ addPortMapping ] for Port[ %1:%2 ]." };
            logMsg = logMsg.arg( protocol )
                           .arg( port );

    emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );

    auto type{ QAbstractSocket::TcpSocket };
    if ( Helper::cmpStrings( protocol, "UDP" ) )
        type = QAbstractSocket::UdpSocket;

    UPnPReply* reply = upnpWanService->addPortMapping( port, QHostAddress( privateIP ), port, type, "ReMix_" % protocol, true, life );
    QObject::connect( reply, &UPnPReply::finished, this, [=, this]
    {
        if ( reply->getError() == true )
        {
            if ( Helper::cmpStrings( "OnlyPermanentLeasesSupported", reply->getErrorString() ) )
                this->portForwardAdd( protocol, privateIP, port, true );
            else
                this->logActionError( "addPortMapping", reply );
        }
        else
            this->logActionReply( "addPortMapping", protocol, port );

        reply->disconnect();
        reply->deleteLater();
    } );
}

void UPNP::portForwardRemove(const QString& protocol, const quint16& port)
{
    QString logMsg{ "Sending UPnP Action [ deletePortMapping ] for Port[ %1:%2 ]." };
            logMsg = logMsg.arg( protocol )
                           .arg( port );

    emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );

    auto type{ QAbstractSocket::TcpSocket };
    if ( Helper::cmpStrings( protocol, "UDP" ) )
        type = QAbstractSocket::UdpSocket;

    UPnPReply* reply = upnpWanService->deletePortMapping( port, type );
    QObject::connect( reply, &UPnPReply::finished, this, [=, this]
    {
        if ( reply->getError() == true )
            this->logActionError( "deletePortMapping", reply );
        else
            this->logActionReply( "deletePortMapping", protocol, port );

        reply->disconnect();
        reply->deleteLater();
    } );
}

void UPNP::logActionReply(const QString& action, const QString& protocol, const int& port)
{
    QString logMsg{ "Got Reply from UPnP Action [ %1 ] for Port[ %2:%3 ]" };
            logMsg = logMsg.arg( action )
                           .arg( protocol )
                           .arg( port );

    emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
}

void UPNP::logActionError(const QString& action, const UPnPReply* reply)
{
    if ( reply == nullptr )
        return;

    QString logMsg{ "Got Error from UPnP Action [ %1 ][ %2 ]." };
            logMsg = logMsg.arg( action )
                           .arg( reply->getErrorString() );

    emit this->insertLogSignal( "UPNP", logMsg, LKeys::UPNPLog, true, true );
}
