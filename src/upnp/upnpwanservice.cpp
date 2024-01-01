
//Class includes.
#include "upnpwanservice.hpp"

//ReMix includes.
#include "upnp/upnpsoap.hpp"
#include "upnp/upnpdevice.hpp"
#include "upnp/upnpreply.hpp"

//Qt Includes.
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDomDocument>
#include <QUrl>

UPnPWanService::UPnPWanService(UPnPServicePrivate* priv, QObject* parent)
    : UPnPService(priv, parent){}

UPnPReply* UPnPWanService::addPortMapping(quint16 externalPort, const QString& internalAddress, quint16 internalPort,
                                          QAbstractSocket::SocketType sockType, const QString& description, bool enabled,
                                          int leaseDuration, const QString& remoteHost)
{
    UPnPReply* ret{ new UPnPReply( this ) };
    UPnPSoap envelope( QStringLiteral("AddPortMapping"), getType());
             envelope.writeTextElement( QStringLiteral( "NewRemoteHost" ), remoteHost );
             envelope.writeTextElement( QStringLiteral( "NewExternalPort" ), QString::number( externalPort ) );
             envelope.writeTextElement( QStringLiteral( "NewProtocol" ),
                                        sockType == QAbstractSocket::TcpSocket ? QStringLiteral( "TCP" ) : QStringLiteral( "UDP" ) );
             envelope.writeTextElement( QStringLiteral( "NewInternalPort" ), QString::number( internalPort ) );
             envelope.writeTextElement( QStringLiteral( "NewInternalClient" ), internalAddress );
             envelope.writeTextElement( QStringLiteral( "NewEnabled" ),
                                        enabled ? QStringLiteral( "1" ) : QStringLiteral( "0" ) );
             envelope.writeTextElement( QStringLiteral( "NewPortMappingDescription" ), description );
             envelope.writeTextElement( QStringLiteral( "NewLeaseDuration" ), QString::number( leaseDuration ) );

    UPnPDevice* device{ static_cast<UPnPDevice*>( this->parent() ) };
    if ( device == nullptr )
        return nullptr;

    QUrl url{ device->getUrlBase() };
         url.setPath( getControlUrl().path() );
         url.setQuery( getControlUrl().query() );

    QNetworkReply* reply{ device->getNetManager()->post( envelope.request( url ), envelope.render() ) };
    if ( reply != nullptr )
    {
        QObject::connect( reply, &QNetworkReply::finished, this, [=]
        {
            reply->deleteLater();
            const QByteArray data{ reply->readAll() };

            if ( reply->error() )
            {
                auto error{ UPnPSoap::responseError( data ) };
                ret->finishWithError( error.second, error.first );
            }
            else
                ret->finish();
        } );
    }
    return ret;
}

UPnPReply* UPnPWanService::addPortMapping(quint16 externalPort, const QHostAddress& internalAddress,
                                          quint16 internalPort, QAbstractSocket::SocketType sockType,
                                          const QString& description, bool enabled, int leaseDuration,
                                          const QHostAddress& remoteHost)
{
    return this->addPortMapping( externalPort, internalAddress.toString(), internalPort, sockType,
                                 description, enabled, leaseDuration, remoteHost.toString() );
}

UPnPReply* UPnPWanService::deletePortMapping(quint16 externalPort, QAbstractSocket::SocketType sockType, const QString& remoteHost)
{
    UPnPReply* ret{ new UPnPReply( nullptr ) };
    UPnPSoap envelope( QStringLiteral( "DeletePortMapping" ), getType() );
             envelope.writeTextElement( QStringLiteral( "NewRemoteHost" ), remoteHost );
             envelope.writeTextElement( QStringLiteral( "NewExternalPort" ), QString::number( externalPort ) );
             envelope.writeTextElement( QStringLiteral( "NewProtocol" ),
                                        sockType == QAbstractSocket::TcpSocket ? QStringLiteral( "TCP" ) : QStringLiteral( "UDP" ) );

    UPnPDevice* device{ static_cast<UPnPDevice*>( this->parent() ) };
    if ( device == nullptr )
        return nullptr;

    QUrl url{ device->getUrlBase() };
         url.setPath( getControlUrl().path() );
         url.setQuery( getControlUrl().query( ) );

    QNetworkReply* reply{ device->getNetManager()->post( envelope.request( url ), envelope.render() ) };
    QObject::connect( reply, &QNetworkReply::finished, this, [=]
    {
        reply->deleteLater();

        const QByteArray data{ reply->readAll() };
        if ( reply->error() )
        {
            auto error{ UPnPSoap::responseError( data ) };
            ret->finishWithError( error.second, error.first );
        }
        else
            ret->finish();
    } );
    return ret;
}

