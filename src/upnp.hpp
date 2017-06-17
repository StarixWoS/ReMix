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
        enum State {
            Opened = 0,
            NotOpened,
            Closed
        };

    private:
        QNetworkAccessManager* httpSocket;
        QNetworkReply* httpReply;
        QUdpSocket* udpSocket;
        QTimer* refreshTunnel;
        QTimer* timer;

        QNetworkAddressEntry localAddress;
        QHostAddress externalAddress;
        QHostAddress gateway;
        QUrl gatewayCtrlUrl;
        int internalPort;
        int externalPort;
        QString ctrlPort;
        State connState;
        QString info;
        QString pcol;

    public:
        explicit UPNP(QHostAddress localip, QObject* parent = 0);
        ~UPNP();

    public:
        void makeTunnel(int internal, int external,
                        QString protocol, QString text = "Tunnel ");

        void setTunnel();
        void removeTunnel();
        State getState() const;
        QHostAddress getExternalAddress() const;

    private:
        void getExternalIP();
        void checkTunnels();
        void extractExternalIP(QString message);
        void extractError(QString message);
        void postSOAP(QString action, QString message);

    private slots:
        void getUdp();
        void timeExpired();

    signals:
        void success();
        void stageSucceded(QString stage);
        void udpResponse();
        void addressExtracted(QHostAddress);
        void error(QString message);
        void createdTunnel();
        void removedTunnel();
};

#endif // UPNP_HPP
