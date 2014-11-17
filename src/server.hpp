
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

struct ServerInfo;
class Server : public QTcpServer
{
    Q_OBJECT

    QStandardItemModel* plrViewModel{ nullptr };
    QHash<QString, QStandardItem*> plrTableItems;

    QHash<QString, QTcpSocket*> tcpSockets;
    QHash<QTcpSocket*, QByteArray> tcpDatas;
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

        QStandardItem* updatePlrListRow(QString& peerIP, QByteArray& data, bool insert);
    private:
        void parsePacket(QString& packet, QTcpSocket* socket = nullptr);
        void parseMIXPacket(QString& packet);
        void parseSRPacket(QString& packet, QTcpSocket* socket = nullptr);

    signals:

    private slots:
        void masterCheckInTimeOutSlot();
        void newConnectionSlot();
        void readyReadUDPSlot();
};

#endif // SERVER_HPP
