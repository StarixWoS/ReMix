
#include "includes.hpp"
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
            this->setBaudIO( this->getBytesIn(), baudIn );
            this->setBytesIn( 0 );

            this->setBaudIO( this->getBytesOut(), baudOut );
            this->setBytesOut( 0 );

            baudTime.restart();
        }
    });

    masterTimeOut.setInterval( MAX_MASTER_RESPONSE_TIME );
    masterTimeOut.setSingleShot( true );
    QObject::connect( &masterTimeOut, &QTimer::timeout, [=]()
    {
        this->setMasterTimedOut( true );
    });

    //Every 2 Seconds we will attempt to Obtain Master Info.
    //This will be set to 300000 (5-Minutes) once Master info is obtained.
    masterCheckIn.setInterval( 2000 );
    QObject::connect( &masterCheckIn, &QTimer::timeout, [=]()
    {
        this->sendMasterInfo();

        if ( !masterTimeOut.isActive() )
            masterTimeOut.start();
    });

    //Updates the Server's Server Usage array every 10 minutes.
    usageUpdate.start( SERVER_USAGE_UPDATE );
    QObject::connect( &usageUpdate, &QTimer::timeout, [=]()
    {
        usageArray[ usageCounter ] = this->getPlayerCount();

        usageDays = 0;
        usageHours = 0;
        usageMins = 0;

        quint32 usageCap{ 0 };
        quint32 code{ 0 };
        for ( int i = 0; i < SERVER_USAGE_48_HOURS; ++i )
        {
            code = usageArray[ ( i + usageCounter ) % SERVER_USAGE_48_HOURS ];
            usageCap = ( SERVER_USAGE_48_HOURS - 1 ) - i;
            if ( usageCap < 156 )
            {
                usageDays += code;
                if ( usageCap < 7 )
                {
                    usageHours += code;
                    if ( usageCap < 3 )
                        usageMins += code;
                }
            }
        }
        ++usageCounter;
        usageCounter %= SERVER_USAGE_48_HOURS;
    });
}

ServerInfo::~ServerInfo()
{
    masterSocket->close();
    masterSocket->deleteLater();

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        this->deletePlayer( i );
    }
    upTimer.disconnect();
}

QString ServerInfo::getMasterInfoHost() const
{
    return masterInfoHost;
}

void ServerInfo::setMasterInfoHost(const QString& value)
{
    //Try to validate a URL.
    QRegExp isUrl( "http[s]?|ftp", Qt::CaseInsensitive );

    QString url{ value };
    QString scheme = QUrl( url ).scheme();

    QFile synRealIni( "synreal.ini" );
    if ( masterInfoHost != value
      && isUrl.exactMatch( scheme ) )
    {
        synRealIni.remove();
    }
    else
        url = "http://synthetic-reality.com/synreal.ini";

    masterInfoHost = url;
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
        masterSocket->writeDatagram( data.toLatin1(), data.size() + 1,
                                     addr, port );
}

void ServerInfo::sendServerInfo(QHostAddress& addr, quint16 port)
{
    if ( addr.isNull() )
        return;

    QString response{ "#name=%1%2 //Rules: %3 //ID:%4 //TM:%5 //US:%6 "
                      "//ReMix" };

    response = response.arg( this->getName() );
    if ( !this->getGameInfo().isEmpty() )
        response = response.arg( " [" % this->getGameInfo() % "]" );
    else
        response = response.arg( "" );

    QString usage{ "%1.%2.%3" };
            usage = usage.arg( this->getUsageMins() )
                         .arg( this->getUsageHours() )
                         .arg( this->getUsageDays() );

    response = response.arg( Rules::getRuleSet() )
                       .arg( Helper::intToStr( this->getServerID(), 16, 8 ) )
                       .arg( Helper::intToStr( QDateTime::currentDateTime()
                                                    .toTime_t(), 16, 8 ) )
                       .arg( usage );

    if ( !response.isEmpty() )
        this->sendUDPData( addr, port, response );
}

