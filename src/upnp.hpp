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
        static QHostAddress localGatewayIP;
        static QMap<qint32, bool> permFwd;
        static bool isTunneled;
        static UPNP* upnp;

    private:
        QNetworkAccessManager* httpSocket;
        QUdpSocket* udpSocket;
        QTimer* refreshTunnel;

        QNetworkAddressEntry localAddress;
        QHostAddress gateway;
        QUrl gatewayCtrlUrl;

        QString rtrSchema{ "urn:schemas-upnp-org:service:WANIPConnection:1" };
        QString ctrlPort{ "" };

    public:
        explicit UPNP(QObject* parent = nullptr);
        ~UPNP() override;

    public:
        void makeTunnel(const QString& privateIP = "");
        void closeTunnel();

        void checkPortForward(const QString& protocol, const QString& privateIP, const quint16& port);
        void portForwardAdd(const QString& protocol, const QString& privateIP, const quint16& port, const bool& lifetime = false);
        void portForwardRemove(const QString& protocol, const quint16& port);

        void logActionReply(const QString& action, const QString& protocol, const int& port);

        static UPNP* getInstance();

        static bool getIsTunneled();
        static void setIsTunneled(bool value);

    private:
        void getExternalIP();
        void extractExternalIP(const QString& action, const QString& message);
        void postSOAP(const QString& action, const QString& message, const QString& protocol, const QString& privateIP, const quint16& port = 0);
        void extractError(const QString& message, const QString& privateIP, const quint16& port, const QString& protocol);

    public slots:
        void upnpPortForwardSlot(const QString& privateIP, const quint16& port, const bool& insert);

    private slots:
        void getUdpSlot();

    signals:
        void upnpPortRemovedSignal(const quint16& port, const QString& protocol);
        void upnpPortAddedSignal(const quint16& port, const QString& protocol);
        void upnpPortCheckedSignal(const QString& protocol);
        void upnpTunnelSuccessSignal();
        void upnpErrorSignal(const QString& message);

        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine);
};

#endif // UPNP_HPP