UPnPReply* UPnPWanService::deletePortMapping(quint16 externalPort, QAbstractSocket::SocketType sockType, const QHostAddress& remoteHost)
{
    return this->deletePortMapping( externalPort, sockType, remoteHost.toString() );
}

UPnPReply* UPnPWanService::getSpecificPortMappingEntry(quint16 externalPort, QAbstractSocket::SocketType sockType, const QString& remoteHost)
{
    UPnPReply* ret{ new UPnPReply( this ) };

    UPnPSoap envelope( QStringLiteral( "GetSpecificPortMappingEntry" ), getType() );
             envelope.writeTextElement( QStringLiteral( "NewRemoteHost" ), remoteHost );
             envelope.writeTextElement( QStringLiteral( "NewExternalPort" ), QString::number( externalPort ) );
             envelope.writeTextElement( QStringLiteral( "NewProtocol" ),
                                        sockType == QAbstractSocket::TcpSocket ? QStringLiteral( "TCP" ) : QStringLiteral( "UDP" ) );

    UPnPDevice* device{ static_cast<UPnPDevice*>( this->parent() ) };
    if ( device == nullptr )
        return nullptr;

    QUrl url{ device->getUrlBase() };
         url.setPath( getControlUrl().path() );
         url.setQuery( getControlUrl().query() );

    QNetworkReply* reply{ device->getNetManager()->post( envelope.request( url ), envelope.render() ) };
    if ( reply != nullptr )
    {
        QObject::connect( reply, &QNetworkReply::finished, this, [=]
        {
            reply->deleteLater();

            const QByteArray data{ reply->readAll() };
            if ( reply->error() )
            {
                auto error{ UPnPSoap::responseError( data ) };
                ret->finishWithError( error.second, error.first );
            }
            else
            {
                QDomDocument doc;
                             doc.setContent( data, true );

                const QDomElement res = doc.documentElement()
                                           .firstChildElement( QStringLiteral( "Body" ) )
                                           .firstChildElement( QStringLiteral( "GetSpecificPortMappingEntryResponse" ) );
                UPnPPortMap map;
                            map.internalPort = quint16( res.firstChildElement( QStringLiteral( "NewInternalPort" ) ).text().toUInt() );
                            map.internalAddress = res.firstChildElement( QStringLiteral( "NewInternalClient" ) ).text();
                            map.externalPort = externalPort;
                            map.sockType = sockType;
                            map.enabled = res.firstChildElement( QStringLiteral( "NewEnabled" ) ).text() == QLatin1String( "1" );
                            map.description = res.firstChildElement( QStringLiteral( "NewPortMappingDescription" ) ).text();
                            map.leaseDuration = res.firstChildElement( QStringLiteral( "NewLeaseDuration" ) ).text().toInt();
                            map.remoteHost = remoteHost;

                ret->finishWithData( QVariant::fromValue( map ) );
            }
        } );
    }
    return ret;
}

UPnPReply* UPnPWanService::getSpecificPortMappingEntry(quint16 externalPort, QAbstractSocket::SocketType sockType, const QHostAddress &remoteHost)
{
    return this->getSpecificPortMappingEntry( externalPort, sockType, remoteHost.toString() );
}

UPnPReply* UPnPWanService::getGenericPortMapping()
{
    UPnPReply* ret{ new UPnPReply( this ) };
    this->getGenericPortMapping(ret);
    return ret;
}

UPnPReply* UPnPWanService::getStatusInfo()
{
    UPnPReply* ret{ new UPnPReply( this ) };

    UPnPSoap envelope{ QStringLiteral( "GetStatusInfo" ), getType() };

    UPnPDevice* device{ static_cast<UPnPDevice*>( this->parent() ) };
    if ( device == nullptr )
        return nullptr;

    QUrl url( device->getUrlBase() );
         url.setPath( getControlUrl().path() );
         url.setQuery( getControlUrl().query() );

    QNetworkReply* reply{ device->getNetManager()->post( envelope.request(url), envelope.render() ) };
    if ( reply != nullptr )
    {
        QObject::connect(reply, &QNetworkReply::finished, this, [=]
        {
            reply->deleteLater();

            const QByteArray data{ reply->readAll() };
            if ( reply->error() )
            {
                auto error = UPnPSoap::responseError( data );
                ret->finishWithError( error.second, error.first );
            }
            else
            {
                QDomDocument doc;
                             doc.setContent( data, true );

                const QDomElement res = doc.documentElement()
                                           .firstChildElement(QStringLiteral( "Body") )
                                           .firstChildElement(QStringLiteral( "GetStatusInfoResponse" ) );
                ret->finishWithData(
                            QVariantHash
                            {
                                { QStringLiteral( "ConnectionStatus" ), res.firstChildElement( QStringLiteral( "NewConnectionStatus" ) ).text() },
                                { QStringLiteral( "LastConnectionError" ), res.firstChildElement( QStringLiteral( "NewLastConnectionError" ) ).text() },
                                { QStringLiteral( "Uptime" ), res.firstChildElement( QStringLiteral( "NewUptime" ) ).text() },
                            });
            }
        } );
    }
    return ret;
}

