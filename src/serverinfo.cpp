
//Class includes.
#include "serverinfo.hpp"

//ReMix Widget includes.
#include "widgets/settingswidget.hpp"
#include "widgets/userdelegate.hpp"

//ReMix Threaded Includes.
#include "thread/udpthread.hpp"

//ReMix includes.
#include "packethandler.hpp"
#include "packetforge.hpp"
#include "settings.hpp"
#include "player.hpp"
#include "server.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "user.hpp"
#include "upnp.hpp"

//Qt Includes.
#include <QRegularExpression>
#include <QHostAddress>
#include <QtConcurrent>
#include <QUdpSocket>
#include <QtCore>
#include <QFile>

ServerInfo::ServerInfo()
{
    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &ServerInfo::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );

    QThread* thread{ new QThread() };
    udpThread = UdpThread::getNewUdpThread( nullptr );
    udpThread->moveToThread( thread );

    //Connect signals from the UdpThread class to the slots within the ServerInfo class.
    QObject::connect( udpThread, &UdpThread::increaseServerPingsSignal, this, &ServerInfo::increaseServerPingSlot );
    QObject::connect( udpThread, &UdpThread::sendServerInfoSignal, this, &ServerInfo::sendServerInfoSlot );
    QObject::connect( udpThread, &UdpThread::sendUserListSignal, this, &ServerInfo::sendUserListSlot );
    QObject::connect( udpThread, &UdpThread::udpDataSignal, this, &ServerInfo::udpDataSlot );
    QObject::connect( udpThread, &UdpThread::dataOutSizeSignal, this, &ServerInfo::dataOutSizeSlot );

    //Connect signals from the ServerInfo class to the slots within the UdpThread class.
    QObject::connect( this, &ServerInfo::serverWorldChangedSignal, udpThread, &UdpThread::serverWorldChangedSlot );
    QObject::connect( this, &ServerInfo::serverUsageChangedSignal, udpThread, &UdpThread::serverUsageChangedSlot );
    QObject::connect( this, &ServerInfo::serverNameChangedSignal, udpThread, &UdpThread::serverNameChangedSlot );
    QObject::connect( this, &ServerInfo::serverIDChangedSignal, udpThread, &UdpThread::serverIDChangedSlot );
    QObject::connect( this, &ServerInfo::closeUdpSocketSignal, udpThread, &UdpThread::closeUdpSocketSlot, Qt::BlockingQueuedConnection );
    QObject::connect( this, &ServerInfo::sendUdpDataSignal, udpThread, &UdpThread::sendUdpDataSlot );
    QObject::connect( this, &ServerInfo::bindSocketSignal, udpThread, &UdpThread::bindSocketSlot );

    //Connect the MasterMixThread signals to the ServerInfo Slots.
    QObject::connect( SettingsWidget::getInstance(), &SettingsWidget::masterMixIPChangedSignal, this, &ServerInfo::masterMixIPChangedSlot );

    //Connect the ServerInfo Class Signals to the UPNP Class Slots.
    upnp = UPNP::getInstance();
    QObject::connect( this, &ServerInfo::upnpPortForwardSignal, upnp, &UPNP::upnpPortForwardSlot );
    QObject::connect( upnp, &UPNP::upnpPortAddedSignal, this, &ServerInfo::upnpPortAddedSlot );

    upnpPortRefresh.setInterval( UPNP_TIME_OUT_MS );
    QObject::connect( &upnpPortRefresh, &QTimer::timeout, &upnpPortRefresh,
    [=,this]()
    {
        emit this->upnpPortForwardSignal( this->getPrivatePort(), this->getUseUPNP() );
    } );

    players.resize( static_cast<int>( MAX_PLAYERS ) );
    players.fill( nullptr );

    upTimer.start( UI_UPDATE_TIME );

    QObject::connect( &upTimer, &QTimer::timeout, &upTimer,
    [=,this]()
    {
        emit this->connectionTimeUpdateSignal();
    } );

    masterTimeOut.setInterval( MAX_MASTER_RESPONSE_TIME );
    masterTimeOut.setSingleShot( true );
    QObject::connect( &masterTimeOut, &QTimer::timeout, &masterTimeOut,
    [=,this]()
    {
        this->setMasterTimedOut( true );
    } );

    QObject::connect( &masterCheckIn, &QTimer::timeout, &masterCheckIn,
    [=,this]()
    {
        this->sendMasterInfo();
        if ( !masterTimeOut.isActive() )
            masterTimeOut.start();
    } );

    //Updates the Server's Server Usage array every 10 minutes.
    usageUpdate.start( SERVER_USAGE_UPDATE );
    QObject::connect( &usageUpdate, &QTimer::timeout, &usageUpdate,
    [=,this]()
    {
        usageArray[ usageCounter ] = this->getPlayerCount();

        usageDays = 0;
        usageHours = 0;
        usageMins = 0;

        quint32 usageCap{ 0 };
        quint32 code{ 0 };
        for ( uint i = 0; i < SERVER_USAGE_48_HOURS; ++i )
        {
            code = usageArray[ ( i + usageCounter ) % SERVER_USAGE_48_HOURS ];
            usageCap = ( SERVER_USAGE_48_HOURS - 1 ) - i;
            if ( usageCap < SERVER_USAGE_DAYS )
            {
                usageDays += code;
                if ( usageCap < SERVER_USAGE_HOURS )
                {
                    usageHours += code;
                    if ( usageCap < SERVER_USAGE_MINUTES )
                        usageMins += code;
                }
            }
        }
        emit this->serverUsageChangedSignal( usageMins, usageDays, usageHours );

        ++usageCounter;
        usageCounter %= SERVER_USAGE_48_HOURS;
    } );

    this->setInitializeDate( QDateTime::currentDateTime().currentSecsSinceEpoch() );
    udpThread->start();
    thread->start();
}

