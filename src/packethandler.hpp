
#ifndef PACKETHANDLER_HPP
#define PACKETHANDLER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSharedPointer>
#include <QObject>
#include <QTimer>

class PacketHandler : public QObject
{
    Q_OBJECT

    static QHash<QSharedPointer<Server>, PacketHandler*> pktHandleInstanceMap;

    QSharedPointer<Server> server;
    ChatView* chatView{ nullptr };

    QTimer masterCheckIn;

    public:
        PacketHandler(QSharedPointer<Server> svr, ChatView* chat);
        ~PacketHandler() override;

        static PacketHandler* getInstance(QSharedPointer<Server> server);
        static void deleteInstance(QSharedPointer<Server> server);

        void startMasterCheckIn();
        void stopMasterCheckIn();

        bool parseTCPPacket(const QByteArray& packet, QSharedPointer<Player> plr);

        bool checkBannedInfo(QSharedPointer<Player> plr) const;
        bool getIsBanned(const QString& serNum, const QString& ipAddr, const QString& plrSerNum) const;

    private:
        void detectFlooding(QSharedPointer<Player> plr);
        bool validatePacketHeader(QSharedPointer<Player> plr, const QByteArray& pkt);

        void readMIX0(const QString& packet, QSharedPointer<Player> plr);
        void readMIX1(const QString& packet, QSharedPointer<Player> plr);
        void readMIX2(const QString& packet, QSharedPointer<Player> plr);
        void readMIX3(const QString& packet, QSharedPointer<Player> plr);
        void readMIX4(const QString& packet, QSharedPointer<Player> plr);
        void readMIX5(const QString& packet, QSharedPointer<Player> plr);
        void readMIX6(const QString& packet, QSharedPointer<Player> plr);
        void readMIX7(const QString& packet, QSharedPointer<Player> plr);
        void readMIX8(const QString& packet, QSharedPointer<Player> plr);
        void readMIX9(const QString& packet, QSharedPointer<Player> plr);
        void handleSSVReadWrite(const QString& packet, QSharedPointer<Player> plr, const SSVModes mode);

    public slots:
        void parsePacketSlot(const QByteArray& packet, QSharedPointer<Player> plr);

    signals:
        void newUserCommentSignal(const QString& sernum, const QString& alias, const QString& message);
        void sendPacketToPlayerSignal(QSharedPointer<Player> plr, qint32 targetType, qint32 trgSerNum, qint32 trgScene, const QByteArray& packet);

        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine) const;
        void insertChatMsgSignal(const QString& msg, const Colors& color, const bool& newLine);
};

#endif // PACKETHANDLER_HPP
