
#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include <QHostInfo>
#include <QString>

struct ServerInfo
{
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

    float versionID_f{ 4.1252E4f };
    int versionID_i{ 41252 };

    QHostInfo hostInfo;

    QString gameName{ "WoS" };
    int gameId{ 0 };

    QString gameInfo{ "" };
    QString info{ "" };
};

#endif // SERVERINFO_HPP