ServerInfo::~ServerInfo()
{
    QThread* thread{ udpThread->thread() };
    emit this->closeUdpSocketSignal();
    if ( thread != nullptr )
    {
        thread->deleteLater();
        thread->exit();
    }

    for ( Player* plr : this->getPlayerVector() )
    {
        if ( plr != nullptr )
            plr->deleteLater();
    }
    upTimer.disconnect();
    upnpPortRefresh.disconnect();

    thread = nullptr;
}

void ServerInfo::setupInfo()
{
    QHostAddress addr{ Helper::getPrivateIP() };

    emit this->bindSocketSignal( addr, this->getPrivatePort() );
    if ( !this->getIsSetUp() )
    {
        this->setPrivateIP( addr.toString() );
        this->setIsSetUp( true );
    }
}

void ServerInfo::setupUPNP(const bool& enable)
{
    if ( !enable )
        this->setUpnpPortAdded( false );

    emit this->upnpPortForwardSignal( this->getPrivatePort(), enable );
}

QString ServerInfo::getMasterInfoHost() const
{
    return masterInfoHost;
}

void ServerInfo::setMasterInfoHost(const QString& value)
{
    //Try to validate a URL.
    QRegularExpression isUrl( "http[s]?|ftp", QRegularExpression::CaseInsensitiveOption );

    QString url{ value };
    QString scheme{ QUrl( url ).scheme() };
    QRegularExpressionMatch match = isUrl.match( scheme );

    QFile synRealIni( "synreal.ini" );
    if ( masterInfoHost != value
      && match.hasMatch() )
    {
        synRealIni.remove();
    }
    else
        url = "http://synthetic-reality.com/synreal.ini";

    masterInfoHost = url;
}

QString ServerInfo::getServerInfoString()
{
    QString response{ "#name=%1%2 //Rules: %3 //ID:%4 //TM:%5 //US:%6 //ReMix[ %7 ]" };
    QString serverName{ this->getServerName() };
    QString sGameInfo{ this->getGameInfo() };

    if ( !sGameInfo.isEmpty() )
        sGameInfo = " [world=" % sGameInfo % "]";

    response = response.arg( serverName )
                       .arg( sGameInfo )
                       .arg( Settings::getRuleSet( serverName ) )
                       .arg( this->getServerID() )
                       .arg( Helper::intToStr( QDateTime::currentDateTimeUtc().toSecsSinceEpoch(), static_cast<int>( IntBase::HEX ), 8 ) )
                       .arg( this->getUsageString() )
                       .arg( QString( REMIX_VERSION ) );

    if ( !response.isEmpty() )
        return response;

    return QString( "" );
}

