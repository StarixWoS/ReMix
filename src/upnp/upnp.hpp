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
        static bool isTunneled;

        static UPnPWanService* upnpWanService;
        static UPnPDevice* upnpDevice;
        static UPNP* upnp;

    public:
        explicit UPNP(QObject* parent = nullptr);
        ~UPNP() override;

    public:
        void makeTunnel();

        void checkPortForward(const QString& protocol, const quint16& port);
        void portForwardAdd(const QString& protocol, const QString& privateIP, const quint16& port, const bool& lifetime = false);
        void portForwardRemove(const QString& protocol, const quint16& port);

        void logActionReply(const QString& action, const QString& protocol, const int& port);
        void logActionError(const QString& action, const UPnPReply* reply);

        static UPNP* getInstance();

        static bool getIsTunneled();
        static void setIsTunneled(bool value);

    private:
        void getExternalIP();

    public slots:
        void upnpPortForwardSlot(const QString& privateIP, const quint16& port, const bool& insert);

    signals:
        void upnpPortRemovedSignal(const quint16& port, const QString& protocol);
        void upnpPortAddedSignal(const quint16& port, const QString& protocol);
        void upnpPortCheckedSignal(const QString& protocol);
        void upnpTunnelSuccessSignal();

        void insertLogSignal(const QString& source, const QString& message, const LKeys& type, const bool& logToFile, const bool& newLine);
};

#endif // UPNP_HPP
