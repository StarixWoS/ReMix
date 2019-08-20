
//Class includes.
#include "worldshuffler.hpp"

//ReMix Includes.
#include "serverinfo.hpp" //Used to get the MasterIP and Socket.

//Qt Includes.
#include <QStringList>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QByteArray>
#include <QString>
#include <QtCore>

WorldShuffler* WorldShuffler::instance{ nullptr };

WorldShuffler::WorldShuffler(QObject *parent) : QObject(parent)
{
    //Update the server list on creation.
}

WorldShuffler* WorldShuffler::getInstance()
{
    if ( instance == nullptr )
        instance = new WorldShuffler();

    return instance;
}

qint32 WorldShuffler::getLastUpdated()
{
    //Note: The server list will only be refreshed every 60 minutes.
    return lastUpdated;
}

QStringList WorldShuffler::getActiveWorlds()
{
    //Note: Returns a list of servers that are currently active on the
    //serverlist including those hosted by our User.
    return activeWorlds;
}

void WorldShuffler::worldFinder()
{
//    QUdpSocket* udpSocket{ nullptr };

//    QUdpSocket* masterUDPSocket{ nullptr };
//    QByteArray udpData;

//    QString masterPort_str{ "" };
//    QString masterIP_str{ "" };

//    QUdpSocket* serverSocket{ nullptr };

    //Note: Contacts the MasterMix and requests the current server list.
    //Note2: For servers being hosted by our User, we will proactively
    //obtain the information locally based on the publicIP address..
}
