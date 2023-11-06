#ifndef UPNPDEVICE_HPP
#define UPNPDEVICE_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QObject>
#include <QString>
#include <QUrl>

enum class XMLElements: qint32
{
    ServiceType, ServiceId, ControlURL, EventSubURL,
    SCPDURL, UPnPDeviceType, DeviceType, FriendlyName,
    Manufacturer, ManufacturerURL, ModelDescription,
    ModelName, ModelNumber, ModelURL, UDN,
    UPnPDeviceList, DeviceList, ServiceList,
    UPnPDevice, Device, Service
};

class UPnPDevice : public QObject
{
    Q_OBJECT

    static const QMap<XMLElements, QString> xmlElementMap;

    public:
        UPnPDevice(UPnPDevicePrivate* priv, UPnPDiscover* parent);
        ~UPnPDevice(){}

        QString getType() const;
        QString getFriendlyName() const;
        QString getManufacturer() const;
        QUrl getManufacturerUrl() const;
        QString getModelDescription() const;
        QString getModelName() const;
        QString getModelNumber() const;
        QUrl getModelUrl() const;
        QString getUdn() const;
        QString getUrlBase() const;

        QNetworkAccessManager* getNetManager() const;

        QVector<UPnPDevice*> getDevices() const;
        QVector<UPnPService*> getServices() const;

        UPnPService* findService(const QString& service) const;
        static UPnPService* parseService(QXmlStreamReader& xml, QObject* parent);
        static UPnPDevice* parseUPnPDevice(QXmlStreamReader& xml, UPnPDiscover* parent);

        static UPnPDevice* fromXml(const QByteArray& data, UPnPDiscover* parent);

        void setUrlBase(const QString& urlBase);

    private:
        UPnPDevicePrivate* devicePrivate;
};

class UPnPDevicePrivate
{
    public:
        QString type;
        QString friendlyName;
        QString manufacturer;
        QUrl manufacturerURL;
        QString modelDescription;
        QString modelName;
        QString modelNumber;
        QUrl modelUrl;
        QString udn;
        QString urlBase;
        UPnPDiscover* discover;
        QVector<UPnPDevice*> devices;
        QVector<UPnPService*> services;
};

#endif // UPNPDEVICE_HPP
