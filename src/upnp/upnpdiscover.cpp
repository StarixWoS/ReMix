
//Class includes.
#include "upnp/upnpdiscover.hpp"

//ReMix includes.
#include "upnp/upnpdevice.hpp"

//Qt Includes.
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUdpSocket>

UPnPDiscover::UPnPDiscover(QObject* parent)
    : QObject(parent),
      discoverPrivate( new UPnPDiscoverPrivate( this ) )
{
    discoverPrivate->netManager = new QNetworkAccessManager( this );

    QObject::connect( &discoverPrivate->udpSocket, &QUdpSocket::readyRead, this, [=,this]
    {
        auto* soc{ &discoverPrivate->udpSocket };
        if ( soc != nullptr )
        {
            while ( soc->hasPendingDatagrams() )
            {
                const qint64 dataSize{ discoverPrivate->udpSocket.pendingDatagramSize() };
                if ( dataSize == 0 ) //Zero width packet, ignore.
                    return;

                QByteArray data( static_cast<qint32>( dataSize ), 0 );
                if ( discoverPrivate->udpSocket.readDatagram( data.data(), dataSize ) == -1 )
                    return;

                discoverPrivate->parse( data, this );
            }
        }
    } );

    if ( !discoverPrivate->udpSocket.bind( QHostAddress::Any, 0 ) )
        qDebug() << "Cannot bind to UDP port" << discoverPrivate->udpSocket.errorString();
}

UPnPDiscover::~UPnPDiscover()
{
    delete discoverPrivate;
}

QNetworkAccessManager *UPnPDiscover::getNetManager() const
{
    return  discoverPrivate->netManager;
}

void UPnPDiscover::discoverInternetGatewayDevice()
{
    static const QString upnp_data{ "M-SEARCH * HTTP/1.1\r\n"
                                    "HOST: 239.255.255.250:1900\r\n"
                                    "ST:urn:schemas-upnp-org:device:InternetGatewayDevice:1\r\n"
                                    "MAN:\"ssdp:discover\"\r\n"
                                    "MX:3\r\n"
                                    "\r\n" };

    static const QString tr64_data{ "M-SEARCH * HTTP/1.1\r\n"
                                    "HOST: 239.255.255.250:1900\r\n"
                                    "ST:urn:dslforum-org:device:InternetGatewayDevice:1\r\n"
                                    "MAN:\"ssdp:discover\"\r\n"
                                    "MX:3\r\n"
                                    "\r\n" };

    discoverPrivate->udpSocket.writeDatagram( upnp_data.toLatin1(), upnp_data.length() + 1, QHostAddress::Broadcast, 1900 );
    discoverPrivate->udpSocket.writeDatagram( tr64_data.toLatin1(), tr64_data.length() + 1, QHostAddress::Broadcast, 1900 );
}

void UPnPDiscoverPrivate::parse(const QByteArray& data, UPnPDiscover* parent)
{
    QStringList lines{ QString::fromLatin1( data ).split( "\r\n" ) };
    QString server;
    QUrl location;

    QString line{ lines.first() };
    if ( !line.contains( QLatin1String( "HTTP" ), Qt::CaseInsensitive )
      && !line.contains( QLatin1String( "NOTIFY" ), Qt::CaseInsensitive )
      && !line.contains( QLatin1String( "200" ) ) )
    {
        return;
    }
    else if ( line.contains( QLatin1String( "M-SEARCH" ), Qt::CaseInsensitive ) )
    { //Ignore M-SEARCH
        return;
    }

    bool validDevice = false;
    for ( const QString& line : lines )
    {
        if ((line.contains( QLatin1String( "ST:" ), Qt::CaseInsensitive )
          || line.contains( QLatin1String( "NT:" ), Qt::CaseInsensitive ) )
          && line.contains( QLatin1String( "InternetGatewayDevice" ), Qt::CaseInsensitive ) )
        {
            validDevice = true;
            break;
        }
    }

    if ( !validDevice )
        return;

    for ( const QString& line : lines )
    {
        if ( line.startsWith( QLatin1String( "Location" ), Qt::CaseInsensitive ) )
        {
            location = QUrl( line.mid( line.indexOf( QLatin1Char( ':' ) ) + 1 ).trimmed() );
            if ( !location.isValid() )
                return;
        }
        else if ( line.startsWith( QLatin1String( "Server" ), Qt::CaseInsensitive ) )
        {
            server = line.mid( line.indexOf( QLatin1Char( ':' ) ) + 1 ).trimmed();
            if ( server.length() == 0 )
                return;
        }
    }

    QNetworkRequest request{ location };
    QNetworkReply* reply{ netManager->get( request ) };
    QObject::connect( reply, &QNetworkReply::finished, this, [=, this]
    {
        const QByteArray data{ reply->readAll() };
        UPnPDevice* dev{ nullptr };
        if ( !reply->error() )
        {
            dev = UPnPDevice::fromXml( data, parent );
            if ( dev != nullptr
              && dev->getUrlBase().isEmpty() )
            {
                dev->setUrlBase( location.toString() );
                emit discover->discovered( dev );
            }
        }
        reply->deleteLater();
    } );
}
