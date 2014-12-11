
#include "serverinfo.hpp"

ServerInfo::ServerInfo()
{
    masterSocket = new QUdpSocket();

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        players[ i ] = nullptr;
    }

    baudTime.start();
    upTimer.start( 1000 );

    QObject::connect( &upTimer, &QTimer::timeout, [=]()
    {
        ++upTime;
        if ( baudTime.elapsed() > 5000 )
        {
            this->setBaudIn( this->getBytesIn() );
            this->setBytesIn( 0 );

            this->setBaudOut( this->getBytesOut() );
            this->setBytesOut( 0 );

            baudTime.restart();
        }
    });
}

ServerInfo::~ServerInfo()
{
    masterSocket->close();
    masterSocket->deleteLater();

    upTimer.disconnect();
}

QUdpSocket* ServerInfo::getMasterSocket() const
{
    return masterSocket;
}

bool ServerInfo::initMasterSocket(QHostAddress& addr, quint16 port)
{
    return masterSocket->bind( addr, port );
}

void ServerInfo::sendUDPData(QHostAddress& addr, quint16 port, QString& data)
{
    if ( masterSocket != nullptr )
        masterSocket->writeDatagram( data.toLatin1(), data.size() + 1, addr, port );
}

void ServerInfo::sendServerInfo(QHostAddress& addr, quint16 port)
{
    if ( addr.isNull() )
        return;

    QString response{ "#name=%1%2 //Rules: %3 //ID:%4 //TM:%5 //US:%6" };
            response = response.arg( this->getName() );

    if ( !this->getGameInfo().isEmpty() )
        response = response.arg( " [" % this->getGameInfo() % "]" );
    else
        response = response.arg( "" );

    response = response.arg( this->getServerRules() )
                       .arg( Helper::intToStr( this->getServerID(), 16, 8 ) )
                       .arg( Helper::intToStr( QDateTime::currentDateTime().toTime_t(), 16, 8 ) )
                       .arg( "999.999.999" );

    if ( !response.isEmpty() )
        this->sendUDPData( addr, port, response );
}

void ServerInfo::sendUserList(QHostAddress& addr, quint16 port)
{
    if ( addr.isNull() )
        return;

    Player* plr{ nullptr };
    QString response{ "Q" };

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        plr = this->getPlayer( i );
        if ( plr != nullptr
          && plr->getSernum() != 0 )
        {
            response += Helper::intToStr( plr->getSernum(), 16 ) % ",";
        }
    }

    if ( !response.isEmpty() )
        this->sendUDPData( addr, port, response );
}

void ServerInfo::sendMasterInfo(bool disconnect)
{
    QHostAddress addr{ this->getMasterIP() };
    quint16 port{ this->getMasterPort() };
    QString response{ "X" };

    if ( !disconnect )
    {
        if ( this->getIsSetUp() )
        {
            response = { "!version=%1,nump=%2,gameid=%3,game=%4,host=%5,id=%6,port=%7,info=%8,name=%9" };
            response = response.arg( this->getVersionID_i() )
                               .arg( this->getPlayerCount() )
                               .arg( this->getGameId() )
                               .arg( this->getGameName() )
                               .arg( this->getHostInfo().localHostName() )
                               .arg( this->getServerID() )
                               .arg( this->getPrivatePort() )
                               .arg( this->getGameInfo() )
                               .arg( this->getName() );
        }
    }

    if ( !response.isEmpty()
      && this->getIsSetUp() )
    {
        this->sendUDPData( addr, port, response );
    }
}

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
    Player* plr = this->getPlayer( slot );
    if ( plr != nullptr )
    {
        if ( this->getLogUsage() )
        {
            QString log{ QDate::currentDate().toString( "mixUsage/yyyy-MM-dd.txt" ) };
            QString logMsg{ "Client: [ %1 ] was on for %2 minutes and sent %3 bytes in %4 packets, averaging %5 baud ( %6 )" };
            if ( plr != nullptr )
            {
                logMsg = logMsg.arg( plr->getPublicIP() )
                               .arg( plr->getConnTime() / 60 )
                               .arg( plr->getBytesIn() )
                               .arg( plr->getPacketsIn() )
                               .arg( plr->getAvgBaudIn() )
                               .arg( QString( plr->getBioData() ));
                Helper::logToFile( log, logMsg, true, true );
            }
        }

        QTcpSocket* soc = plr->getSocket();
        if ( soc != nullptr )
        {
            soc->disconnect();
            soc->deleteLater();
        }
        plr->setSocket( nullptr );
    }
    plr = nullptr;

    delete players[ slot ];
    players[ slot ] = nullptr;
}

