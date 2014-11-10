
#ifndef SERVER_HPP
#define SERVER_HPP

#include <QNetworkInterface>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QByteArray>
#include <QDateTime>
#include <QtEndian>
#include <QString>
#include <QDebug>
#include <QTimer>

struct ServerInfo;

class Server : public QTcpServer
{
    Q_OBJECT

    QHash<QHostAddress, QTcpSocket*> tcpSockets;
    QHash<QHostAddress, QByteArray> udpDatas;

    QUdpSocket* masterSocket{ nullptr };
    QByteArray udpData;
    QTimer masterCheckIn;

    ServerInfo* serverInfo{ nullptr };
    bool isPublic{ false };

    public:
        explicit Server(QObject *parent = 0);

        void parseMasterServerResponse(QByteArray& mData);
        void setupServerInfo(ServerInfo* svrInfo);
        void setupPublicServer(bool value);

    signals:

    private slots:
        void masterCheckInTimeoutSlot();
        void newConnectionSlot();
        void readyReadTCPSlot();
        void readyReadUDPSlot();
};

#endif // SERVER_HPP
