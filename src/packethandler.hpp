
#ifndef PACKETHANDLER_HPP
#define PACKETHANDLER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QObject>
#include <QTimer>

class PacketHandler : public QObject
{
    Q_OBJECT

    CmdHandler* cmdHandle{ nullptr };
    ServerInfo* server{ nullptr };
    Admin* admin{ nullptr };

    QTimer masterCheckIn;


#ifdef DECRYPT_PACKET_PLUGIN
    PacketDecryptInterface* packetInterface{ nullptr };
    QPluginLoader* pluginManager{ nullptr };
    bool loadPlugin();
#endif

    public:
        PacketHandler(Admin* adm, ServerInfo* svr);
        ~PacketHandler();

        void startMasterCheckIn();
        void stopMasterCheckIn();

        void parsePacket(QString& packet, Player* plr = nullptr);
        void parseSRPacket(QString& packet, Player* plr = nullptr);
        void parseMIXPacket(QString& packet, Player* plr = nullptr);

        void parseUDPPacket(QByteArray& udp, QHostAddress& ipAddr, qint16 port,
                            QHash<QHostAddress, QByteArray>* bioHash);

    private:
        bool checkBannedInfo(Player* plr);
        void detectFlooding(Player* plr);

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
        void newUserCommentSignal(QString& sernum, QString& alias,
                                  QString& message);
};

#endif // PACKETHANDLER_HPP