void ServerInfo::sendUserList(QHostAddress& addr, quint16 port, quint32 type)
{
    if ( addr.isNull() )
        return;

    Player* plr{ nullptr };

    QString response{ "Q" };
    if ( type == 1 )
        response = "R";

    QString filler_Q{ "%1," };
    QString filler_R{ "%1|%2," };

    //Note: The 'R' Response sent by the Syn-Real Mix Server
    //Sends the User's IP address within field(%2) 2; we are sending the User's
    //position within the players[] array instead to prevent IP leakage.

    for ( int i = 0; i < MAX_PLAYERS && response.length() < 800; ++i )
    {
        plr = this->getPlayer( i );
        if ( plr != nullptr
          && plr->getSernum_i() != 0 )
        {
            if ( type == Q_Response ) //Standard 'Q' Response.
            {
                response += filler_Q.arg( Helper::intToStr(
                                              plr->getSernum_i(), 16 ) );
            }
            else if ( type == R_Response ) //Non-Standard 'R' Response
            {
                response += filler_R.arg( Helper::intToStr(
                                              plr->getSernum_i(), 16 ) )
                                    .arg( Helper::intToStr(
                                              plr->getSlotPos(), 16, 8 ) );
            }
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
            response = { "!version=%1,nump=%2,gameid=%3,game=%4,host=%5,id=%6,"
                         "port=%7,info=%8,name=%9" };
            response = response.arg( this->getVersionID() )
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
        this->setSentUDPCheckIn( true );
    }
    else
        this->setSentUDPCheckIn( false );
}

Player* ServerInfo::createPlayer(int slot)
{
    if ( slot >= 0 && slot < MAX_PLAYERS  )
    {
        players[ slot ] = new Player();
        players[ slot ]->setSlotPos( slot );
        this->setPlayerCount( this->getPlayerCount() + 1 );
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
        if ( this->getLogFiles() )
        {
            QString log{ QDate::currentDate()
                          .toString( "logs/UsageLog.txt" ) };
            QString logMsg{ "Client: [ %1 ] was on for %2 minutes and sent %3 "
                            "bytes in %4 packets, averaging %5 baud [ %6 ]" };
            if ( plr != nullptr )
            {
                logMsg = logMsg.arg( plr->getPublicIP() )
                               .arg( plr->getConnTime() / 60 )
                               .arg( plr->getBytesIn() )
                               .arg( plr->getPacketsIn() )
                               .arg( plr->getAvgBaudIn() )
                               .arg( plr->getBioData() );
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

    this->setPlayerCount( this->getPlayerCount() - 1 );
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

void ServerInfo::sendServerRules(Player* plr)
{
    QTcpSocket* soc{ nullptr };
    if ( plr == nullptr )
        return;

    soc = plr->getSocket();
    if ( soc == nullptr )
        return;

    quint64 bOut{ 0 };
    QString rules{ ":SR$%1\r\n" };
            rules = rules.arg( Rules::getRuleSet() );

    bOut = soc->write( rules.toLatin1(), rules.length() );
    if ( bOut >= 1 )
    {
        plr->setPacketsOut( plr->getPacketsOut() + 1 );
        plr->setBytesOut( plr->getBytesOut() + bOut );

        this->setBytesOut( this->getBytesOut() + bOut );
    }
}

void ServerInfo::sendServerGreeting(Player* plr)
{
    QString greeting = Settings::getMOTDMessage();
    if ( Settings::getRequirePassword() )
    {
        greeting.append( " Password required: Please reply with (/login *PASS)"
                         " or be disconnected." );
        plr->setPwdRequested( true );
    }

    if ( !greeting.isEmpty() )
        this->sendMasterMessage( greeting, plr, false );

    if ( !Rules::getRuleSet().isEmpty() )
        this->sendServerRules( plr );
}

void ServerInfo::sendMasterMessage(QString packet, Player* plr, bool toAll)
{
    QString msg = QString( ":SR@M%1\r\n" )
                      .arg( packet );
    QTcpSocket* soc{nullptr };
    if ( plr != nullptr )
        soc = plr->getSocket();

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
            && soc != nullptr )
           && !toAll )
    {
        //Iterate over all Player objects.
        //And check if our Player object exists.
        //This is to prevent a Crash related to sending messages
        //to a disconnected User.

        Player* tmpPlr{ nullptr };
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            tmpPlr = this->getPlayer( i );
            if ( plr == tmpPlr )
            {
                bOut = soc->write( msg.toLatin1(),
                                   msg.length() );
                plr->setPacketsOut( plr->getPacketsOut() + 1 );
                plr->setBytesOut( plr->getBytesOut() + bOut );
                break;
            }
        }
    }

    if ( bOut >= 1 )
        this->setBytesOut( this->getBytesOut() + bOut );
}

quint64 ServerInfo::sendToAllConnected(QString packet)
{
    Player* tmpPlr{ nullptr };
    quint64 tmpBOut{ 0 };
    quint64 bOut{ 0 };

    QTcpSocket* tmpSoc{ nullptr };
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = this->getPlayer( i );
        if ( tmpPlr != nullptr )
        {
            tmpSoc = tmpPlr->getSocket();
            if ( tmpSoc != nullptr )
            {
                tmpBOut = tmpSoc->write( packet.toLatin1(),
                                         packet.length() );
                tmpPlr->setBytesOut( tmpPlr->getBytesOut() + tmpBOut );
                tmpPlr->setPacketsOut( tmpPlr->getPacketsOut() + 1 );

                bOut += tmpBOut;
            }
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

int ServerInfo::getVersionID() const
{
    return versionID;
}

void ServerInfo::setVersionID(int value)
{
    versionID = value;
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

quint32 ServerInfo::getPlayerCount() const
{
    return playerCount;
}

void ServerInfo::setPlayerCount(quint32 value)
{
    if ( value <= 0 )
    {
        this->setGameInfo( "" );
        value = 0;
    }
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

quint64 ServerInfo::getBytesOut() const
{
    return bytesOut;
}

void ServerInfo::setBytesOut(const quint64& value)
{
    bytesOut = value;
}

void ServerInfo::setBaudIO(const quint64& bytes, quint64& baud)
{
    quint64 time = baudTime.elapsed();

    if ( bytes > 0 && time > 0 )
        baud = 10000 * bytes / time;
}

quint64 ServerInfo::getBaudIn() const
{
    return baudIn;
}

quint64 ServerInfo::getBaudOut() const
{
    return baudOut;
}

bool ServerInfo::getLogFiles() const
{
    return logFiles;
}

void ServerInfo::setLogFiles(bool value)
{
    logFiles = value;
}

bool ServerInfo::getSentUDPCheckin() const
{
    return sentUDPCheckin;
}

void ServerInfo::setSentUDPCheckIn(bool value)
{
    sentUDPCheckin = value;
}

bool ServerInfo::getMasterUDPResponse() const
{
    return masterUDPResponse;
}

void ServerInfo::setMasterUDPResponse(bool value)
{
    masterUDPResponse = value;
    if ( masterUDPResponse )
    {
        this->setMasterTimedOut( false );
        masterCheckIn.setInterval( 300000 );
    }
    else if ( this->getMasterTimedOut() )
        masterCheckIn.setInterval( 2000 );
}

bool ServerInfo::getMasterTimedOut()
{
    return masterTimedOut;
}

void ServerInfo::setMasterTimedOut(bool value)
{
    masterTimedOut = value;
    if ( masterTimedOut )
        this->setMasterUDPResponse( false );
    else
       masterTimeOut.stop();
}

void ServerInfo::startMasterCheckIn()
{
    masterCheckIn.start();
}

void ServerInfo::stopMasterCheckIn()
{
    masterCheckIn.stop();
}

quint32 ServerInfo::getUsageHours() const
{
    return usageHours;
}

quint32 ServerInfo::getUsageDays() const
{
    return usageDays;
}

quint32 ServerInfo::getUsageMins() const
{
    return usageMins;
}
