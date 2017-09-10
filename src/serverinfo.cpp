
#include "includes.hpp"
#include "serverinfo.hpp"

ServerInfo::ServerInfo(QString svrID)
{
    serverTabID = svrID;
    masterSocket = new QUdpSocket();

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        players[ i ] = nullptr;
    }

    baudTime.start();
    upTimer.start( 500 ); //Refresh the UI ever .5 seconds.

    QObject::connect( &upTimer, &QTimer::timeout, &upTimer,
    [=]()
    {
        upTime.setValue( upTime.toDouble() + 0.5 );
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
    QObject::connect( &masterTimeOut, &QTimer::timeout, &masterTimeOut,
    [=]()
    {
        this->setMasterTimedOut( true );
    });

    QObject::connect( &masterCheckIn, &QTimer::timeout, &masterCheckIn,
    [=]()
    {
        this->sendMasterInfo();
        if ( !masterTimeOut.isActive() )
            masterTimeOut.start();
    });

    //Updates the Server's Server Usage array every 10 minutes.
    usageUpdate.start( SERVER_USAGE_UPDATE );
    QObject::connect( &usageUpdate, &QTimer::timeout, &usageUpdate,
    [=]()
    {
        usageArray[ usageCounter ] = this->getPlayerCount();

        usageDays = 0;
        usageHours = 0;
        usageMins = 0;

        quint32 usageCap{ 0 };
        qint32 code{ 0 };
        for ( uint i = 0; i < SERVER_USAGE_48_HOURS; ++i )
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

void ServerInfo::setupInfo()
{
    QHostAddress addr{ Helper::getPrivateIP() };
    this->initMasterSocket( addr, this->getPrivatePort() );

    if ( !this->getIsSetUp() )
    {
        this->setPrivateIP( addr.toString() );
        this->setIsSetUp( true );
    }
}

void ServerInfo::setupUPNP(bool isDisable)
{
    upnp = UPNP::getUpnp();
    if ( upnp == nullptr )
        return;

    if ( !isDisable )
    {
        bool tunneled = UPNP::getTunneled();
        if ( !tunneled )
        {
            QObject::connect( upnp, &UPNP::success, upnp,
            [=]()
            {
                upnp->checkPortForward( "TCP", this->getPrivatePort() );
                upnp->checkPortForward( "UDP", this->getPrivatePort() );
            });
            upnp->makeTunnel( this->getPrivatePort(), this->getPublicPort() );
        }
        else
        {
            upnp->checkPortForward( "TCP", this->getPrivatePort() );
            upnp->checkPortForward( "UDP", this->getPrivatePort() );
        }
    }
    else
    {
        //Add a delay of one second after each removal command.
        //This is to ensure the command is sent.
        upnp->removePortForward( "TCP", this->getPrivatePort() );
        upnp->removePortForward( "UDP", this->getPrivatePort() );
    }
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
    if ( masterSocket->state() == QAbstractSocket::BoundState )
        masterSocket->close();

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
                      "//ReMix[ %7 ]" };

    response = response.arg( this->getName() );
    if ( !this->getGameInfo().isEmpty() )
        response = response.arg( " [" % this->getGameInfo() % "]" );
    else
        response = response.arg( "" );

    QString usage{ "%1.%2.%3" };
            usage = usage.arg( this->getUsageMins() )
                         .arg( this->getUsageHours() )
                         .arg( this->getUsageDays() );

    response = response.arg( Rules::getRuleSet( serverTabID ) )
                       .arg( this->getServerID() )
                       .arg( Helper::intToStr( QDateTime::currentDateTime()
                                                    .toTime_t(), 16, 8 ) )
                       .arg( usage )
                       .arg( QString( REMIX_VERSION ) );

    if ( !response.isEmpty() )
        this->sendUDPData( addr, port, response );
}

void ServerInfo::sendUserList(QHostAddress& addr, quint16 port, quint32 type)
{
    if ( addr.isNull() )
        return;

    Player* plr{ nullptr };

    QString response{ "Q" };
    if ( type == R_Response )
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
            response = "!version=%1,nump=%2,gameid=%3,game=%4,host=%5,id=%6,"
                       "port=%7,info=%8,name=%9";
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
        //Store the Master Server Check-In time.
        this->setMasterPingSendTime( QDateTime::currentMSecsSinceEpoch() );

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
        if ( Settings::getLogFiles() )
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
                               .arg( plr->getAvgBaud( false ) )
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

void ServerInfo::sendServerRules(Player* plr)
{
    QTcpSocket* soc{ nullptr };
    if ( plr == nullptr )
        return;

    soc = plr->getSocket();
    if ( soc == nullptr )
        return;

    qint64 bOut{ 0 };
    QString rules{ ":SR$%1\r\n" };
            rules = rules.arg( Rules::getRuleSet( serverTabID ) );

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
    QString greeting = Settings::getMOTDMessage( serverTabID );
    if ( Settings::getRequirePassword() )
    {
        greeting.append( " Password required: Please reply with (/login *PASS)"
                         " or be disconnected." );
        plr->setSvrPwdRequested( true );
    }

    if ( !greeting.isEmpty() )
        plr->sendMessage( greeting );

    if ( !Rules::getRuleSet( serverTabID ).isEmpty() )
        this->sendServerRules( plr );
}

void ServerInfo::sendMasterMessage(QString packet, Player* plr, bool toAll)
{
    QString msg = QString( ":SR@M%1\r\n" )
                      .arg( packet );
    QTcpSocket* soc{ nullptr };
    if ( plr != nullptr )
        soc = plr->getSocket();

    qint64 bOut{ 0 };
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

//        Player* tmpPlr{ nullptr };
//        for ( int i = 0; i < MAX_PLAYERS; ++i )
//        {
//            tmpPlr = this->getPlayer( i );
//            if ( plr == tmpPlr )
//            {
                bOut = soc->write( msg.toLatin1(),
                                   msg.length() );
                plr->setPacketsOut( plr->getPacketsOut() + 1 );
                plr->setBytesOut( plr->getBytesOut() + bOut );
//                break;
//            }
//        }
    }

    if ( bOut >= 1 )
        this->setBytesOut( this->getBytesOut() + bOut );
}

qint64 ServerInfo::sendToAllConnected(QString packet)
{
    Player* tmpPlr{ nullptr };
    qint64 tmpBOut{ 0 };
    qint64 bOut{ 0 };

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
    return upTime.toULongLong();
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

void ServerInfo::setMasterPort(quint16 value)
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
    Settings::setIsPublic( QVariant( value ), this->getName() );

    this->setMasterUDPResponse( false );
    this->setSentUDPCheckIn( false );

    if ( value )
    {
        if ( !this->getIsSetUp() )
            this->setupInfo();

        //Tell the server to use a UPNP Port Forward.
        this->setupUPNP( false );

        this->startMasterCheckIn();

        Server* server{ this->getTcpServer() };
        if ( server != nullptr )
            server->setupServerInfo();
    }
    else
    {
        //Disconnect from the Master Server if applicable.
        this->stopMasterCheckIn();
        this->sendMasterInfo( true );
        this->setupUPNP( true );
    }
}

bool ServerInfo::getIsMaster() const
{
    return isMaster;
}

void ServerInfo::setIsMaster(bool value)
{
    isMaster = value;
}

QString ServerInfo::getServerID() const
{
    return serverID;
}

void ServerInfo::setServerID(QString value)
{
    serverID = value;
}

qint32 ServerInfo::getPlayerCount() const
{
    return playerCount;
}

void ServerInfo::setPlayerCount(qint32 value)
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

quint16 ServerInfo::getPrivatePort() const
{
    return privatePort;
}

void ServerInfo::setPrivatePort(quint16 value)
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

quint32 ServerInfo::getUserPings() const
{
    return userPings;
}

void ServerInfo::setUserPings(const quint32& value)
{
    userPings = value;
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

qint64 ServerInfo::getBytesIn() const
{
    return bytesIn;
}

void ServerInfo::setBytesIn(const qint64& value)
{
    bytesIn = value;
}

qint64 ServerInfo::getBytesOut() const
{
    return bytesOut;
}

void ServerInfo::setBytesOut(const qint64& value)
{
    bytesOut = value;
}

void ServerInfo::setBaudIO(const qint64& bytes, qint64& baud)
{
    qint64 time = baudTime.elapsed();
    if ( bytes > 0 && time > 0 )
        baud = 10000 * bytes / time;
}

qint64 ServerInfo::getBaudIn() const
{
    return baudIn;
}

qint64 ServerInfo::getBaudOut() const
{
    return baudOut;
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
        masterCheckIn.setInterval( MAX_MASTER_CHECKIN_TIME );
    }
    else if ( this->getMasterTimedOut() )
        masterCheckIn.setInterval( MIN_MASTER_CHECK_IN_TIME );
}

bool ServerInfo::getMasterTimedOut()
{
    return masterTimedOut;
}

void ServerInfo::setMasterTimedOut(bool value)
{
    masterTimedOut = value;
    if ( masterTimedOut )
    {
        this->setMasterUDPResponse( false );
        this->setMasterPingFailCount( this->getMasterPingFailCount() + 1 );
    }
    else
       masterTimeOut.stop();
}

void ServerInfo::startMasterCheckIn()
{
    //Every 2 Seconds we will attempt to Obtain Master Info.
    //This will be set to 300000 (5-Minutes) once Master info is obtained.

    masterCheckIn.setInterval( MIN_MASTER_CHECK_IN_TIME );
    masterCheckIn.start();
}

void ServerInfo::stopMasterCheckIn()
{
    masterCheckIn.stop();
}

qint64 ServerInfo::getMasterPingSendTime() const
{
    return masterPingSendTime;
}

void ServerInfo::setMasterPingSendTime(const qint64& value)
{
    masterPingSendTime = value;
}

qint64 ServerInfo::getMasterPingRespTime() const
{
    return masterPingRespTime;
}

void ServerInfo::setMasterPingRespTime(const qint64& value)
{
    masterPingRespTime = value;
    //Store the Master Server's Response Count.
    this->setMasterPingCount( this->getMasterPingCount() + 1 );
    this->setMasterPing();
}

double ServerInfo::getMasterPingTrend() const
{
    return masterPingTrend;
}

void ServerInfo::setMasterPingTrend(double value)
{
    masterPingTrend = masterPingTrend * 0.9 + value * 0.1;
}

double ServerInfo::getMasterPingAvg() const
{
    if ( masterPingAvg > 0 )
        return masterPingAvg / this->getMasterPingCount();

    return masterPingAvg;
}

void ServerInfo::setMasterPingAvg(const double& value)
{
    masterPingAvg += value;
}

qint32 ServerInfo::getMasterPingFailCount() const
{
    return masterPingFailCount;
}

void ServerInfo::setMasterPingFailCount(const qint32& value)
{
    masterPingFailCount = value;
}

qint32 ServerInfo::getMasterPingCount() const
{
    return masterPingCount;
}

void ServerInfo::setMasterPingCount(const qint32& value)
{
    masterPingCount = value;
}

double ServerInfo::getMasterPing() const
{
    return masterPing;
}

void ServerInfo::setMasterPing()
{
    masterPing = this->getMasterPingRespTime()
                 - this->getMasterPingSendTime();

    this->setMasterPingAvg( masterPing );
    this->setMasterPingTrend( masterPing );
}

qint32 ServerInfo::getUsageHours() const
{
    return usageHours;
}

qint32 ServerInfo::getUsageDays() const
{
    return usageDays;
}

qint32 ServerInfo::getUsageMins() const
{
    return usageMins;
}

Server* ServerInfo::getTcpServer() const
{
    return tcpServer;
}

void ServerInfo::setTcpServer(Server* value)
{
    tcpServer = value;
}
