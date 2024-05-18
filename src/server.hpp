
#ifndef SERVER_HPP
#define SERVER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSharedPointer>
#include <QElapsedTimer>
#include <QTcpServer>
#include <QDateTime>
#include <QHostInfo>
#include <QVariant>
#include <QObject>
#include <QMutex>
#include <QTimer>

class Server : public QTcpServer
{
    Q_OBJECT

    QSharedPointer<Server> serverSharePtr;

    UdpThread* udpThread{ nullptr };
    QThread* thread{ nullptr };

    qint64 initializeDate{ 0 };

    QTimer upTimer;

    bool isSetUp{ false };
    QString serverName{ "AHitB ReMix Server" };

    QString privateIP{ "" };
    quint16 privatePort{ 8888 };

    QString publicIP{ "" };
    quint16 publicPort{ 8888 };

    qint32 usageArray[ *Globals::SERVER_USAGE_48_HOURS ]{ 0 };
    qint32 usageCounter{ 0 };

    QTimer usageUpdate;
    qint32 usageHours{ 0 };
    qint32 usageDays{ 0 };
    qint32 usageMins{ 0 };

    qint32 maxPlayerCount{ *Globals::MAX_PLAYERS };
    qint32 playerCount{ 0 };
    QString serverID{ "" };

    bool isPublic{ false };
    bool useUPNP{ false };

    bool sentUDPCheckin{ false };
    bool masterUDPResponse{ false };

    QTimer masterCheckIn;
    QTimer masterTimeOut;
    bool masterTimedOut{ false };
    bool masterInfoRecv{ false };

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

    QString gameWorld{ "" };

    qint32 peakPlayerCount{ 0 };
    QVector<QSharedPointer<Player>> players;
    QMap<qint32, QSharedPointer<Player>> plrSlotMap;

    quint32 userCalls{ 0 };
    quint32 userPings{ 0 };
    quint32 dupDc{ 0 };
    quint32 pktDc{ 0 };
    quint32 ipDc{ 0 };

    quint64 bytesOut{ 0 };
    quint64 bytesIn{ 0 };

    QTimer upnpPortRefresh;
    QTimer upnpPortTimeOut;
    bool upnpPortAdded{ false };
    bool upnpTimedOut{ false };

    QTimer masterSerNumKeepAliveTimer;

    public:
        Server(QWidget* parent = nullptr);
        ~Server() override;

        static void customDeconstruct(Server* svr);

        void incomingConnection(qintptr socketDescriptor) override;

        void setupInfo(const QString& interfaceIP = "");
        void setupUPNP(const bool& enable = false);

        void sendUserList(const QHostAddress& addr, const quint16& port, const UserListResponse& type);
        void sendMasterInfo(const bool& disconnect = false);

        QSharedPointer<Player> createPlayer(const qintptr& socketDescriptor, QSharedPointer<Server> server);
        QSharedPointer<Player> getPlayer(const int& slot);
        QSharedPointer<Player> getPlayer(const qintptr& socketDescriptor);
        QSharedPointer<Player> getPlayer(const QString& hexSerNum);
        qint32 getPlayerSlot(const QSharedPointer<Player> plr);
        void deletePlayer(QSharedPointer<Player> plr, const bool& all, const bool& timedOut = false);
        void deleteAllPlayers();

        QSharedPointer<Player> getLastPlayerInStorage(const QSharedPointer<Player> plr);
        int getEmptySlot();
        int getSocketSlot(const qintptr& socketDescriptor);

        void sendPlayerSocketInfo();
        void sendPlayerSocketPosition(QSharedPointer<Player> plr, const bool& forceIssue);
        void sendServerRules(QSharedPointer<Player> plr);
        void sendServerGreeting(QSharedPointer<Player> plr);
        void sendMasterMessage(const QString& packet, QSharedPointer<Player> plr, const bool toAll = false);
        void sendMasterMessageToAdmins(const QString& message);

        void startMasterSerNumKeepAliveTimer();
        void masterSerNumKeepAliveSlot();
        void sendPingToPlayer(QSharedPointer<Player> plr);

        qint64 getUpTime() const;
        QTimer* getUpTimer();

        QString getGameWorld() const;
        void setGameWorld(const QString& value);

        Games getGameId() const;
        void setGameId(const Games& game);

        QString getGameName() const;
        void setGameName(const QString& value);

        QHostInfo getHostInfo() const;

        int getVersionID() const;
        void setVersionID(const int& value);

        quint16 getMasterPort() const;
        void setMasterPort(const quint16& value);

