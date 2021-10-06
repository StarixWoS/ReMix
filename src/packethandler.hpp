
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
    Server* server{ nullptr };
    ChatView* chatView{ nullptr };

    QTimer masterCheckIn;

    public:
        PacketHandler(Server* svr, ChatView* chat);
        ~PacketHandler() override;

        void startMasterCheckIn();
        void stopMasterCheckIn();

        void parseUDPPacket(const QByteArray& udp, const QHostAddress& ipAddr, const quint16& port);

        bool checkBannedInfo(Player* plr) const;
        bool getIsBanned(const QString& serNum, const QString& ipAddr, const QString& plrSerNum) const;

    private:
        void detectFlooding(Player* plr);
        bool validatePacketHeader(Player* plr, const QByteArray& pkt);

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
        void handleSSVReadWrite(const QString& packet, Player* plr, const SSVModes mode);

    public slots:
        void parsePacketSlot(const QByteArray& packet, Player* plr = nullptr);

    signals:
        void newUserCommentSignal(const QString& sernum, const QString& alias, const QString& message);
        void sendPacketToPlayerSignal(Player* plr, qint32 targetType, qint32 trgSerNum, qint32 trgScene, const QByteArray& packet);

        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine) const;
};

#endif // PACKETHANDLER_HPP