void ServerInfo::sendServerInfo(const QHostAddress& addr, const quint16& port)
{
    if ( addr.isNull() )
        return;

    QString response{ this->getServerInfoString() };
    if ( !response.isEmpty() )
    {
        //this->sendUDPData( addr, port, response );
        QString msg{ "Sending Server Info to [ %1:%2 ]; %3" };
                msg = msg.arg( addr.toString() )
                         .arg( port )
                         .arg( response );
        emit this->insertLogSignal( this->getServerName(), msg, LogTypes::PING, true, true );
    }
}

void ServerInfo::sendUserList(const QHostAddress& addr, const quint16& port, const UserListResponse& type)
{
    if ( addr.isNull() )
        return;

    Player* plr{ nullptr };

    QString response{ "Q" };
    if ( type == UserListResponse::R_Response )
        response = "R";

    QString filler_Q{ "%1," };
    QString filler_R{ "%1|%2," };

    //Note: The 'R' Response sent by the Syn-Real Mix Server
    //Sends the User's IP address within field(%2) 2; we are sending the User's
    //position within the players[] array instead to prevent IP leakage.

    //Hide any Userlist pings from the LogView if no Player's are found.
    bool emptyResponse{ true };
    for ( int i = 0; i < MAX_PLAYERS && response.length() < 800; ++i )
    {
        plr = this->getPlayer( i );
        if ( plr != nullptr
          && plr->getSernum_i() != 0 )
        {
            //Don't show Invisible Administrators on the User List.
            if ( plr->getIsVisible() )
            {
                if ( type == UserListResponse::Q_Response ) //Standard 'Q' Response.
                {
                    response += filler_Q.arg( Helper::intToStr( plr->getSernum_i(), static_cast<int>( IntBase::HEX ) ) );
                }
                else if ( type == UserListResponse::R_Response ) //Non-Standard 'R' Response
                {
                    response += filler_R.arg( Helper::intToStr( plr->getSernum_i(), static_cast<int>( IntBase::HEX ) ) )
                                        .arg( Helper::intToStr( plr->getSlotPos(), static_cast<int>( IntBase::HEX ), 8 ) );
                }
            }
            emptyResponse = false;
        }
    }

    emit this->sendUdpDataSignal( addr, port, response );
    if ( !emptyResponse )
    {
        QString msg{ "Sending User List to [ %1:%2 ]; %3" };
                msg = msg.arg( addr.toString() )
                         .arg( port )
                         .arg( response );
        emit this->insertLogSignal( this->getServerName(), msg, LogTypes::PING, true, true );
    }
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
            response = "!version=%1,nump=%2,gameid=%3,game=%4,host=%5,id=%6,port=%7,info=%8,name=%9";
            response = response.arg( this->getVersionID() )
                               .arg( this->getPlayerCount() )
                               .arg( static_cast<int>( this->getGameId() ) )
                               .arg( this->getGameName() )
                               .arg( this->getHostInfo().localHostName() )
                               .arg( this->getServerID() )
                               .arg( this->getPrivatePort() )
                               .arg( this->getGameInfo() )
                               .arg( this->getServerName() );
        }
    }

    if ( !response.isEmpty()
      && this->getIsSetUp()
      && !addr.isNull() )
    {
        QString msg{ "Sending Master Check-In to [ %1:%2 ]; %3" };
                msg = msg.arg( addr.toString() )
                         .arg( port )
                         .arg( response );
        if ( !disconnect )
        {
            this->setMasterPingSendTime( QDateTime::currentMSecsSinceEpoch() );
            this->setSentUDPCheckIn( true );
        }
        else
            msg.append( " [ Disconnect ]." );

        emit this->insertLogSignal( this->getServerName(), msg, LogTypes::MASTERMIX, true, true );
        emit this->sendUdpDataSignal( addr, port, response );
    }
    else
        this->setSentUDPCheckIn( false );

    if ( !disconnect )
    {
        masterCheckIn.setInterval( MAX_MASTER_CHECKIN_TIME );
        this->setMasterTimedOut( false );
    }
}