        QString getMasterIP() const;
        void setMasterIP(const QString& value, const quint16& port);

        bool getMasterInfoRecv() const;
        void setMasterInfoRecv(bool infoRecv);

        bool getIsPublic() const;
        void setIsPublic(const bool& value, const QString& netInterface = "");

        bool getUseUPNP() const;
        void setUseUPNP(const bool& value);

        QString getServerID() const;
        void setServerID(const QString& value);

        qint32 getPlayerCount() const;
        void setPlayerCount(const qint32& value);

        qint32 getPeakPlayerCount() const;
        void setPeakPlayerCount(const qint32& value);

        qint32 getQuarantinedPlayerCount() const;
        QString getQuarantinedPlayerList() const;

        qint32 getMutedPlayerCount() const;
        QString getMutedPlayerList() const;

        quint16 getPublicPort() const;
        void setPublicPort(const quint16& value);

        QString getPublicIP() const;
        void setPublicIP(const QString& value);

        quint16 getPrivatePort() const;
        void setPrivatePort(const quint16& value);

        QString getPrivateIP() const;
        void setPrivateIP(const QString& value);

        QString getServerName() const;
        void setServerName(const QString& value);

        bool getIsSetUp() const;
        void setIsSetUp(const bool& value);

        quint32 getUserCalls() const;
        void setUserCalls(const quint32& value);

        quint32 getUserPings() const;
        void setUserPings(const quint32& value);

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

        void updateBytesOut(QSharedPointer<Player> plr, const qint64 bOut);

        bool getUpnpPortAdded() const;
        void setUpnpPortAdded(bool value);

        qint64 getMaxAFKTime();

        qint64 getInitializeDate() const;
        void setInitializeDate(const qint64& value);

        bool getUpnpTimedOut() const;
        void setUpnpTimedOut(bool newUpnpTimedOut);

        qint32 getMaxPlayerCount() const;
        void setMaxPlayerCount(const qint32& value);

    public:
        const inline QVector<QSharedPointer<Player>>& getPlayerVector() const{ return players; }

    signals:
        void plrConnectedSignal(qintptr socketDescriptor);
        void bindSocketSignal(const QHostAddress& addr, const quint16& port);
        void sendUdpDataSignal(const QHostAddress& addr, const quint16& port, const QString& data);
        void initializeServerSignal();
        void closeUdpSocketSignal();
        void serverIsSetupSignal();

        void serverUsageChangedSignal(const qint32& minute, const qint32& day, const qint32& hour);
        void serverWorldChangedSignal(const QString& newWorld);
        void serverNameChangedSignal(const QString& newName);
        void serverGameChangedSignal(const Games& game);
        void serverIDChangedSignal(const QString& serverID);
        void refreshAFKTimersSignal(const qint64& maxAFK);

        void insertLogSignal(const QString& source, const QString& message, const LKeys& type, const bool& logToFile, const bool& newLine) const;
        void sendMasterMsgToPlayerSignal(QSharedPointer<Player> plr, const bool& all, const QByteArray& packet);

        void upnpPortForwardSignal(const QString& privateIP, const quint16& port, const bool& insert);
        void connectionTimeUpdateSignal();
        void recvMasterInfoSignal();

    public slots:
        void dataOutSizeSlot(const quint64& size);
        void refreshAFKTimersSlot();
        void masterMixIPChangedSlot();
        void masterMixInfoSlot(const Games& game, const QString& ip, const quint16& port);
        void setBytesInSignal(const quint64& bytes);
        void recvMasterInfoResponseSlot(const QString& masterIP, const quint16& masterPort, const QString& userIP, const quint16& userPort);
        void recvPlayerGameInfoSlot(const QString& info, const QString& ip);
        void gameInfoChangedSlot(const QString& info);

        void ipDCIncreaseSlot(const DCTypes& type);
        void setVisibleStateSlot( const bool& state);
        void recvMasterInfoSlot();
        void setMaxPlayersSlot(const qint32& maxPlayers);

    private slots:
        void masterMixInfoSyncSlot();
        void sendUserListSlot(const QHostAddress& addr, const quint16& port, const UserListResponse& type);
        void increaseServerPingSlot();
        void upnpPortAddedSlot(const quint16& port, const QString& protocol);
        void updateUsageTimeOutSlot();
        void masterCheckInTimeOutSlot();
        void upnpTimeOutSlot();
        void upTimerTimeOutSlot();
        void upnpPortRefreshTimeOutSlot();
        void masterTimeOutSlot();
};

#endif // SERVER_HPP