UPnPReply* UPnPWanService::getExternalIp()
{
    UPnPReply* ret{ new UPnPReply( this ) };

    UPnPSoap envelope{ QStringLiteral( "GetExternalIPAddress" ), getType() };

    UPnPDevice* device{ static_cast<UPnPDevice*>( this->parent() ) };
    if ( device == nullptr )
        return nullptr;

    QUrl url( device->getUrlBase() );
         url.setPath( getControlUrl().path() );
         url.setQuery( getControlUrl().query() );

    QNetworkReply* reply{ device->getNetManager()->post( envelope.request( url ), envelope.render() ) };
    if ( reply != nullptr )
    {
        QObject::connect( reply, &QNetworkReply::finished, this, [=]
        {
            reply->deleteLater();

            QByteArray data{ reply->readAll() };
            if ( !reply->error() )
            {
                QDomDocument doc;
                             doc.setContent( data, true );
                const QDomElement res = doc.documentElement()
                                           .firstChildElement( QStringLiteral( "Body" ) )
                                           .firstChildElement( QStringLiteral( "GetExternalIPAddressResponse" ) )
                                           .firstChildElement( QStringLiteral( "NewExternalIPAddress" ) );
                ret->finishWithData( res.text() );
            }
            else
                ret->finishWithError( QStringLiteral( "error" ) );
        } );
    }
    return ret;
}

void UPnPWanService::getGenericPortMapping(UPnPReply* ret, int index, const QVector<UPnPPortMap>& portMaps)
{
    UPnPSoap envelope( QStringLiteral( "GetGenericPortMappingEntry" ), getType() );
             envelope.writeTextElement( QStringLiteral( "NewPortMappingIndex" ), QString::number( index ) );

    UPnPDevice* device{ static_cast<UPnPDevice*>( this->parent() ) };
    if ( device == nullptr )
        return;

    QUrl url( device->getUrlBase() );
         url.setPath( getControlUrl().path() );
         url.setQuery( getControlUrl().query() );

    QNetworkReply* reply{ device->getNetManager()->post( envelope.request( url ), envelope.render() ) };
    if ( reply != nullptr )
    {
        QObject::connect( reply, &QNetworkReply::finished, this, [=,this]
        {
            reply->deleteLater();

            const QByteArray data{ reply->readAll() };
            if ( !reply->error() )
            {
                QDomDocument doc;
                             doc.setContent( data, true );
                const QDomElement res = doc.documentElement()
                                           .firstChildElement( QStringLiteral( "Body" ) )
                                           .firstChildElement( QStringLiteral( "GetGenericPortMappingEntryResponse" ) );

                UPnPPortMap map;
                            map.internalPort = quint16( res.firstChildElement( QStringLiteral( "NewInternalPort" ) ).text().toUInt() );
                            map.internalAddress = res.firstChildElement( QStringLiteral( "NewInternalClient" ) ).text();
                            map.externalPort = quint16( res.firstChildElement( QStringLiteral( "NewExternalPort" ) ).text().toUInt() );
                            map.sockType = res.firstChildElement( QStringLiteral( "NewProtocol" ) ).text()
                                              == QLatin1String( "TCP" ) ? QAbstractSocket::TcpSocket : QAbstractSocket::UdpSocket;
                            map.enabled = res.firstChildElement( QStringLiteral( "NewEnabled" ) ).text() == QLatin1String( "1" );
                            map.description = res.firstChildElement( QStringLiteral( "NewPortMappingDescription" ) ).text();
                            map.leaseDuration = res.firstChildElement( QStringLiteral( "NewLeaseDuration" ) ).text().toInt();
                            map.remoteHost = res.firstChildElement( QStringLiteral( "NewRemoteHost" ) ).text();

                QVector<UPnPPortMap> maps{ portMaps };
                                     maps.push_back( map );

                this->getGenericPortMapping( ret, index + 1, maps );
            }
            else
            {
                auto error = UPnPSoap::responseError( data );
                if ( error.first == QLatin1String( "713" )
                  || error.second == QLatin1String( "SpecifiedArrayIndexInvalid" ) )
                {
                    ret->finishWithData( QVariant::fromValue( portMaps ) );
                }
                else
                    ret->finishWithError( error.second, error.first );
            }
        });
    }
}
