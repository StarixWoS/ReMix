
#ifndef SERVERINFO
#define SERVERINFO

#include <QHostInfo>
#include <QString>

struct ServerInfo
{
    QString serverName{ "AHitB ReMix Server" };
    QString serverRules{ "" };
    int serverPort{ 8888 };

    int playerCount{ 0 };
    int serverID{ 0 };

    bool isMaster{ false };
    bool isPublic{ false };

    QString masterIP{ "" };
    int masterPort{ 0 };

    int versionID_i{ 41252 };
    float versionID_f{ 4.1252E4f };
    int gameId{ 0 };

    QHostInfo hostInfo;

    QString gameName{ "WoS" };
    QString info{ "" };
    QString world{ "" };
};

#endif // SERVERINFO

