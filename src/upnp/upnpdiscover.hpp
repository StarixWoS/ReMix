#ifndef UPNPDISCOVER_HPP
#define UPNPDISCOVER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUdpSocket>
#include <QObject>

class UPnPDiscover : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(UPnPDiscover)

    public:
        explicit UPnPDiscover(QObject* parent = nullptr);
        ~UPnPDiscover();

        QNetworkAccessManager* getNetManager() const;

    public slots:
        void discoverInternetGatewayDevice();

    signals:
        void discovered(UPnPDevice* device);

    private:
        UPnPDiscoverPrivate* discoverPrivate;
};

class UPnPDiscoverPrivate : public QObject
{
    Q_OBJECT

    public:
        UPnPDiscoverPrivate(UPnPDiscover* query)
            : discover(query),
              groupAddress( "239.255.255.250" )
        {}

        void parse(const QByteArray& data, UPnPDiscover* parent);

        UPnPDiscover* discover;
        QNetworkAccessManager* netManager;
        QUdpSocket udpSocket;
        QHostAddress groupAddress;
};

#endif // UPNPDISCOVER_HPP
