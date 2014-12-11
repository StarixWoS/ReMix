
#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include <QUdpSocket>
#include <QHostInfo>
#include <QString>
#include <QDir>

#include "helper.hpp"
#include "player.hpp"

const int MAX_PLAYERS = 256;
class ServerInfo
{
    QUdpSocket* masterSocket{ nullptr };

    QTimer upTimer;
    quint64 upTime{ 0 };

    bool isSetUp{ false };
    QString name{ "AHitB ReMix Server" };

    QString privateIP{ "" };
    int privatePort{ 8888 };

    QString publicIP{ "" };
    quint16 publicPort{ 8888 };

    int playerCount{ 0 };
    int serverID{ 0 };

    bool isMaster{ false };
    bool isPublic{ false };

    QString masterIP{ "" };
    quint16 masterPort{ 23999 };

    float versionID_f{ 1.10f };
    int versionID_i{ 41252 };

    QHostInfo hostInfo;

    QString gameName{ "WoS" };
    int gameId{ 0 };

    QString gameInfo{ "" };
    QString info{ "" };

    Player* players[ MAX_PLAYERS ];

    quint32 userCalls{ 0 };
    quint32 serNumDc{ 0 };
    quint32 dupDc{ 0 };
    quint32 pktDc{ 0 };
    quint32 ipDc{ 0 };

    QElapsedTimer baudTime;
    quint64 bytesIn{ 0 };
    quint64 baudIn{ 0 };

    quint64 bytesOut{ 0 };
    quint64 baudOut{ 0 };

    bool logUsage{ false };

    public:
        ServerInfo();
        ~ServerInfo();

        void sendUDPData(QHostAddress& addr, quint16 port, QString& data);

        void sendServerInfo(QHostAddress& addr, quint16 port);
        void sendUserList(QHostAddress& addr, quint16 port);
        void sendMasterInfo(bool disconnect = false);

        Player* createPlayer(int slot);
        Player* getPlayer(int slot);
        void deletePlayer(int slot);

        int getEmptySlot();
        int getSocketSlot(QTcpSocket* soc);
        int getSernumSlot(qint32 sernum);
        int getQItemSlot(QStandardItem* index);
        int getIPAddrSlot(QString ip);

        quint64 sendMasterMessage(QString packet, Player* plr = nullptr, bool toAll = false);
        quint64 sendToAllConnected(QString packet);

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
        void setHostInfo(const QHostInfo& value);

        int getVersionID_i() const;
        void setVersionID_i(int value);

        float getVersionID_f() const;
        void setVersionID_f(float value);

        quint16 getMasterPort() const;
        void setMasterPort(int value);

        QString getMasterIP() const;
        void setMasterIP(const QString& value);

        bool getIsPublic() const;
        void setIsPublic(bool value);

        bool getIsMaster() const;
        void setIsMaster(bool value);

        int getServerID() const;
        void setServerID(int value);

        int getPlayerCount() const;
        void setPlayerCount(int value);

        quint16 getPublicPort() const;
        void setPublicPort(quint16 value);

        QString getPublicIP() const;
        void setPublicIP(const QString& value);

        int getPrivatePort() const;
        void setPrivatePort(int value);

        QString getPrivateIP() const;
        void setPrivateIP(const QString& value);

        QString getServerRules() const;
        void setServerRules(const QString& value);

        QString getName() const;
        void setName(const QString& value);

        bool getIsSetUp() const;
        void setIsSetUp(bool value);

        quint32 getUserCalls() const;
        void setUserCalls(const quint32& value);

        quint32 getSerNumDc() const;
        void setSerNumDc(const quint32& value);

        quint32 getDupDc() const;
        void setDupDc(const quint32& value);

        quint32 getPktDc() const;
        void setPktDc(const quint32& value);

        quint32 getIpDc() const;
        void setIpDc(const quint32& value);

        quint64 getBytesIn() const;
        void setBytesIn(const quint64& value);

        quint64 getBaudIn() const;
        void setBaudIn(const quint64& bIn);

        quint64 getBytesOut() const;
        void setBytesOut(const quint64& value);

        quint64 getBaudOut() const;
        void setBaudOut(const quint64& bOut);

        bool getLogUsage() const;
        void setLogUsage(bool value);

        QUdpSocket* getMasterSocket() const;
        bool initMasterSocket(QHostAddress& addr, quint16 port);
};

#endif // SERVERINFO_HPP

