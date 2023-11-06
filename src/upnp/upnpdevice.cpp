
//Class includes.
#include "upnpdevice.hpp"

//ReMix includes.
#include "upnp/upnpwanservice.hpp"
#include "upnp/upnpwandevice.hpp"
#include "upnp/upnpdiscover.hpp"
#include "upnp/upnpservice.hpp"
#include "upnp/upnpigd.hpp"

//Qt Includes.
#include <QXmlStreamReader>
#include <QVector>

const QMap<XMLElements, QString> UPnPDevice::xmlElementMap =
{
    { XMLElements::ServiceType,      "serviceType"      },
    { XMLElements::ServiceId,        "serviceId"        },
    { XMLElements::ControlURL,       "controlURL"       },
    { XMLElements::EventSubURL,      "eventSubURL"      },
    { XMLElements::SCPDURL,          "SCPDURL"          },
    { XMLElements::UPnPDeviceType,   "UPnPDeviceType"   },
    { XMLElements::DeviceType,       "deviceType"       },
    { XMLElements::FriendlyName,     "friendlyName"     },
    { XMLElements::Manufacturer,     "manufacturer"     },
    { XMLElements::ManufacturerURL,  "manufacturerURL"  },
    { XMLElements::ModelDescription, "modelDescription" },
    { XMLElements::ModelName,        "modelName"        },
    { XMLElements::ModelNumber,      "modelNumber"      },
    { XMLElements::ModelURL,         "modelURL"         },
    { XMLElements::UDN,              "UDN"              },
    { XMLElements::UPnPDeviceList,   "UPnPDeviceList"   },
    { XMLElements::DeviceList,       "deviceList"       },
    { XMLElements::ServiceList,      "serviceList"      },
    { XMLElements::UPnPDevice,       "UPnPDevice"       },
    { XMLElements::Device,           "device"           },
    { XMLElements::Service,          "service"          },
};

class UPnPDiscover;
UPnPDevice::UPnPDevice(UPnPDevicePrivate* priv, UPnPDiscover* parent)
    : QObject( parent ),
      devicePrivate( priv )
{
    devicePrivate->discover = parent;

    for ( UPnPDevice* dev : devicePrivate->devices )
    {
        dev->setParent( this );
    }

    for ( UPnPService* srv : devicePrivate->services )
    {
        srv->setParent( this );
    }
}

QString UPnPDevice::getType() const
{
    return devicePrivate->type;
}

QString UPnPDevice::getFriendlyName() const
{
    return devicePrivate->friendlyName;
}

QString UPnPDevice::getManufacturer() const
{
    return devicePrivate->manufacturer;
}

QUrl UPnPDevice::getManufacturerUrl() const
{
    return devicePrivate->manufacturerURL;
}

QString UPnPDevice::getModelDescription() const
{
    return devicePrivate->modelDescription;
}

QString UPnPDevice::getModelName() const
{
    return devicePrivate->modelName;
}

QString UPnPDevice::getModelNumber() const
{
    return devicePrivate->modelNumber;
}

QUrl UPnPDevice::getModelUrl() const
{
    return devicePrivate->modelUrl;
}

QString UPnPDevice::getUdn() const
{
    return devicePrivate->udn;
}

QString UPnPDevice::getUrlBase() const
{
    return devicePrivate->urlBase;
}

QNetworkAccessManager* UPnPDevice::getNetManager() const
{
    return devicePrivate->discover->getNetManager();
}

QVector<UPnPDevice*> UPnPDevice::getDevices() const
{
    return devicePrivate->devices;
}

QVector<UPnPService*> UPnPDevice::getServices() const
{
    return devicePrivate->services;
}

UPnPService* UPnPDevice::findService(const QString& service) const
{
    for ( UPnPService* srv : this->getServices() )
    {
        if ( srv != nullptr )
        {
            const QString type{ srv->getType() };
            if ( type.contains( service ) )
                return srv;
        }
    }

    for ( UPnPDevice* upnpDevice : this->getDevices() )
    {
        UPnPService* srv{ upnpDevice->findService( service ) };
        if ( srv != nullptr )
            return srv;
    }
    return nullptr;
}

UPnPService* UPnPDevice::parseService(QXmlStreamReader& xml, QObject* parent)
{
    UPnPServicePrivate* privateService{ new UPnPServicePrivate() };
    while ( !xml.atEnd() )
    {
        QXmlStreamReader::TokenType type{ xml.readNext() };
        if ( type == QXmlStreamReader::StartElement )
        {
            XMLElements name{ xmlElementMap.key( xml.name().toString() ) };
            switch ( name )
            {
                case XMLElements::ServiceType:
                    privateService->type = xml.readElementText();
                break;
                case XMLElements::ServiceId:
                    privateService->id = xml.readElementText();
                break;
                case XMLElements::ControlURL:
                    privateService->controlurl = QUrl(xml.readElementText());
                break;
                case XMLElements::EventSubURL:
                    privateService->eventsuburl = QUrl(xml.readElementText());
                break;
                case XMLElements::SCPDURL:
                    privateService->scpdurl = QUrl(xml.readElementText());
                break;
                default:
                    xml.skipCurrentElement();
                break;
            }
        }
        else if ( type == QXmlStreamReader::EndElement )
            break;
    }

    if ( privateService->type == QLatin1String( "urn:schemas-upnp-org:service:WANPPPConnection:1" ) )
        return new UPnPWanService( privateService, parent );
    else if ( privateService->type == QLatin1String( "urn:schemas-upnp-org:service:WANIPConnection:1" ) )
        return new UPnPWanService( privateService, parent );

    return new UPnPService( privateService, parent );
}

