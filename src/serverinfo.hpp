
#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include <QHostInfo>
#include <QString>

#include "player.hpp"

const int MAX_PLAYERS = 256;
class ServerInfo
{
    bool isSetUp{ false };
    QString name{ "AHitB ReMix Server" };
    QString serverRules{ "" };

    QString privateIP{ "" };
    int privatePort{ 8888 };

    QString publicIP{ "" };
    int publicPort{ 8888 };

    int playerCount{ 0 };
    int serverID{ 0 };

    bool isMaster{ false };
    bool isPublic{ false };

    QString masterIP{ "" };
    int masterPort{ 23999 };

    float versionID_f{ 1.10f };
    int versionID_i{ 41252 };

    QHostInfo hostInfo;

    QString gameName{ "WoS" };
    int gameId{ 0 };

    QString gameInfo{ "" };
    QString info{ "" };

    Player* players[ MAX_PLAYERS ];

    public:
//        explicit ServerInfo();
//        ~ServerInfo();

        bool createPlayer(int slot);
        bool deletePlayer(int slot);
        Player* getPlayer(int slot);

        int getEmptySlot();
        int getSocketSlot(QTcpSocket* soc);
        int getSernumSlot(unsigned int sernum);

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

        int getMasterPort() const;
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

        int getPublicPort() const;
        void setPublicPort(int value);

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
};

#endif // SERVERINFO_HPP

