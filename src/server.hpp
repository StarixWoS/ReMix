
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
#include <QDir>

//#ifdef DECRYPT_PACKET_PLUGIN
//    #include <QCoreApplication>
//    #include <QPluginLoader>

//    #include "packetdecryptinterface.hpp"
//#endif

class UserMessage;
class ServerInfo;
class Player;
class Admin;

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
    UserMessage* serverComments{ nullptr };
    Admin* admin{ nullptr };

//#ifdef DECRYPT_PACKET_PLUGIN
//    PacketDecryptInterface* packetInterface{ nullptr };
//    QPluginLoader* pluginManager{ nullptr };
//    bool loadPlugin();
//#endif

    public:
        Server(QWidget* parent = nullptr, ServerInfo* svr = nullptr, Admin* adminDlg = nullptr,
               QStandardItemModel* plrView = nullptr );
        ~Server();

        void checkBannedInfo(Player* plr = nullptr);
        void detectPacketFlood(Player* plr);

        void parseMasterServerResponse(QByteArray& mData);
        void setupServerInfo();
        void setupPublicServer(bool value);
        void disconnectFromMaster();
        void sendServerInfo(QUdpSocket* socket, QHostAddress& socAddr, quint16 socPort);
        void sendUserList(QUdpSocket* soc, QHostAddress& addr, quint16 port);
        quint64 sendServerRules(Player* plr);

        QStandardItem* updatePlrListRow(QString& peerIP, QByteArray& data, Player* plr, bool insert);

        void showServerComments();

    private:
        void parsePacket(QString& packet, Player* plr = nullptr);
        void parseSRPacket(QString& packet, Player* plr = nullptr);
        void parseMIXPacket(QString& packet, Player* plr = nullptr);

        void readMIX0(QString& packet, Player* plr);
        void readMIX1(QString& packet, Player* plr);
        void readMIX2(QString& packet, Player* plr);
        void readMIX3(QString& packet, Player* plr);
        void readMIX4(QString& packet, Player* plr);
        void readMIX5(QString& packet, Player* plr);
        void readMIX6(QString& packet, Player* plr);
        void readMIX7(QString& packet, Player* plr);
        void readMIX8(QString& packet, Player* plr);
        void readMIX9(QString& packet, Player* plr);

        void sendRemoteAdminPwdReq(Player* plr, QString& serNum);
        void authRemoteAdmin(Player* plr, quint32 id );

    signals:
        void newUserCommentSignal(QString& sernum, QString& alias, QString& message);

    private slots:
        void masterCheckInTimeOutSlot();
        void newConnectionSlot();
        void readyReadUDPSlot();
};

#endif // SERVER_HPP
