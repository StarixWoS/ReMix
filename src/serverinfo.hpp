
#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QElapsedTimer>
#include <QHostInfo>
#include <QVariant>
#include <QObject>
#include <QTimer>

class ServerInfo : public QObject
{
    Q_OBJECT

    QUdpSocket* masterSocket{ nullptr };
    PacketHandler* pktHandle{ nullptr };
    Server* tcpServer{ nullptr };
    UPNP* upnp{ nullptr };

    QString masterInfoHost{ "http://synthetic-reality.com/synreal.ini" };

    QTimer upTimer;
    QVariant upTime{ 0.0 };

    bool isSetUp{ false };
    QString name{ "AHitB ReMix Server" };

    QString privateIP{ "" };
    quint16 privatePort{ 8888 };

    QString publicIP{ "" };
    quint16 publicPort{ 8888 };

    quint32 usageArray[ SERVER_USAGE_48_HOURS ]{ 0 };
    quint32 usageCounter{ 0 };

    qint32 usageHours{ 0 };
    qint32 usageDays{ 0 };
    qint32 usageMins{ 0 };

    QTimer usageUpdate;

    quint32 playerCount{ 0 };
    QString serverID{ "" };

    bool isMaster{ false };
    bool isPublic{ false };
    bool useUPNP{ false };

    bool sentUDPCheckin{ false };
    bool masterUDPResponse{ false };

    QTimer masterCheckIn;
    QTimer masterTimeOut;
    bool masterTimedOut{ false };

    qint32 masterPingFailCount{ 0 };
    qint32 masterPingCount{ 0 };

    qint64 masterPingSendTime{ 0 };
    qint64 masterPingRespTime{ 0 };
    double masterPingTrend{ 80 };
    double masterPingAvg{ 0 };
    double masterPing{ 0 };

    QString masterIP{ "" };
    quint16 masterPort{ 23999 };

    int versionID{ 41252 };

    QHostInfo hostInfo;

    QString gameName{ "WoS" };
    Games gameId{ Games::Invalid };

    QString gameInfo{ "" };
    QString info{ "" };

    Player* players[ MAX_PLAYERS ];

    quint32 userCalls{ 0 };
    quint32 userPings{ 0 };
    quint32 serNumDc{ 0 };
    quint32 dupDc{ 0 };
    quint32 pktDc{ 0 };
    quint32 ipDc{ 0 };

    QElapsedTimer baudTime;
    quint64 bytesIn{ 0 };
    quint64 baudIn{ 0 };

    quint64 bytesOut{ 0 };
    quint64 baudOut{ 0 };

    public:
        ServerInfo();
        ~ServerInfo();

        void setupInfo();
        void setupUPNP(const bool& enable = false);

        void sendUDPData(const QHostAddress& addr, const quint16& port,
                         const QString& data);

        void sendServerInfo(const QHostAddress& addr, const quint16& port);
        void sendUserList(const QHostAddress& addr, const quint16& port,
                          const quint32& type = 0);
        void sendMasterInfo(const bool& disconnect = false);

        Player* createPlayer(const int& slot);
        Player* getPlayer(const int& slot);
        void deletePlayer(const int& slot);

        int getEmptySlot();
        int getSocketSlot(QTcpSocket* soc);
        int getQItemSlot(QStandardItem* index);

        void sendServerRules(Player* plr);
        void sendServerGreeting(Player* plr);
        void sendMasterMessage(const QString& packet, Player* plr = nullptr,
                               const bool toAll = false);
        qint64 sendToAllConnected(const QString& packet);

        quint64 getUpTime() const;
        QTimer* getUpTimer();

        QString getInfo() const;
        void setInfo(const QString& value);

        QString getGameInfo() const;
        void setGameInfo(const QString& value);

        Games getGameId() const;
        void setGameId(const QString& gameName);

        QString getGameName() const;
        void setGameName(const QString& value);

        QHostInfo getHostInfo() const;

        int getVersionID() const;
        void setVersionID(const int& value);

        quint16 getMasterPort() const;
        void setMasterPort(const quint16& value);

        QString getMasterIP() const;
        void setMasterIP(const QString& value);

        bool getIsPublic() const;
        void setIsPublic(const bool& value);

        bool getUseUPNP() const;
        void setUseUPNP(const bool& value);

        bool getIsMaster() const;
        void setIsMaster(const bool& value);

        QString getServerID() const;
        void setServerID(const QString& value);

        quint32 getPlayerCount() const;
        void setPlayerCount(const quint32& value);

        quint16 getPublicPort() const;
        void setPublicPort(const quint16& value);

        QString getPublicIP() const;
        void setPublicIP(const QString& value);

        quint16 getPrivatePort() const;
        void setPrivatePort(const quint16& value);

        QString getPrivateIP() const;
        void setPrivateIP(const QString& value);

        QString getName() const;
        void setName(const QString& value);

        bool getIsSetUp() const;
        void setIsSetUp(const bool& value);

        quint32 getUserCalls() const;
        void setUserCalls(const quint32& value);

        quint32 getUserPings() const;
        void setUserPings(const quint32& value);

        quint32 getSerNumDc() const;
        void setSerNumDc(const quint32& value);

        quint32 getDupDc() const;
        void setDupDc(const quint32& value);

        quint32 getPktDc() const;
        void setPktDc(const quint32& value);

        quint32 getIpDc() const;
        void setIpDc(const quint32& value);

        quint64 getBytesIn() const;
        void setBytesIn(const quint64 &value);

        quint64 getBytesOut() const;
        void setBytesOut(const quint64 &value);

        void setBaudIO(const quint64 &bytes, quint64 &baud);
        quint64 getBaudIn() const;
        quint64 getBaudOut() const;

        QUdpSocket* getMasterSocket() const;
        bool initMasterSocket(const QHostAddress& addr, const quint16& port);

        QString getMasterInfoHost() const;
        void setMasterInfoHost(const QString& value);

        bool getSentUDPCheckin() const;
        void setSentUDPCheckIn(const bool& value);

        bool getMasterUDPResponse() const;
        void setMasterUDPResponse(const bool& value);

        bool getMasterTimedOut() const;
        void setMasterTimedOut(const bool& value);

        void startMasterCheckIn();
        void stopMasterCheckIn();

        qint64 getMasterPingSendTime() const;
        void setMasterPingSendTime(const qint64& value);

        qint64 getMasterPingRespTime() const;
        void setMasterPingRespTime(const qint64& value);

        double getMasterPingTrend() const;
        void setMasterPingTrend(const double& value);

        double getMasterPingAvg() const;
        void setMasterPingAvg(const double& value);

        qint32 getMasterPingFailCount() const;
        void setMasterPingFailCount(const qint32& value);

        qint32 getMasterPingCount() const;
        void setMasterPingCount(const qint32& value);

        double getMasterPing() const;
        void setMasterPing();

        QString getUsageString();
        qint32 getUsageHours() const;
        qint32 getUsageDays() const;
        qint32 getUsageMins() const;

        Server* getTcpServer() const;
        void setTcpServer(Server* value);

        PacketHandler* getPktHandle() const;
        void setPktHandle(PacketHandler* value);

    signals:
        void serverIsSetup();
};

#endif // SERVERINFO_HPP
