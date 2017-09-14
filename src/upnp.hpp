#ifndef UPNP_HPP
#define UPNP_HPP

#include <prototypes.hpp>

//Required Qt Includes.
#include <QNetworkAddressEntry>
#include <QNetworkReply>
#include <QHostAddress>
#include <QObject>
#include <QUrl>

class UPNP : public QObject
{
    Q_OBJECT

    public:
        static QString schemas[ UPNP_SCHEMA_COUNT ];
        static QVector<qint32> ports;
        static bool tunneled;
        static UPNP* upnp;

        static QString upnpLog;

    private:
        QNetworkAccessManager* httpSocket;
        QUdpSocket* udpSocket;
        QTimer* refreshTunnel;

        QNetworkAddressEntry localAddress;
        QHostAddress externalAddress;
        QHostAddress gateway;
        QHostAddress localIP;
        QUrl gatewayCtrlUrl;

        QString rtrSchema{ "urn:schemas-upnp-org:service:WANIPConnection:1" };
        QString ctrlPort{ "" };

    public:
        explicit UPNP(QObject* parent = 0);
        ~UPNP();

    public:
        void makeTunnel();

        void checkPortForward(QString protocol, qint32 port);
        void addPortForward(QString protocol, qint32 port);
        void removePortForward(QString protocol, qint32 port);

        static UPNP* getInstance();

        static bool getTunneled();
        static void setTunneled(bool value);

    private:
        void getExternalIP();
        void extractExternalIP(QString message);
        void postSOAP(QString action, QString message, QString protocol, qint32 port = 0);
        void extractError(QString message, qint32 port, QString protocol);

    private slots:
        void getUdp();

    signals:
        void removedPortForward(qint32 port, QString protocol);
        void addedPortForward(qint32 port, QString protocol);
        void checkedPortForward(qint32 port, QString protocol);
        void success();
        void udpResponse();
        void error(QString message);
        void createdTunnel();
};

#endif // UPNP_HPP