Player* ServerInfo::createPlayer(const int& slot, qintptr socketDescriptor)
{
    if ( slot >= 0 && slot < MAX_PLAYERS )
    {
        Player* plr{ new Player( socketDescriptor ) };

        players[ slot ] = plr;
        plr->setSlotPos( slot );
        plr->setMaxIdleTimeSlot( this->getMaxIdleTime() );

        this->setPlayerCount( this->getPlayerCount() + 1 );

        QObject::connect( pktHandle, &PacketHandler::sendPacketToPlayerSignal, players[ slot ], &Player::sendPacketToPlayerSlot );
        QObject::connect( this, &ServerInfo::setMaxIdleTimeSignal, players[ slot ], &Player::setMaxIdleTimeSlot );
        QObject::connect( this, &ServerInfo::sendMasterMsgToPlayerSignal, players[ slot ], &Player::sendMasterMsgToPlayerSlot );
        QObject::connect( this, &ServerInfo::connectionTimeUpdateSignal, players[ slot ], &Player::connectionTimeUpdateSlot );

        return plr;
    }
    return nullptr;
}

Player* ServerInfo::getPlayer(const int& slot)
{
    //Return the Player object within the position [ slot ] of the Players vector.
    //Do not return Player objects which are 'valid' but in a disconnected state.

    Player* plr{ nullptr };
    if ( slot >= 0 )
        plr = this->getPlayerVector().at( slot );

    if ( plr != nullptr )
    {
        //Reduce instances where a disconnected Player object is not cleared
        if ( plr->getIsDisconnected() )
            plr = nullptr;
    }
    return plr;
}

void ServerInfo::deletePlayer(Player* plr, const bool& timedOut)
{
    Player* player{ plr };
    if ( player == nullptr )
        return;

    int slot{ player->getSlotPos() };

    QString timeOut{ "" };
    if ( timedOut == true )
        timeOut = " Timed Out";

    QString bytesUnit{ "" };
    QString bytesSec{ "" };
    Helper::sanitizeToFriendlyUnits( player->getBytesIn(), bytesSec, bytesUnit );

    QString logMsg{ "Client%1: [ %2 ] was on for %3 minutes and sent %4 %5 in %6 packets, [ %7 ]" };
            logMsg = logMsg.arg( timeOut )
                           .arg( player->peerAddress().toString() )
                           .arg( Helper::getTimeIntFormat( player->getConnTime(), TimeFormat::Minutes ) )
                           .arg( bytesSec )
                           .arg( bytesUnit )
                           .arg( player->getPacketsIn() )
                           .arg( player->getBioData() );

    plrSlotMap.remove( player->getPktHeaderSlot() );
    player->disconnect();
    player->deleteLater();
    player = nullptr;

    players[ slot ] = nullptr;

    this->setPlayerCount( this->getPlayerCount() - 1 );
    emit this->insertLogSignal( this->getServerName(), logMsg, LogTypes::CLIENT, true, true );
}

Player* ServerInfo::getLastPlayerInStorage(Player* plr)
{
    Player* tmpPlayer{ nullptr };
    for ( Player* tmpPlr : this->getPlayerVector() )
    {
        if ( tmpPlr == nullptr )
            break;

        if ( tmpPlayer != plr )
            tmpPlayer = tmpPlr;
    }
    return tmpPlayer;
}

int ServerInfo::getEmptySlot()
{
    int slot{ -1 };
    for ( Player* plr : this->getPlayerVector() )
    {
        ++slot;
        if ( plr == nullptr )
            break;
    }
    return slot;
}

int ServerInfo::getSocketSlot(qintptr socketDescriptor)
{
    int slot{ -1 };
    for ( Player* plr : this->getPlayerVector() )
    {
        if ( plr != nullptr
          && plr->socketDescriptor() == socketDescriptor )
        {
            slot = plr->getSlotPos();
            break;
        }
    }
    return slot;
}

int ServerInfo::getQItemSlot(QStandardItem* index)
{
    int slot{ -1 };
    for ( Player* plr : this->getPlayerVector() )
    {
        if ( plr != nullptr
          && plr->getTableRow() == index )
        {
            slot = plr->getSlotPos();
            break;
        }
    }
    return slot;
}

