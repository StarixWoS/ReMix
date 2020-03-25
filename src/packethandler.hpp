
#ifndef PACKETHANDLER_HPP
#define PACKETHANDLER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QObject>
#include <QTimer>

class PacketHandler : public QObject
{
    Q_OBJECT

    PacketForge* pktForge{ nullptr };
    CmdHandler* cmdHandle{ nullptr };
    ServerInfo* server{ nullptr };
    ChatView* chatView{ nullptr };

    QTimer masterCheckIn;

    public:
        PacketHandler(ServerInfo* svr, ChatView* chat);
        ~PacketHandler() override;

        void startMasterCheckIn();
        void stopMasterCheckIn();

        void parsePacket(const QByteArray& packet, Player* plr = nullptr);
        void parseUDPPacket(const QByteArray& udp, const QHostAddress& ipAddr, const quint16& port);

        bool checkBannedInfo(Player* plr) const;
        bool getIsBanned(const QString& serNum, const QString& ipAddr,
                         const QString& plrSerNum) const;

    private:
        void detectFlooding(Player* plr);

        void readMIX0(const QString& packet, Player* plr);
        void readMIX1(const QString& packet, Player* plr);
        void readMIX2(const QString& packet, Player* plr);
        void readMIX3(const QString& packet, Player* plr);
        void readMIX4(const QString& packet, Player* plr);
        void readMIX5(const QString& packet, Player* plr);
        void readMIX6(const QString& packet, Player* plr);
        void readMIX7(const QString& packet, Player* plr);
        void readMIX8(const QString& packet, Player* plr);
        void readMIX9(const QString& packet, Player* plr);
        void handleSSVReadWrite(const QString& packet, Player* plr, const bool write);

    signals:
        void newUserCommentSignal(const QString& sernum, const QString& alias, const QString& message);
        void signalSendPacketToPlayer(Player* plr, QTcpSocket* srcSocket, qint32 targetType, quint32 trgSerNum,
                                      quint32 trgScene, const QByteArray& packet);
};

#endif // PACKETHANDLER_HPP
