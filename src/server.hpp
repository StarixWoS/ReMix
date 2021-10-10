
#ifndef SERVER_HPP
#define SERVER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
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

    UdpThread* udpThread{ nullptr };

    qint64 initializeDate{ 0 };

    QTimer upTimer;

    bool isSetUp{ false };
    QString serverName{ "AHitB ReMix Server" };

    QString privateIP{ "" };
    quint16 privatePort{ 8888 };

    QString publicIP{ "" };
    quint16 publicPort{ 8888 };

    quint32 usageArray[ static_cast<int>( Globals::SERVER_USAGE_48_HOURS ) ]{ 0 };
    quint32 usageCounter{ 0 };

    QTimer usageUpdate;
    qint32 usageHours{ 0 };
    qint32 usageDays{ 0 };
    qint32 usageMins{ 0 };

    quint32 playerCount{ 0 };
    QString serverID{ "" };

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

    QVector<Player*> players;
    QMap<qint32, Player*> plrSlotMap;

    quint32 userCalls{ 0 };
    quint32 userPings{ 0 };
    quint32 dupDc{ 0 };
    quint32 pktDc{ 0 };
    quint32 ipDc{ 0 };

    quint64 bytesOut{ 0 };
    quint64 bytesIn{ 0 };

    QTimer upnpPortRefresh;
    bool upnpPortAdded{ false };

    public:
        Server(QWidget* parent = nullptr);
        ~Server() override;
        void incomingConnection(qintptr socketDescriptor) override;

        void setupInfo();
        void setupUPNP(const bool& enable = false);

        void sendUserList(const QHostAddress& addr, const quint16& port, const UserListResponse& type);
        void sendMasterInfo(const bool& disconnect = false);

        Player* createPlayer(qintptr socketDescriptor);
        Player* getPlayer(const int& slot);
        Player* getPlayer(const qintptr& socketDescriptor);
        void deletePlayer(Player* plr, const bool& timedOut = false);

        Player* getLastPlayerInStorage(Player* plr);
        int getEmptySlot();
        int getSocketSlot(qintptr socketDescriptor);

        void sendPlayerSocketInfo();
        void sendPlayerSocketPosition(Player* plr, const bool& forceIssue);
        void sendServerRules(Player* plr);
        void sendServerGreeting(Player* plr);
        void sendMasterMessage(const QString& packet, Player* plr = nullptr, const bool toAll = false);
        void sendMasterMessageToAdmins(const QString& message, Player* srcPlayer = nullptr);

        qint64 getUpTime() const;
        QTimer* getUpTimer();

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
        void setMasterIP(const QString& value, const quint16& port);

        bool getIsPublic() const;
        void setIsPublic(const bool& value);

        bool getUseUPNP() const;
        void setUseUPNP(const bool& value);

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

        PacketHandler* getPktHandle();

        void updateBytesOut(Player* plr, const qint64 bOut);

        bool getUpnpPortAdded() const;
        void setUpnpPortAdded(bool value);

        qint64 getMaxIdleTime();

        qint64 getInitializeDate() const;
        void setInitializeDate(const qint64& value);

    private:
        const inline QVector<Player*> getPlayerVector(){ return players; }

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
        void serverIDChangedSignal(const QString& serverID);
        void setMaxIdleTimeSignal(const qint64& maxAFK);

        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine) const;
        void sendMasterMsgToPlayerSignal(Player* plr, const bool& all, const QByteArray& packet);

        void upnpPortForwardSignal(const quint16& port, const bool& insert);
        void connectionTimeUpdateSignal();

    public slots:
        void dataOutSizeSlot(const quint64& size);
        void setMaxIdleTimeSlot();
        void masterMixIPChangedSlot();
        void setBytesInSignal(const quint64& bytes);
        void recvMasterInfoResponseSlot(const QString& masterIP, const quint16& masterPort, const QString& userIP, const quint16& userPort);
        void recvPlayerGameInfoSlot(const QString& info, const QString& ip);

    private slots:
        void sendUserListSlot(const QHostAddress& addr, const quint16& port, const UserListResponse& type);
        void increaseServerPingSlot();
        void upnpPortAddedSlot(const quint16& port, const QString& protocol);
};

#endif // SERVER_HPP