void ServerInfo::sendPlayerSocketInfo()
{
    QString response{ ":SR@I" };
    QString filler{ "%1=%2," };

    QHostAddress ipAddr;

    for ( Player* plr : this->getPlayerVector() )
    {
        if ( plr != nullptr && plr->getHasSernum() )
        {
            ipAddr = QHostAddress( plr->peerAddress().toString() );
            response = response.append( filler.arg( Helper::intToStr( plr->getSernum_i(), static_cast<int>( IntBase::HEX ) ) )
                                              .arg( Helper::intToStr( qFromBigEndian( ipAddr.toIPv4Address() ) ^ 0xA9876543,
                                                                      static_cast<int>( IntBase::HEX ) ) ) );
        }
    }

    response = response.append( "\r\n" );

    qint64 bOut{ 0 };
    for ( Player* plr : this->getPlayerVector() )
    {
        if ( plr != nullptr )
        {
            bOut = plr->write( response.toLatin1(), response.length() );
            this->updateBytesOut( plr, bOut );
        }
    }
}

void ServerInfo::sendPlayerSocketPosition(Player* plr, const bool& forceIssue)
{
    if ( plr == nullptr )
        return;

    qint32 plrKey{ plrSlotMap.key( plr, 0 ) };
    bool isValidKey{ plrKey > 0 };
    if ( forceIssue
      && isValidKey )
    {
        //The player is already assigned a Slot however we are forcefully re-issuing a new Slot.
        //Remove this Player Object from the storage.
        plrSlotMap.remove( plr->getPktHeaderSlot() );
    }
    else if ( isValidKey )
    {
        //The player is already assigned a Slot, do nothing.
        return;
    }

    Player* lastPlr{ this->getLastPlayerInStorage( plr ) };
    Player* tmpPlr{ nullptr };

    qint32 slot{ 1 }; //Slot must be above 1 to be valid.
    for ( int i = 1; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = plrSlotMap.value( i, nullptr );
        if ( tmpPlr == nullptr )
        {
            slot = i;
            break;
        }
        tmpPlr = nullptr;
    }

    if ( lastPlr == nullptr )
        lastPlr = plr;

    plr->setPktHeaderSlot( slot );
    plrSlotMap.insert( slot, plr );
    QString slotResponse{ ":SR!%1%2%3\r\n" };
    slotResponse = slotResponse.arg( plr->getSernumHex_s() )
                               .arg( Helper::intToStr( slot, static_cast<int>( IntBase::HEX ), 2 ) )
                               .arg( lastPlr->getSernumHex_s() );

    qint64 bOut{ plr->write( slotResponse.toLatin1(), slotResponse.length() ) };
    this->updateBytesOut( plr, bOut );
}

void ServerInfo::sendServerRules(Player* plr)
{
    if ( plr == nullptr )
        return;

    QString serverName{ this->getServerName() };
    QString rules{ ":SR$%1\r\n" };
            rules = rules.arg( Settings::getRuleSet( serverName ) );

    qint64 bOut{ plr->write( rules.toLatin1(), rules.length() ) };
    this->updateBytesOut( plr, bOut );
}

void ServerInfo::sendServerGreeting(Player* plr)
{
    QString serverName{ this->getServerName() };
    QString greeting{ Settings::getSetting( SKeys::Setting, SSubKeys::MOTD, serverName ).toString() };
    if ( !Settings::getSetting( SKeys::Rules, SSubKeys::HasSvrPassword, serverName ).toString().isEmpty() )
    {
        greeting.append( " Password required: Please reply with (/login *PASS) or be disconnected." );
        plr->setSvrPwdRequested( true );
    }

    if ( !greeting.isEmpty() )
        this->sendMasterMessage( greeting, plr, false );

    if ( !Settings::getRuleSet( serverName ).isEmpty() )
        this->sendServerRules( plr );
}

void ServerInfo::sendMasterMessage(const QString& packet, Player* plr, const bool toAll)
{
    QString msg{ ":SR@M%1\r\n" };
            msg = msg.arg( packet );

    emit this->sendMasterMsgToPlayerSignal( plr, toAll, msg.toLatin1() );
}

