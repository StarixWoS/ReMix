
#ifndef SERVER_HPP
#define SERVER_HPP

#include <QStandardItemModel>
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

class ServerInfo;
class Player;

class Server : public QTcpServer
{
    Q_OBJECT

    QStandardItemModel* plrViewModel{ nullptr };
    QHash<QString, QStandardItem*> plrTableItems;
    QHash<QHostAddress, QByteArray> udpDatas;

    QUdpSocket* masterSocket{ nullptr };
    QByteArray udpData;
    QTimer masterCheckIn;

    ServerInfo* server{ nullptr };

    public:
        explicit Server(ServerInfo* svr = nullptr, QStandardItemModel* plrView = nullptr );
        ~Server();

        void parseMasterServerResponse(QByteArray& mData);
        void setupServerInfo();
        void setupPublicServer(bool value);
        void disconnectFromMaster();
        void sendServerInfo(QUdpSocket* socket, QHostAddress& socAddr, quint16 socPort);
        void sendUserList(QUdpSocket* socket, QHostAddress& socAddr, quint16 socPort);

        QStandardItem* updatePlrListRow(QString& peerIP, QByteArray& data, bool insert);

    private:
        void parsePacket(QString& packet, Player* plr = nullptr);
        void parseSRPacket(QString& packet, Player* plr = nullptr);
        void parseMIXPacket(QString& packet, Player* plr = nullptr);

    signals:

    private slots:
        void masterCheckInTimeOutSlot();
        void newConnectionSlot();
        void readyReadUDPSlot();
};

#endif // SERVER_HPP
