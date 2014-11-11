
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
    QByteArray tcpBuffer;
    QByteArray udpData;
    QTimer masterCheckIn;

    ServerInfo* serverInfo{ nullptr };

    bool isSetUp{ false };
    bool isPublic{ false };

    public:
        explicit Server(QObject *parent = 0);

        void parseMasterServerResponse(QByteArray& mData);
        void setupServerInfo(ServerInfo* svrInfo);
        void setupPublicServer(bool value);
        void disconnectFromMaster();

        bool getIsSetUp() const;
        void setIsSetUp(bool value);

    private:
        void parsePacket(QString& packet);
        void parseMIXPacket(QString& packet);
        void parseSRPacket(QString& packet);

    signals:

    private slots:
        void masterCheckInTimeOutSlot();
        void newConnectionSlot();
        void readyReadUDPSlot();
};

#endif // SERVER_HPP