void ServerInfo::sendMasterMessageToAdmins(const QString& message, Player* srcPlayer)
{
    QString msg{ ":SR@M%1\r\n" };
            msg = msg.arg( message );

    for ( Player* plr : this->getPlayerVector() )
    {
        if ( plr != nullptr
          && plr != srcPlayer
          && plr->getAdminPwdReceived() )
        {
            emit this->sendMasterMsgToPlayerSignal( plr, false, msg.toLatin1() );
        }
    }
}

qint64 ServerInfo::getUpTime() const
{
    return (( QDateTime::currentDateTime().toSecsSinceEpoch() - this->getInitializeDate() ) );
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
    if ( !Helper::cmpStrings( gameInfo, value ) )
    {
        gameInfo = value;
        emit this->serverWorldChangedSignal( gameInfo );
    }
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

    Settings::setSetting( value, SKeys::Setting, SSubKeys::GameName, this->getServerName() );
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

void ServerInfo::setMasterIP(const QString& value, const quint16& port)
{
    //Port should be set before the IP Address.
    this->setMasterPort( port );

    masterIP = value;
    this->setIsPublic( this->getIsPublic() );
}

bool ServerInfo::getIsPublic() const
{
    return isPublic;
}

void ServerInfo::setIsPublic(const bool& value)
{
    isPublic = value;
    Settings::setSetting( value, SKeys::Setting, SSubKeys::IsPublic, this->getServerName() );

    this->setMasterUDPResponse( false );
    this->setSentUDPCheckIn( false );

    if ( value )
    {
        if ( !this->getIsSetUp() )
            this->setupInfo();

        if (( !this->getUpnpPortAdded() && !this->getUseUPNP() )
          || ( this->getUpnpPortAdded() && this->getUseUPNP() ) )
        {
            this->startMasterCheckIn();
            emit this->initializeServerSignal();
            this->sendMasterInfo();
        }
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
    Settings::setSetting( value, SKeys::Setting, SSubKeys::UseUPNP, this->getServerName() );
    if ( useUPNP != value )
    {
        if ( !this->getIsSetUp() )
        {
            //Catch a possible race condition with a signal connection.
            QObject::connect( this, &ServerInfo::serverIsSetupSignal, this,
            [=,this]()
            {
                this->setupUPNP( value );
            } );
        }
        else
            this->setupUPNP( value );
    }

    if ( !value )
        upnpPortRefresh.stop();

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
    if ( !Helper::cmpStrings( serverID, value ) )
    {
        serverID = value;
        emit this->serverIDChangedSignal( serverID );
    }
}

quint32 ServerInfo::getPlayerCount() const
{
    return playerCount;
}

void ServerInfo::setPlayerCount(const quint32& value)
{
    if ( value <= 0 )
    {
        this->setGameInfo( Settings::getSetting( SKeys::Rules, SSubKeys::World, this->getServerName() ).toString() );
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
    Settings::setSetting( value, SKeys::Setting, SSubKeys::PortNumber, this->getServerName() );
}

QString ServerInfo::getPrivateIP() const
{
    return privateIP;
}

void ServerInfo::setPrivateIP(const QString& value)
{
    privateIP = value;
}

QString ServerInfo::getServerName() const
{
    return serverName;
}

void ServerInfo::setServerName(const QString& value)
{
    if ( !Helper::cmpStrings( serverName, value ) )
    {
        serverName = value;
        if ( this->getMasterUDPResponse() )
        {
            this->sendMasterInfo( true ); //Disconnect from the Master Mix.
            this->sendMasterInfo( false ); //Reconnect to the Master Mix using the new name.
        }
        emit this->serverNameChangedSignal( serverName );
        this->setGameInfo( "" );
    }
}

bool ServerInfo::getIsSetUp() const
{
    return isSetUp;
}

void ServerInfo::setIsSetUp(const bool& value)
{
    isSetUp = value;
    emit this->serverIsSetupSignal();
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
    {
        masterCheckIn.setInterval( MIN_MASTER_CHECK_IN_TIME );
        if ( this->getGameId() == Games::W97 )
            masterCheckIn.setInterval( MAX_MASTER_CHECKIN_TIME );
    }
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
    masterPing = static_cast<double>( this->getMasterPingRespTime() - this->getMasterPingSendTime() );

    this->setMasterPingAvg( masterPing );
    this->setMasterPingTrend( masterPing );
}

QString ServerInfo::getUsageString()
{
    return QString( "%1.%2.%3" )
            .arg( this->getUsageMins() )
            .arg( this->getUsageHours() )
            .arg( this->getUsageDays() );
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

PacketHandler* ServerInfo::getPktHandle() const
{
    return pktHandle;
}

void ServerInfo::setPktHandle(PacketHandler* value)
{
    pktHandle = value;
}

void ServerInfo::updateBytesOut(Player* plr, const qint64 bOut)
{
    if ( plr == nullptr )
        return;

    if ( bOut <= 0 )
        return;

    plr->setBytesOut( plr->getBytesOut() + static_cast<quint64>( bOut ) );
    plr->setPacketsOut( plr->getPacketsOut() + 1 );
    this->setBytesOut( this->getBytesOut() + static_cast<quint64>( bOut ) );

    return;
}

bool ServerInfo::getUpnpPortAdded() const
{
    return upnpPortAdded;
}

void ServerInfo::setUpnpPortAdded(bool value)
{
    upnpPortAdded = value;
}

qint64 ServerInfo::getMaxIdleTime()
{
    QVariant val{ Settings::getSetting( SKeys::Rules, SSubKeys::MaxIdle, this->getServerName() ) };
    qint64 maxIdle{ static_cast<qint64>( MAX_IDLE_TIME ) };
    if ( val.isValid() && val.toBool() )
    {
        maxIdle = val.toUInt() * static_cast<qint64>( MultiplyTime::Seconds )
                               * static_cast<qint64>( MultiplyTime::Miliseconds );
    }
    return maxIdle;
}

qint64 ServerInfo::getInitializeDate() const
{
    return initializeDate;
}

void ServerInfo::setInitializeDate(const qint64& value)
{
    initializeDate = value;
}

Server* ServerInfo::getTcpServer() const
{
    return tcpServer;
}

void ServerInfo::setTcpServer(Server* value)
{
    tcpServer = value;
}

//Slots
void ServerInfo::dataOutSizeSlot(const quint64& size)
{
    this->setBytesOut( this->getBytesOut() + size );
}

void ServerInfo::setMaxIdleTimeSlot()
{
    emit this->setMaxIdleTimeSignal( this->getMaxIdleTime() );
}

void ServerInfo::masterMixIPChangedSlot()
{
    qDebug() << "IP Changed";
    QString overrideIP{ Settings::getSetting( SKeys::Setting, SSubKeys::OverrideMasterIP ).toString() };
    if ( !overrideIP.isEmpty() )
    {
        int index{ static_cast<int>( overrideIP.indexOf( ":" ) ) };
        if ( index > 0 )
        {
            this->setMasterIP( overrideIP.left( index ), static_cast<quint16>( overrideIP.mid( index + 1 ).toInt() ) );
            QString msg{ "Loading Master Server Override [ %1:%2 ]." };
                    msg = msg.arg( this->getMasterIP() )
                             .arg( this->getMasterPort() );
            emit this->insertLogSignal( this->getServerName(), msg, LogTypes::MASTERMIX, true, true );
        }
    }
}

void ServerInfo::masterInfoSlot()
{

}

void ServerInfo::udpDataSlot(const QByteArray& data, const QHostAddress& ipAddr, const quint16& port)
{
    PacketHandler* pktHandle{ this->getPktHandle() };
    if ( pktHandle != nullptr )
        pktHandle->parseUDPPacket( data, ipAddr, port );
}

void ServerInfo::sendUserListSlot(const QHostAddress& addr, const quint16& port, const UserListResponse& type)
{
    this->sendUserList( addr, port, type );
}

void ServerInfo::sendServerInfoSlot(const QHostAddress& addr, const quint16& port)
{
    this->sendServerInfo( addr, port );
}

void ServerInfo::increaseServerPingSlot()
{
    this->setUserPings( this->getUserPings() + 1 );
}

void ServerInfo::upnpPortAddedSlot(const quint16& port, const QString& protocol)
{
    if ( port != this->getPrivatePort() )
        return;

    if ( Helper::cmpStrings( protocol, "UDP" ) )
    {
        this->startMasterCheckIn();
        emit this->initializeServerSignal();
        this->sendMasterInfo();

        this->setUpnpPortAdded( true );
        upnpPortRefresh.start();
    }
}
