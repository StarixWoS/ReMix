
//Class includes.
#include "serverinfo.hpp"

//ReMix Widget includes.
#include "widgets/userdelegate.hpp"

//ReMix includes.
#include "settings.hpp"
#include "player.hpp"
#include "server.hpp"
#include "helper.hpp"
#include "rules.hpp"
#include "upnp.hpp"

//Qt Includes.
#include <QHostAddress>
#include <QUdpSocket>
#include <QtCore>
#include <QFile>

ServerInfo::ServerInfo()
{
    masterSocket = new QUdpSocket();

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        players[ i ] = nullptr;
    }

    baudTime.start();
    upTimer.start( UI_UPDATE_TIME );

    QObject::connect( &upTimer, &QTimer::timeout, &upTimer,
    [=]()
    {
        upTime.setValue( upTime.toDouble() + 0.5 );
        if ( baudTime.elapsed() > BAUD_UPDATE_TIME )
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

void ServerInfo::setupUPNP(const bool& enable)
{
    upnp = UPNP::getInstance();
    if ( upnp == nullptr )
        return;

    if ( enable )
    {
        bool tunneled = UPNP::getTunneled();
        if ( !tunneled )
        {
            QObject::connect( upnp, &UPNP::success, upnp,
            [=]()
            {
                upnp->addPortForward( "TCP", this->getPrivatePort() );
                upnp->addPortForward( "UDP", this->getPrivatePort() );
                upnp->disconnect();
            });
            upnp->makeTunnel();
        }
        else
        {
            upnp->addPortForward( "TCP", this->getPrivatePort() );
            upnp->addPortForward( "UDP", this->getPrivatePort() );
        }
    }
    else //Remove our UPNP Port Forward.
    {
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

bool ServerInfo::initMasterSocket(const QHostAddress& addr, const quint16& port)
{
    if ( masterSocket->state() == QAbstractSocket::BoundState )
        masterSocket->close();

    return masterSocket->bind( addr, port );
}

void ServerInfo::sendUDPData(const QHostAddress& addr, const quint16& port,
                             const QString& data)
{
    if ( masterSocket != nullptr )
        masterSocket->writeDatagram( data.toLatin1(), data.size() + 1,
                                     addr, port );
}

void ServerInfo::sendServerInfo(const QHostAddress& addr, const quint16& port)
{
    if ( addr.isNull() )
        return;

    QString serverName{ this->getName() };
    QString sGameInfo{ this->getGameInfo() };
    QString response{ "#name=%1%2 //Rules: %3 //ID:%4 //TM:%5 //US:%6 "
                      "//ReMix[ %7 ]" };

    if ( !sGameInfo.isEmpty() )
        sGameInfo = " [world=" % sGameInfo % "]";

    response = response.arg( serverName,
                             sGameInfo,
                             Rules::getRuleSet( serverName ),
                             this->getServerID(),
                             Helper::intToStr( QDateTime::currentDateTime()
                                                    .toTime_t(), 16, 8 ),
                             this->getUsageString(),
                             QString( REMIX_VERSION ) );

    if ( !response.isEmpty() )
        this->sendUDPData( addr, port, response );
}

void ServerInfo::sendUserList(const QHostAddress& addr, const quint16& port,
                              const quint32& type)
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
            //Don't show Invisible Administrators on the User List.
            if ( !plr->getIsInvisible() )
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
    }

    if ( !response.isEmpty() )
        this->sendUDPData( addr, port, response );
}

void ServerInfo::sendMasterInfo(const bool& disconnect)
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
            response = response.arg( QString::number(
                                         this->getVersionID() ),
                                     QString::number(
                                         this->getPlayerCount() ),
                                     QString::number(
                                         static_cast<int>( this->getGameId() ) ),
                                     this->getGameName(),
                                     this->getHostInfo().localHostName(),
                                     this->getServerID(),
                                     QString::number(
                                         this->getPrivatePort() ),
                                     this->getGameInfo(),
                                     this->getName() );
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

Player* ServerInfo::createPlayer(const int& slot)
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

Player* ServerInfo::getPlayer(const int& slot)
{
    return players[ slot ];
}

void ServerInfo::deletePlayer(const int& slot)
{
    Player* plr = this->getPlayer( slot );
    if ( plr != nullptr )
    {
        QString logMsg{ "Client: [ %1 ] was on for %2 minutes and sent %3 "
                        "bytes in %4 packets, averaging %5 baud [ %6 ]" };
        if ( plr != nullptr )
        {
            logMsg = logMsg.arg( plr->getPublicIP(),
                                 QString::number(
                                     Helper::getTimeIntFormat(
                                         plr->getConnTime(),
                                         TimeFormat::Minutes ) ),
                                 QString::number(
                                     plr->getBytesIn() ),
                                 QString::number(
                                     plr->getPacketsIn() ),
                                 QString::number(
                                     plr->getAvgBaud( false ) ),
                                 plr->getBioData() );
            Helper::logToFile( Helper::USAGE, logMsg, true, true );
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

    QString serverName{ this->getName() };
    QString rules{ ":SR$%1\r\n" };
            rules = rules.arg( Rules::getRuleSet( serverName ) );

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
    QString serverName{ this->getName() };
    QString greeting = Settings::getMOTDMessage( serverName );
    if ( Settings::getRequirePassword() )
    {
        greeting.append( " Password required: Please reply with (/login *PASS)"
                         " or be disconnected." );
        plr->setSvrPwdRequested( true );
    }

    if ( !greeting.isEmpty() )
        plr->sendMessage( greeting );

    if ( !Rules::getRuleSet( serverName ).isEmpty() )
        this->sendServerRules( plr );
}

void ServerInfo::sendMasterMessage(const QString& packet, Player* plr,
                                   const bool toAll)
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
        bOut = soc->write( msg.toLatin1(),
                           msg.length() );
        plr->setPacketsOut( plr->getPacketsOut() + 1 );
        plr->setBytesOut( plr->getBytesOut() + bOut );
    }

    if ( bOut >= 1 )
        this->setBytesOut( this->getBytesOut() + bOut );
}

qint64 ServerInfo::sendToAllConnected(const QString& packet)
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

Games ServerInfo::getGameId() const
{
    return gameId;
}

void ServerInfo::setGameId(const QString& gameName)
{
    Games gameID{ Games::Invalid };
    if ( Helper::cmpStrings( gameName, "WoS" ) )
        gameID = Games::WoS;
    else if ( Helper::cmpStrings( gameName, "ToY" ) )
        gameID = Games::ToY;
    else if ( Helper::cmpStrings( gameName, "W97" ) )
        gameID = Games::W97;

    gameId = gameID;
}

QString ServerInfo::getGameName() const
{
    return gameName;
}

void ServerInfo::setGameName(const QString& value)
{
    gameName = value;
    this->setGameId( value );

    Settings::setGameName( value, this->getName() );
}

QHostInfo ServerInfo::getHostInfo() const
{
    return hostInfo;
}

int ServerInfo::getVersionID() const
{
    return versionID;
}

void ServerInfo::setVersionID(const int& value)
{
    versionID = value;
}

quint16 ServerInfo::getMasterPort() const
{
    return masterPort;
}

void ServerInfo::setMasterPort(const quint16& value)
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

void ServerInfo::setIsPublic(const bool& value)
{
    isPublic = value;
    Settings::setIsPublic( value, this->getName() );

    this->setMasterUDPResponse( false );
    this->setSentUDPCheckIn( false );

    if ( value )
    {
        if ( !this->getIsSetUp() )
            this->setupInfo();

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
    }
}

bool ServerInfo::getUseUPNP() const
{
    return useUPNP;
}

void ServerInfo::setUseUPNP(const bool& value)
{
    //Tell the server to use a UPNP Port Forward.

    Settings::setUseUPNP( value, this->getName() );
    if ( useUPNP != value )
    {
        if ( !this->getIsSetUp() )
        {
            //Catch a possible race condition with a signal connection.
            QObject::connect( this, &ServerInfo::serverIsSetup,
            [=]()
            {
                this->setupUPNP( value );
            });
        }
        else
            this->setupUPNP( value );
    }
    useUPNP = value;
}

bool ServerInfo::getIsMaster() const
{
    return isMaster;
}

void ServerInfo::setIsMaster(const bool& value)
{
    isMaster = value;
}

QString ServerInfo::getServerID() const
{
    return serverID;
}

void ServerInfo::setServerID(const QString& value)
{
    serverID = value;
}

quint32 ServerInfo::getPlayerCount() const
{
    return playerCount;
}

void ServerInfo::setPlayerCount(const quint32& value)
{
    if ( value <= 0 )
    {
        this->setGameInfo( Rules::getWorldName( this->getName() ) );
        playerCount = 0;
    }
    else
        playerCount = value;
}

quint16 ServerInfo::getPublicPort() const
{
    return publicPort;
}

void ServerInfo::setPublicPort(const quint16& value)
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

void ServerInfo::setPrivatePort(const quint16& value)
{
    privatePort = value;
    Settings::setPortNumber( value, this->getName() );
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

void ServerInfo::setIsSetUp(const bool& value)
{
    isSetUp = value;
    emit this->serverIsSetup();
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

void ServerInfo::setSentUDPCheckIn(const bool& value)
{
    sentUDPCheckin = value;
}

bool ServerInfo::getMasterUDPResponse() const
{
    return masterUDPResponse;
}

void ServerInfo::setMasterUDPResponse(const bool& value)
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

bool ServerInfo::getMasterTimedOut() const
{
    return masterTimedOut;
}

void ServerInfo::setMasterTimedOut(const bool& value)
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

void ServerInfo::setMasterPingTrend(const double& value)
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

QString ServerInfo::getUsageString()
{
    return QString( "%1.%2.%3" )
            .arg( QString::number( this->getUsageMins() ),
                  QString::number( this->getUsageHours() ),
                  QString::number( this->getUsageDays() ) );
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
