#ifndef UPNP_HPP
#define UPNP_HPP

#include "prototypes.hpp"

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
        static QStringList schemas;
        static QHostAddress externalAddress;
        static QVector<qint32> ports;
        static QMap<qint32, bool> permFwd;
        static bool tunneled;
        static UPNP* upnp;

    private:
        QNetworkAccessManager* httpSocket;
        QUdpSocket* udpSocket;
        QTimer* refreshTunnel;

        QNetworkAddressEntry localAddress;
        QHostAddress gateway;
        QHostAddress localIP;
        QUrl gatewayCtrlUrl;

        QString rtrSchema{ "urn:schemas-upnp-org:service:WANIPConnection:1" };
        QString ctrlPort{ "" };

    public:
        explicit UPNP(QObject* parent = nullptr);
        ~UPNP() override;

    public:
        void makeTunnel();

        void checkPortForward(const QString& protocol, const qint32& port);
        void addPortForward(const QString& protocol, const qint32& port, const bool& lifetime = false);
        void removePortForward(const QString& protocol, const qint32& port);

        static UPNP* getInstance();

        static bool getTunneled();
        static void setTunneled(bool value);

    private:
        void getExternalIP();
        void extractExternalIP(const QString& action, const QString& message);
        void postSOAP(const QString& action, const QString& message, const QString& protocol, const qint32& port = 0);
        void extractError(const QString& message, const qint32& port, const QString& protocol);

    private slots:
        void getUdpSlot();

    signals:
        void removedPortForward(const qint32 port, const QString& protocol);
        void addedPortForward(const qint32 port, const QString& protocol);
        void checkedPortForward(const qint32 port, const QString protocol);
        void success();
        void udpResponse();
        void error(const QString& message);
        void createdTunnel();

        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine);
};

#endif // UPNP_HPP
