
#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include <QHostInfo>
#include <QString>

#include "player.hpp"

const int MAX_PLAYERS = 256;
struct ServerInfo
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

    int getEmptySlot()
    {
        int slot = 0;
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            if ( players[ i ]->getSocket() == nullptr )
                slot = i;
        }
        return slot;
    }

    int getSocketSlot(QTcpSocket* soc)
    {
        int slot = -1;
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            if ( players[ i ]->getSocket() == soc )
                slot = i;
        }
        return slot;
    }

    int getSernumSlot(unsigned int sernum)
    {
        int slot = -1;
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            if ( players[ i ]->getSernum() == sernum )
                slot = i;
        }
        return slot;
    }
};

#endif // SERVERINFO_HPP