int ServerInfo::getEmptySlot()
{
    int slot{ -1 };
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
    int slot{ -1 };
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        if ( players[ i ] != nullptr
          && players[ i ]->getSocket() == soc )
        {
            slot = i;
            break;
        }
    }
    return slot;
}

int ServerInfo::getSernumSlot(qint32 sernum)
{
    int slot{ -1 };
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        if ( players[ i ] != nullptr
          && players[ i ]->getSernum() == sernum )
        {
            slot = i;
            break;
        }
    }
    return slot;
}

int ServerInfo::getQItemSlot(QStandardItem* index)
{
    int slot{ -1 };
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        if ( players[ i ] != nullptr
          && players[ i ]->getTableRow() == index )
        {
            slot = i;
            break;
        }
    }
    return slot;
}

int ServerInfo::getIPAddrSlot(QString ip)
{
    int slot{ -1 };
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        if ( players[ i ] != nullptr
          && players[ i ]->getPublicIP() == ip )
        {
            slot = i;
            break;
        }
    }
    return slot;
}

quint64 ServerInfo::sendMasterMessage(QString packet, Player* plr, bool toAll)
{
    QString msg = QString( ":SR@M%1\r\n" )
                      .arg( packet );
    quint64 bOut{ 0 };
    if ( toAll )
    {
        bOut = this->sendToAllConnected( msg );
    }
    else if ( plr == nullptr
           && toAll )
    {
        bOut = this->sendToAllConnected( msg );
    }
    else if (( plr != nullptr
            && plr->getSocket() != nullptr )
           && !toAll )
    {
        bOut = plr->getSocket()->write( msg.toLatin1(), msg.length() );
    }
    return bOut;
}

quint64 ServerInfo::sendToAllConnected(QString packet)
{
    Player* tmpPlr{ nullptr };
    quint64 bOut{ 0 };

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = this->getPlayer( i );
        if ( tmpPlr != nullptr
          && tmpPlr->getSocket() != nullptr )
        {
            bOut = tmpPlr->getSocket()->write( packet.toLatin1(), packet.length() );
        }
    }
    return bOut;
}

quint64 ServerInfo::getUpTime() const
{
    return upTime;
}

QTimer* ServerInfo::getUpTimer()
{
    return &upTimer;
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

quint16 ServerInfo::getMasterPort() const
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

quint16 ServerInfo::getPublicPort() const
{
    return publicPort;
}

void ServerInfo::setPublicPort(quint16 value)
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
    return Helper::getServerRules();
}

void ServerInfo::setServerRules(const QString& value)
{
    QVariant val = QVariant( value );
    Helper::setServerRules( val );
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

quint32 ServerInfo::getUserCalls() const
{
    return userCalls;
}

void ServerInfo::setUserCalls(const quint32& value)
{
    userCalls = value;
}

quint32 ServerInfo::getSerNumDc() const
{
    return serNumDc;
}

void ServerInfo::setSerNumDc(const quint32& value)
{
    serNumDc = value;
}

quint32 ServerInfo::getDupDc() const
{
    return dupDc;
}

void ServerInfo::setDupDc(const quint32& value)
{
    dupDc = value;
}

quint32 ServerInfo::getPktDc() const
{
    return pktDc;
}

void ServerInfo::setPktDc(const quint32& value)
{
    pktDc = value;
}

quint32 ServerInfo::getIpDc() const
{
    return ipDc;
}

void ServerInfo::setIpDc(const quint32& value)
{
    ipDc = value;
}

quint64 ServerInfo::getBytesIn() const
{
    return bytesIn;
}

void ServerInfo::setBytesIn(const quint64& value)
{
    bytesIn = value;
}

quint64 ServerInfo::getBaudIn() const
{
    return baudIn;
}

void ServerInfo::setBaudIn(const quint64& bIn)
{
    quint64 time = baudTime.elapsed();
    quint64 baud{ 0 };

    if ( bIn > 0 && time > 0 )
        baud = 10000 * bIn / time;

    baudIn = baud;
}

quint64 ServerInfo::getBytesOut() const
{
    return bytesOut;
}

void ServerInfo::setBytesOut(const quint64& value)
{
    bytesOut = value;
}

quint64 ServerInfo::getBaudOut() const
{
    return baudOut;
}

void ServerInfo::setBaudOut(const quint64& bOut)
{
    quint64 time = baudTime.elapsed();
    quint64 baud{ 0 };

    if ( bOut > 0 && time > 0 )
        baud = 10000 * bOut / time;

    baudOut = baud;
}

bool ServerInfo::getLogUsage() const
{
    return logUsage;
}

void ServerInfo::setLogUsage(bool value)
{
    logUsage = value;
}
