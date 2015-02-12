
#ifndef SERVER_HPP
#define SERVER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QTcpServer>
#include <QTimer>

class Server : public QTcpServer
{
    Q_OBJECT

    QStandardItemModel* plrViewModel{ nullptr };
    QHash<QString, QStandardItem*> plrTableItems;
    QHash<QHostAddress, QByteArray> udpDatas;

    QWidget* mother{ nullptr };
    QUdpSocket* masterSocket{ nullptr };
    QByteArray udpData;
    QTimer masterCheckIn;

    UserMessage* serverComments{ nullptr };
    CmdHandler* cmdHandle{ nullptr };
    ServerInfo* server{ nullptr };
    Admin* admin{ nullptr };

#ifdef DECRYPT_PACKET_PLUGIN
    PacketDecryptInterface* packetInterface{ nullptr };
    QPluginLoader* pluginManager{ nullptr };
    bool loadPlugin();
#endif

    public:
        Server(QWidget* parent = nullptr, ServerInfo* svr = nullptr,
               Admin* adminDlg = nullptr,
               QStandardItemModel* plrView = nullptr );
        ~Server();

        void checkBannedInfo(Player* plr = nullptr);
        void detectPacketFlood(Player* plr);

        void parseMasterServerResponse(QByteArray& mData);
        void setupServerInfo();
        void setupPublicServer(bool value);

        QStandardItem* updatePlrListRow(QString& peerIP, QByteArray& data,
                                        Player* plr, bool insert);

        void showServerComments();

    private:
        void parsePacket(QString& packet, Player* plr = nullptr);
        void parseSRPacket(QString& packet, Player* plr = nullptr);
        void parseMIXPacket(QString& packet, Player* plr = nullptr);

        void parseUDPPacket(QByteArray& udp, QHostAddress& ipAddr, qint16 port);

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

    signals:
    public slots:
        void sendRemoteAdminPwdReqSlot(Player* plr);

    private slots:
        void newConnectionSlot();
        void readyReadUDPSlot();
};

#endif // SERVER_HPP
