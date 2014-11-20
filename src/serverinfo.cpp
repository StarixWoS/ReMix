
#include "serverinfo.hpp"

//ServerInfo::ServerInfo( )
//{

//}

//ServerInfo::~ServerInfo()
//{
//    for ( int x = 0; x < MAX_PLAYERS; ++x );
//    {
//        if ( players[ x ] != nullptr )
//            this->deletePlayer( x );
//    }
//}

Player* ServerInfo::createPlayer(int slot)
{
    if ( slot >= 0 && slot < MAX_PLAYERS  )
    {
        players[ slot ] = new Player();
        players[ slot ]->setSlotPos( slot );    //The player will be Slot-Aware.
        return players[ slot ];
    }
    return nullptr;
}

Player* ServerInfo::getPlayer(int slot)
{
    return players[ slot ];
}

void ServerInfo::deletePlayer(int slot)
{
    QTcpSocket* soc = players[ slot ]->getSocket();
                soc->disconnect();
                soc->deleteLater();

    players[ slot ]->setSocket( nullptr );
    delete players[ slot ];

    players[ slot ] = nullptr;
}

int ServerInfo::getEmptySlot()
{
    int slot = -1;
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        slot = i;
        if ( players[ i ] == nullptr )
            break;
    }
    return slot;
}

int ServerInfo::getSocketSlot(QTcpSocket* soc)
{
    int slot = -1;
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        if ( players[ i ] != nullptr
          && players[ i ]->getSocket() == soc )
        {
            slot = i;
        }
    }
    return slot;
}

int ServerInfo::getSernumSlot(quint32 sernum)
{
    int slot = -1;
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        if ( players[ i ] != nullptr
          && players[ i ]->getSernum() == sernum )
        {
            slot = i;
        }
    }
    return slot;
}

QString ServerInfo::getInfo() const
{
    return info;
}

void ServerInfo::setInfo(const QString& value)
{
    info = value;
}

QString ServerInfo::getGameInfo() const
{
    return gameInfo;
}

void ServerInfo::setGameInfo(const QString& value)
{
    gameInfo = value;
}

int ServerInfo::getGameId() const
{
    return gameId;
}

void ServerInfo::setGameId(int value)
{
    gameId = value;
}

QString ServerInfo::getGameName() const
{
    return gameName;
}

void ServerInfo::setGameName(const QString& value)
{
    gameName = value;
}

QHostInfo ServerInfo::getHostInfo() const
{
    return hostInfo;
}

void ServerInfo::setHostInfo(const QHostInfo& value)
{
    hostInfo = value;
}

int ServerInfo::getVersionID_i() const
{
    return versionID_i;
}

void ServerInfo::setVersionID_i(int value)
{
    versionID_i = value;
}

float ServerInfo::getVersionID_f() const
{
    return versionID_f;
}

void ServerInfo::setVersionID_f(float value)
{
    versionID_f = value;
}

int ServerInfo::getMasterPort() const
{
    return masterPort;
}

void ServerInfo::setMasterPort(int value)
{
    masterPort = value;
}

QString ServerInfo::getMasterIP() const
{
    return masterIP;
}

void ServerInfo::setMasterIP(const QString& value)
{
    masterIP = value;
}

bool ServerInfo::getIsPublic() const
{
    return isPublic;
}

void ServerInfo::setIsPublic(bool value)
{
    isPublic = value;
}

bool ServerInfo::getIsMaster() const
{
    return isMaster;
}

void ServerInfo::setIsMaster(bool value)
{
    isMaster = value;
}

int ServerInfo::getServerID() const
{
    return serverID;
}

void ServerInfo::setServerID(int value)
{
    serverID = value;
}

int ServerInfo::getPlayerCount() const
{
    return playerCount;
}

void ServerInfo::setPlayerCount(int value)
{
    playerCount = value;
}

int ServerInfo::getPublicPort() const
{
    return publicPort;
}

void ServerInfo::setPublicPort(int value)
{
    publicPort = value;
}

QString ServerInfo::getPublicIP() const
{
    return publicIP;
}

void ServerInfo::setPublicIP(const QString& value)
{
    publicIP = value;
}

int ServerInfo::getPrivatePort() const
{
    return privatePort;
}

void ServerInfo::setPrivatePort(int value)
{
    privatePort = value;
}

QString ServerInfo::getPrivateIP() const
{
    return privateIP;
}

void ServerInfo::setPrivateIP(const QString& value)
{
    privateIP = value;
}

QString ServerInfo::getServerRules() const
{
    return serverRules;
}

void ServerInfo::setServerRules(const QString& value)
{
    serverRules = value;
}

QString ServerInfo::getName() const
{
    return name;
}

void ServerInfo::setName(const QString& value)
{
    name = value;
}

bool ServerInfo::getIsSetUp() const
{
    return isSetUp;
}

void ServerInfo::setIsSetUp(bool value)
{
    isSetUp = value;
}
