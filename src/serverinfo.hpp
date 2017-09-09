
#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QElapsedTimer>
#include <QHostInfo>
#include <QVariant>
#include <QObject>
#include <QTimer>

class ServerInfo
{
    QUdpSocket* masterSocket{ nullptr };
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

    quint32 usageHours{ 0 };
    quint32 usageDays{ 0 };
    quint32 usageMins{ 0 };

    QTimer usageUpdate;

    quint32 playerCount{ 0 };
    QString serverID{ "" };

    bool isMaster{ false };
    bool isPublic{ false };

    bool sentUDPCheckin{ false };
    bool masterUDPResponse{ false };

    QTimer masterCheckIn;
    QTimer masterTimeOut;
    bool masterTimedOut{ false };

    qint64 masterPingSendTime{ 0 };
    qint64 masterPingRespTime{ 0 };
    qint32 masterPingCount{ 0 };
    double masterPingTrend{ 80 };
    double masterPingAvg{ 0 };
    double masterPing{ 0 };

    QString masterIP{ "" };
    quint16 masterPort{ 23999 };

    int versionID{ 41252 };

    QHostInfo hostInfo;

    QString gameName{ "WoS" };
    int gameId{ 0 };

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
    qint64 bytesIn{ 0 };
    qint64 baudIn{ 0 };

    qint64 bytesOut{ 0 };
    qint64 baudOut{ 0 };

    QString serverTabID{ "" };

    public:
        ServerInfo(QString svrID = "0");
        ~ServerInfo();

        void setupInfo();
        void setupUPNP(bool isDisable = false);

        void sendUDPData(QHostAddress& addr, quint16 port, QString& data);

        void sendServerInfo(QHostAddress& addr, quint16 port);
        void sendUserList(QHostAddress& addr, quint16 port, quint32 type = 0);
        void sendMasterInfo(bool disconnect = false);

        Player* createPlayer(int slot);
        Player* getPlayer(int slot);
        void deletePlayer(int slot);

        int getEmptySlot();
        int getSocketSlot(QTcpSocket* soc);
        int getQItemSlot(QStandardItem* index);

        void sendServerRules(Player* plr);
        void sendServerGreeting(Player* plr);
        void sendMasterMessage(QString packet, Player* plr = nullptr,
                               bool toAll = false);
        qint64 sendToAllConnected(QString packet);

        quint64 getUpTime() const;
        QTimer* getUpTimer();

        QString getInfo() const;
        void setInfo(const QString& value);

        QString getGameInfo() const;
        void setGameInfo(const QString& value);

        int getGameId() const;
        void setGameId(int value);

        QString getGameName() const;
        void setGameName(const QString& value);

        QHostInfo getHostInfo() const;

        int getVersionID() const;
        void setVersionID(int value);

        quint16 getMasterPort() const;
        void setMasterPort(quint16 value);

        QString getMasterIP() const;
        void setMasterIP(const QString& value);

        bool getIsPublic() const;
        void setIsPublic(bool value);

        bool getIsMaster() const;
        void setIsMaster(bool value);

        QString getServerID() const;
        void setServerID(QString value);

        quint32 getPlayerCount() const;
        void setPlayerCount(quint32 value);

        quint16 getPublicPort() const;
        void setPublicPort(quint16 value);

        QString getPublicIP() const;
        void setPublicIP(const QString& value);

        quint16 getPrivatePort() const;
        void setPrivatePort(quint16 value);

        QString getPrivateIP() const;
        void setPrivateIP(const QString& value);

        QString getName() const;
        void setName(const QString& value);

        bool getIsSetUp() const;
        void setIsSetUp(bool value);

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

        qint64 getBytesIn() const;
        void setBytesIn(const qint64& value);

        qint64 getBytesOut() const;
        void setBytesOut(const qint64& value);

        void setBaudIO(const qint64& bytes, qint64& baud);
        qint64 getBaudIn() const;
        qint64 getBaudOut() const;

        QUdpSocket* getMasterSocket() const;
        bool initMasterSocket(QHostAddress& addr, quint16 port);

        QString getMasterInfoHost() const;
        void setMasterInfoHost(const QString& value);

        bool getSentUDPCheckin() const;
        void setSentUDPCheckIn(bool value);

        bool getMasterUDPResponse() const;
        void setMasterUDPResponse(bool value);

        bool getMasterTimedOut();
        void setMasterTimedOut(bool value);

        void startMasterCheckIn();
        void stopMasterCheckIn();

        qint64 getMasterPingSendTime() const;
        void setMasterPingSendTime(const qint64& value);

        qint64 getMasterPingRespTime() const;
        void setMasterPingRespTime(const qint64& value);

        double getMasterPingTrend() const;
        void setMasterPingTrend(double value);

        double getMasterPingAvg() const;
        void setMasterPingAvg(const double& value);

        qint32 getMasterPingCount() const;
        void setMasterPingCount(const qint32& value);

        double getMasterPing() const;
        void setMasterPing();

        quint32 getUsageHours() const;
        quint32 getUsageDays() const;
        quint32 getUsageMins() const;

        Server* getTcpServer() const;
        void setTcpServer(Server* value);
};

#endif // SERVERINFO_HPP