UPnPDevice* UPnPDevice::parseUPnPDevice(QXmlStreamReader& xml, UPnPDiscover* parent)
{
    UPnPDevicePrivate* privateService{ new UPnPDevicePrivate() };
    while ( !xml.atEnd() )
    {
        QXmlStreamReader::TokenType type{ xml.readNext() };
        if ( type == QXmlStreamReader::StartElement )
        {
            XMLElements name{ xmlElementMap.key( xml.name().toString() ) };
            switch ( name )
            {
                case XMLElements::UPnPDeviceType:
                case XMLElements::DeviceType:
                    privateService->type = xml.readElementText();
                break;
                case XMLElements::FriendlyName:
                    privateService->friendlyName = xml.readElementText();
                break;
                case XMLElements::Manufacturer:
                    privateService->manufacturer = xml.readElementText();
                break;
                case XMLElements::ManufacturerURL:
                    privateService->manufacturerURL = QUrl(xml.readElementText());
                break;
                case XMLElements::ModelDescription:
                    privateService->modelDescription = xml.readElementText();
                break;
                case XMLElements::ModelName:
                    privateService->modelName = xml.readElementText();
                break;
                case XMLElements::ModelNumber:
                    privateService->modelNumber = xml.readElementText();
                break;
                case XMLElements::ModelURL:
                    privateService->modelUrl = QUrl(xml.readElementText());
                break;
                case XMLElements::UDN:
                    privateService->udn = xml.readElementText();
                break;
                case XMLElements::UPnPDeviceList:
                case XMLElements::DeviceList:
                    {
                        while ( !xml.atEnd() )
                        {
                            QXmlStreamReader::TokenType typeRoot{ xml.readNext() };
                            if ( typeRoot == QXmlStreamReader::StartElement )
                            {
                                XMLElements tName{ xmlElementMap.key( xml.name().toString() ) };
                                if ( tName == XMLElements::UPnPDevice
                                  || tName == XMLElements::Device )
                                {
                                    UPnPDevice* dev{ parseUPnPDevice( xml, parent ) };
                                    if ( dev != nullptr )
                                    {
                                        if ( !privateService->devices.contains( dev ) )
                                            privateService->devices.push_back( dev );
                                    }
                                }
                                else
                                    xml.skipCurrentElement();
                            }
                            else if ( typeRoot == QXmlStreamReader::EndElement )
                                break;
                        }
                    }
                break;
                case XMLElements::ServiceList:
                    {
                        while ( !xml.atEnd() )
                        {
                            QXmlStreamReader::TokenType typeRoot{ xml.readNext() };
                            if ( typeRoot == QXmlStreamReader::StartElement )
                            {
                                XMLElements tName{ xmlElementMap.key( xml.name().toString() ) };
                                if ( tName == XMLElements::Service )
                                {
                                    UPnPService* service{ parseService( xml, parent ) };
                                    if ( service != nullptr )
                                    {
                                        if ( !privateService->services.contains( service ) )
                                            privateService->services.push_back( service );
                                    }
                                }
                                else
                                    xml.skipCurrentElement();
                            }
                            else if ( typeRoot == QXmlStreamReader::EndElement )
                                break;
                        }
                    }
                break;
                default:
                    xml.skipCurrentElement();
                break;
            }
        }
        else if ( type == QXmlStreamReader::EndElement )
            break;
    }

    if ( privateService->type == QLatin1String( "urn:schemas-upnp-org:UPnPDevice:InternetGatewayUPnPDevice:1" ) )
        return new UPnPDevice( privateService, parent );
    else if ( privateService->type == QLatin1String( "urn:schemas-upnp-org:UPnPDevice:WANConnectionUPnPDevice:1" ) )
        return new UPnPDevice( privateService, parent );

    return new UPnPDevice( privateService, parent );
}

UPnPDevice* UPnPDevice::fromXml(const QByteArray &data, UPnPDiscover *parent)
{
    UPnPDevice *ret = nullptr;
    QString urlBase;
    QXmlStreamReader xml{ data };
    while ( !xml.atEnd() )
    {
        QXmlStreamReader::TokenType type{ xml.readNext() };
        if ( type == QXmlStreamReader::StartElement )
        {
            if ( xml.name() == QLatin1String( "root" ) )
            {
                while ( !xml.atEnd() )
                {
                    QXmlStreamReader::TokenType typeRoot{ xml.readNext() };
                    if ( typeRoot == QXmlStreamReader::StartElement )
                    {
                        if ( xml.name() == QLatin1String( "UPnPDevice" )
                          || xml.name() == QLatin1String( "device" ) )
                        {
                            ret = parseUPnPDevice( xml, parent );
                        }
                        else if ( xml.name() == QLatin1String( "URLBase" ) )
                            urlBase = xml.readElementText();
                        else
                            xml.skipCurrentElement();
                    }
                    else if ( typeRoot == QXmlStreamReader::EndElement )
                        break;
                }
            }
            else
                xml.skipCurrentElement();
        }
    }

    if ( xml.hasError() )
    {
        delete ret;
        return nullptr;
    }

    if ( ret != nullptr )
        ret->setUrlBase( urlBase );
    else
        return nullptr;

    return ret;
}

void UPnPDevice::setUrlBase(const QString& urlBase)
{
    devicePrivate->urlBase = urlBase;
    for ( UPnPDevice* dev : devicePrivate->devices )
    {
        if ( dev != nullptr
          && dev->getUrlBase().isEmpty() )
        {
            dev->setUrlBase( urlBase );
        }
    }
}
