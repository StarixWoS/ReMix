
//Class includes.
#include "server.hpp"

//ReMix Widget includes.
#include "widgets/settingswidget.hpp"
#include "widgets/userdelegate.hpp"
#include "widgets/ruleswidget.hpp"

//ReMix Threaded Includes.
#include "thread/mastermixthread.hpp"
#include "thread/udpthread.hpp"

//ReMix includes.
#include "packethandler.hpp"
#include "packetforge.hpp"
#include "upnp/upnp.hpp"
#include "settings.hpp"
#include "player.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "user.hpp"

//Qt Includes.
#include <QRegularExpression>
#include <QHostAddress>
#include <QtConcurrent>
#include <QUdpSocket>
#include <QtCore>
#include <QFile>

Server::Server(QWidget* parent)
    : QTcpServer(parent)
{
    //Ensure all possible User slots are fillable.
    this->setMaxPendingConnections( *Globals::MAX_PLAYERS );

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &Server::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );

    thread = new QThread();
    udpThread = UdpThread::getNewUdpThread( nullptr );
    udpThread->moveToThread( thread );

    //Connect signals from the UdpThread class to the slots within the Server class.
    QObject::connect( udpThread, &UdpThread::increaseServerPingsSignal, this, &Server::increaseServerPingSlot );
    QObject::connect( udpThread, &UdpThread::sendUserListSignal, this, &Server::sendUserListSlot );
    QObject::connect( udpThread, &UdpThread::dataOutSizeSignal, this, &Server::dataOutSizeSlot );
    QObject::connect( udpThread, &UdpThread::setBytesInSignal, this, &Server::setBytesInSignal );
    QObject::connect( udpThread, &UdpThread::recvMasterInfoResponseSignal, this, &Server::recvMasterInfoResponseSlot );
    QObject::connect( udpThread, &UdpThread::recvPlayerGameInfoSignal, this, &Server::recvPlayerGameInfoSlot );

    //Connect signals from the Server class to the slots within the UdpThread class.
    QObject::connect( this, &Server::serverWorldChangedSignal, udpThread, &UdpThread::serverWorldChangedSlot );
    QObject::connect( this, &Server::serverUsageChangedSignal, udpThread, &UdpThread::serverUsageChangedSlot );
    QObject::connect( this, &Server::serverNameChangedSignal, udpThread, &UdpThread::serverNameChangedSlot );
    QObject::connect( this, &Server::serverGameChangedSignal, udpThread, &UdpThread::serverGameChangedSlot );
    QObject::connect( this, &Server::serverIDChangedSignal, udpThread, &UdpThread::serverIDChangedSlot );
    QObject::connect( this, &Server::closeUdpSocketSignal, udpThread, &UdpThread::closeUdpSocketSlot, Qt::BlockingQueuedConnection );
    QObject::connect( this, &Server::sendUdpDataSignal, udpThread, &UdpThread::sendUdpDataSlot );
    QObject::connect( this, &Server::bindSocketSignal, udpThread, &UdpThread::bindSocketSlot );

    //Connect Signals from the MasterMixThread class to the slots within the Server class.
    QObject::connect( MasterMixThread::getInstance(), &MasterMixThread::masterMixInfoSignal, this, &Server::masterMixInfoSlot, Qt::UniqueConnection );
    QObject::connect( MasterMixThread::getInstance(), &MasterMixThread::masterMixInfoSyncSignal, this, &Server::masterMixInfoSyncSlot, Qt::UniqueConnection );
    QObject::connect( this, &Server::recvMasterInfoSignal, this, &Server::recvMasterInfoSlot );

    //Connect the MasterMixThread signals to the Server Slots.
    QObject::connect( SettingsWidget::getInstance(), &SettingsWidget::masterMixIPChangedSignal, this, &Server::masterMixIPChangedSlot );

    //Connect the Server Class Signals to the UPNP Class Slots.
    QObject::connect( this, &Server::upnpPortForwardSignal, UPNP::getInstance(), &UPNP::upnpPortForwardSlot );
    QObject::connect( UPNP::getInstance(), &UPNP::upnpPortAddedSignal, this, &Server::upnpPortAddedSlot );

    //Connect Timers to Slots.
    QObject::connect( &masterSerNumKeepAliveTimer, &QTimer::timeout, this, &Server::masterSerNumKeepAliveSlot );
    QObject::connect( &upnpPortRefresh, &QTimer::timeout, this, &Server::upnpPortRefreshTimeOutSlot );
    QObject::connect( &masterCheckIn, &QTimer::timeout, this, &Server::masterCheckInTimeOutSlot );
    QObject::connect( &usageUpdate, &QTimer::timeout, this, &Server::updateUsageTimeOutSlot );
    QObject::connect( &upnpPortTimeOut, &QTimer::timeout, this, &Server::upnpTimeOutSlot );
    QObject::connect( &masterTimeOut, &QTimer::timeout, this, &Server::masterTimeOutSlot );
    QObject::connect( &upTimer, &QTimer::timeout, this, &Server::upTimerTimeOutSlot );

    //Start Timers.
    usageUpdate.start( *Globals::SERVER_USAGE_UPDATE );
    upTimer.start( *Globals::UI_UPDATE_TIME );

    //Set KeepAlive timer.
    masterSerNumKeepAliveTimer.setInterval( *Globals::MASTER_SERNUM_KEEPALIVE );

    //Set Conditional Timers with an initial interval.
    masterTimeOut.setInterval( *Globals::MAX_MASTER_RESPONSE_TIME );
    upnpPortRefresh.setInterval( *Globals::UPNP_TIME_OUT_MS );

    //Set Timers as Single-use.
    upnpPortTimeOut.setSingleShot( true );
    masterTimeOut.setSingleShot( true );

    this->setInitializeDate( QDateTime::currentDateTime().currentSecsSinceEpoch() );
    udpThread->start();
    thread->start();
}

Server::~Server()
{
    //emit this->upnpPortForwardSignal( this->getPrivateIP(), this->getPrivatePort(), false );

    thread->disconnect();
    thread->exit();
    thread->wait();    //Properly await thread exit.

    udpThread->disconnect();
    udpThread->exit();
    udpThread->wait();    //Properly await thread exit.

    //Explicitly delete Thread objects now.
    delete thread;
    delete udpThread;

    //thread->deleteLater();
    //udpThread->deleteLater();

    thread = nullptr;
    udpThread = nullptr;

    upTimer.disconnect();
    upnpPortRefresh.disconnect();

    QString msg{ "Server( 0x%1 ) deconstructed." };
            msg = msg.arg( Helper::intToStr( reinterpret_cast<quintptr>( this ), IntBase::HEX, IntFills::QuadWord ) );

    emit this->insertLogSignal( this->getServerName(), msg, LKeys::MiscLog, true, true );

    this->disconnect();
}

void Server::customDeconstruct(Server* svr)
{
    if ( svr != nullptr )
    {
        svr->deleteAllPlayers();
        svr->deleteLater();
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    emit this->plrConnectedSignal( socketDescriptor );
}

void Server::setupInfo(const QString& interfaceIP)
{
    QHostAddress addr;
    if ( interfaceIP.isEmpty() )
        addr = Helper::getPrivateIP();
    else
        addr = QHostAddress( interfaceIP );

    emit this->bindSocketSignal( addr, this->getPrivatePort() );
    if ( !this->getIsSetUp() )
    {
        this->setPrivateIP( addr.toString() );
        this->setIsSetUp( true );
    }
}

void Server::setupUPNP(const bool& enable)
{
    if ( !enable )
        this->setUpnpPortAdded( false );
    else
        upnpPortTimeOut.start( *Globals::UPNP_RESPONSE_TIME_OUT );

    emit this->upnpPortForwardSignal( this->getPrivateIP(), this->getPrivatePort(), enable );
}

void Server::sendUserList(const QHostAddress& addr, const quint16& port, const UserListResponse& type)
{
    if ( !this->getIsPublic() )
        return;

    if ( addr.isNull() )
        return;

    QString response{ "Q" };
    if ( type == UserListResponse::R_Response )
        response = "R";

    QString filler_Q{ "%1," };
    QString filler_R{ "%1|%2," };

    //Note: The 'R' Response sent by the Syn-Real Mix Server
    //Sends the User's IP address within field(%2) 2; we are sending the User's
    //position within the players[] array instead to prevent IP leakage.

    for ( QSharedPointer<Player> tmpPlayer : this->getPlayerVector() )
    {
        if ( response.length() > 800 )
            break;

        if ( tmpPlayer != nullptr
          && tmpPlayer->getSernum_i() != 0 )
        {
            //Don't show Invisible Administrators on the User List.
            if ( tmpPlayer->getIsVisible() )
            {
                if ( type == UserListResponse::Q_Response ) //Standard 'Q' Response.
                {
                    response += filler_Q.arg( Helper::intToStr( tmpPlayer->getSernum_i(), IntBase::HEX, IntFills::QuadWord ) );
                }
                else if ( type == UserListResponse::R_Response ) //Non-Standard 'R' Response
                {
                    response += filler_R.arg( Helper::intToStr( tmpPlayer->getSernum_i(), IntBase::HEX ) )
                                        .arg( Helper::intToStr( this->getPlayerSlot( tmpPlayer ), IntBase::HEX, IntFills::QuadWord ) );
                }
            }
        }
    }

    emit this->sendUdpDataSignal( addr, port, response );

    QString msg{ "Sending User List to [ %1:%2 ]; %3" };
            msg = msg.arg( addr.toString() )
                     .arg( port )
                     .arg( response );
    emit this->insertLogSignal( this->getServerName(), msg, LKeys::PingLog, true, true );
}

void Server::sendMasterInfo(const bool& disconnect)
{
    if ( !this->getIsPublic()
      && !disconnect )
    {
        return;
    }

    QHostAddress addr{ this->getMasterIP() };
    QString response{ "X" };

    if ( !disconnect )
    {
        if ( this->getIsSetUp() )
        {
            response = "!version=%1,nump=%2,gameid=%3,game=%4,host=%5,id=%6,port=%7,info=%8,name=%9";
            response = response.arg( this->getVersionID() )
                               .arg( this->getPlayerCount() )
                               .arg( *this->getGameId() )
                               .arg( this->getGameName() )
                               .arg( this->getHostInfo().localHostName() )
                               .arg( this->getServerID() )
                               .arg( this->getPrivatePort() )
                               .arg( this->getGameWorld() )
                               .arg( this->getServerName() );
        }
    }

    if ( !response.isEmpty()
      && this->getIsSetUp()
      && !addr.isNull() )
    {
        quint16 port{ this->getMasterPort() };
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

        emit this->sendUdpDataSignal( addr, port, response );
        emit this->insertLogSignal( this->getServerName(), msg, LKeys::MasterMixLog, true, true );
    }
    else
        this->setSentUDPCheckIn( false );

    if ( !disconnect )
    {
        masterCheckIn.setInterval( *Globals::MAX_MASTER_CHECKIN_TIME );
        this->setMasterTimedOut( false );
    }
}

QSharedPointer<Player> Server::createPlayer(const qintptr& socketDescriptor, QSharedPointer<Server> server)
{
    int slot{ this->getEmptySlot() };
    if ( slot >= 0 && slot < this->getMaxPlayerCount() )
    {
        QSharedPointer<Player> plr{ Player::createPlayer( socketDescriptor, server ) };
        if ( plr != nullptr )
        {
            players.insert( slot, plr );
            plr->refreshAFKTimersSlot( this->getMaxAFKTime() );

            this->setPlayerCount( this->getPlayerCount() + 1 );

            QObject::connect( this, &Server::refreshAFKTimersSignal, plr.get(), &Player::refreshAFKTimersSlot );
            QObject::connect( this, &Server::sendMasterMsgToPlayerSignal, plr.get(), &Player::sendMasterMsgToPlayerSlot );
            QObject::connect( this, &Server::connectionTimeUpdateSignal, plr.get(), &Player::connectionTimeUpdateSlot );
            QObject::connect( plr.get(), &Player::ipDCIncreaseSignal, this, &Server::ipDCIncreaseSlot );
            QObject::connect( plr.get(), &Player::setVisibleStateSignal, this, &Server::setVisibleStateSlot );
            QObject::connect( User::getInstance(), &User::setAdminRankSignal, plr.get(), &Player::setAdminRankSlot );
            return plr;
        }
    }
    return nullptr;
}

QSharedPointer<Player> Server::getLastPlayerInStorage(const QSharedPointer<Player> plr)
{
    QSharedPointer<Player> tmpPlayer{ nullptr };
    for ( QSharedPointer<Player> tmpPlr : this->getPlayerVector() )
    {
        if ( tmpPlr == nullptr )
            break;

        if ( tmpPlr != plr )
            tmpPlayer = tmpPlr;
    }
    return tmpPlayer;
}

int Server::getEmptySlot()
{
    int slot{ -1 };
    for ( QSharedPointer<Player> plr : this->getPlayerVector() )
    {
        ++slot;
        if ( plr == nullptr )
            break;
    }
    return slot;
}

QSharedPointer<Player> Server::getPlayer(const int& slot)
{
    //Return the Player object within the position [ slot ] of the Players vector.
    //Do not return Player objects which are 'valid' but in a disconnected state.

    QSharedPointer<Player> plr{ nullptr };
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

QSharedPointer<Player> Server::getPlayer(const qintptr& socketDescriptor)
{
    //Return the Player object within the position [ slot ] of the Players vector.
    QSharedPointer<Player> plr{ nullptr };
    int slot{ this->getSocketSlot( socketDescriptor ) };
    if ( slot >= 0 )
        plr = this->getPlayerVector().at( slot );

    return plr;
}

QSharedPointer<Player> Server::getPlayer(const QString& hexSerNum)
{
    for ( QSharedPointer<Player> plr : this->getPlayerVector() )
    {
        if ( plr != nullptr
          && Helper::cmpStrings( plr->getSernumHex_s(), hexSerNum ) )
        {
            return plr;
        }
    }
    return nullptr;
}

qint32 Server::getPlayerSlot(const QSharedPointer<Player> plr)
{
    return players.indexOf( plr );
}

int Server::getSocketSlot(const qintptr& socketDescriptor)
{
    int slot{ -1 };
    for ( QSharedPointer<Player> plr : this->getPlayerVector() )
    {
        if ( plr != nullptr )
        {
            if ( plr->socketDescriptor() == socketDescriptor )
            {
                slot = this->getPlayerSlot( plr );
                break;
            }
        }
    }
    return slot;
}

void Server::deletePlayer(QSharedPointer<Player> plr, const bool& all, const bool& timedOut)
{
    if ( plr == nullptr )
        return;

    QString timeOut{ "" };
    if ( timedOut == true )
        timeOut = " Timed Out";

    QString bytesUnit{ "" };
    QString bytesSec{ "" };
    Helper::sanitizeToFriendlyUnits( plr->getBytesIn(), bytesSec, bytesUnit );

    QString logMsg{ "Client%1: [ %2 ] was on for %3 minutes and sent %4 %5 in %6 packets, [ %7 ]" };
            logMsg = logMsg.arg( timeOut )
                           .arg( plr->getIPAddress() )
                           .arg( Helper::getTimeIntFormat( plr->getConnTime(), TimeFormat::Minutes ) )
                           .arg( bytesSec )
                           .arg( bytesUnit )
                           .arg( plr->getPacketsIn() )
                           .arg( plr->getBioData() );

    //Do not retain the Players BIO Data in storage.
    Settings::removeBioHash( plr->peerAddress() );

    plrSlotMap.remove( plr->getPktHeaderSlot() );
    players.removeOne( plr );

    if ( !all )
    {
        qint32 count{ this->getPlayerCount() };
        if ( plr->getIsVisible() )
            this->setPlayerCount( qMax( count - 1, static_cast<qint32>( 0 ) ) );
        else
            this->setPlayerCount( qMax( count, static_cast<qint32>( 0 ) ) );
    }

    plr->clearThisPlayer();
    plr.clear();

    emit this->insertLogSignal( this->getServerName(), logMsg, LKeys::ClientLog, true, true );
}

void Server::deleteAllPlayers()
{
    for ( QSharedPointer<Player> plr : this->getPlayerVector() )
    {
        this->deletePlayer( plr, true, false );
    }
}

void Server::sendPlayerSocketInfo()
{
    QString response{ ":SR@I" };
    QString filler{ "%1=%2," };

    QHostAddress ipAddr;

    for ( QSharedPointer<Player> plr : this->getPlayerVector() )
    {
        if ( plr != nullptr && plr->getHasSerNum() )
        {
            ipAddr = QHostAddress( plr->getIPAddress() );
            response = response.append( filler.arg( Helper::intToStr( plr->getSernum_i(), IntBase::HEX ) )
                                              .arg( Helper::intToStr( qFromBigEndian( ipAddr.toIPv4Address() ) ^ 0xA9876543, IntBase::HEX ) ) );
        }
    }

    response = response.append( "\r\n" );

    qint64 bOut{ 0 };
    for ( QSharedPointer<Player> plr : this->getPlayerVector() )
    {
        if ( plr != nullptr )
        {
            bOut = plr->write( response.toLatin1(), response.length() );
            this->updateBytesOut( plr, bOut );
        }
    }
}

void Server::sendPlayerSocketPosition(QSharedPointer<Player> plr, const bool& forceIssue)
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

    QSharedPointer<Player> lastPlr{ this->getLastPlayerInStorage( plr ) };
    qint32 slot{ 1 }; //Slot must be above 1 to be valid.
    if ( this->getGameId() == Games::W97 )  //Warpath must start at slot 0.
        slot = 0;

    for ( int i = slot; i < this->getMaxPlayerCount(); ++i )
    {
        const QSharedPointer<Player> tmpPlr = plrSlotMap.value( i, nullptr );
        if ( tmpPlr == nullptr )
        {
            slot = i;
            break;
        }
    }

    if ( lastPlr == nullptr )
        lastPlr = plr;

    plr->setPktHeaderSlot( slot );
    plrSlotMap.insert( slot, plr );
    QString slotResponse{ ":SR!%1%2%3\r\n" };
            slotResponse = slotResponse.arg( plr->getSernumHex_s() )
                                       .arg( Helper::intToStr( slot, IntBase::HEX, IntFills::Byte ) )
                                       .arg( lastPlr->getSernumHex_s() );

    qint64 bOut{ plr->write( slotResponse.toLatin1(), slotResponse.length() ) };
    this->updateBytesOut( plr, bOut );
}

void Server::sendServerRules(QSharedPointer<Player> plr)
{
    if ( plr == nullptr )
        return;

    QString serverName{ this->getServerName() };
    QString rules{ ":SR$%1\r\n" };
            rules = rules.arg( Settings::getRuleSet( serverName ) );

    qint64 bOut{ plr->write( rules.toLatin1(), rules.length() ) };
    this->updateBytesOut( plr, bOut );
}

void Server::sendServerGreeting(QSharedPointer<Player> plr)
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

void Server::sendMasterMessage(const QString& packet, QSharedPointer<Player> plr, const bool toAll)
{
    QString msg{ ":SR@M%1\r\n" };
            msg = msg.arg( packet );

    emit this->sendMasterMsgToPlayerSignal( plr, toAll, msg.toLatin1() );
}

void Server::sendMasterMessageToAdmins(const QString& message)
{
    QString msg{ ":SR@M%1\r\n" };
            msg = msg.arg( message );

    for ( QSharedPointer<Player> plr : this->getPlayerVector() )
    {
        if ( plr != nullptr
          && plr->getAdminPwdReceived() )
        {
            emit this->sendMasterMsgToPlayerSignal( plr, false, msg.toLatin1() );
        }
    }
}

void Server::startMasterSerNumKeepAliveTimer()
{
    if ( !masterSerNumKeepAliveTimer.isActive() )
        masterSerNumKeepAliveTimer.start();
}

void Server::masterSerNumKeepAliveSlot()
{
    static const QString packet{ ":;o0%1D" };
    QString pkt{ packet.arg( Helper::serNumToHexStr( Helper::intToStr( *ReMixSerNum::SerNum, IntBase::HEX, IntFills::DblWord ) ) ) };
    emit this->sendMasterMsgToPlayerSignal( nullptr, true, PacketForge::getInstance()->encryptPacket( pkt.toLatin1(), 0, this->getGameId() ) );

    this->startMasterSerNumKeepAliveTimer();
}

void Server::sendPingToPlayer(QSharedPointer<Player> plr)
{
    static const QString pingPacket{ ":;o1%1D" };
    if ( plr != nullptr
      && plr->getIsIncarnated() )
    {
        const QString pkt{ pingPacket.arg( plr->getSernumHex_s() ) };
        emit this->sendMasterMsgToPlayerSignal( plr, false, PacketForge::getInstance()->encryptPacket( pkt.toLatin1(), 0, this->getGameId() ) );

        plr->setPlrPingTime( QDateTime::currentMSecsSinceEpoch() );
    }
}

qint64 Server::getUpTime() const
{
    return (( QDateTime::currentDateTime().toSecsSinceEpoch() - this->getInitializeDate() ) );
}

QTimer* Server::getUpTimer()
{
    return &upTimer;
}

QString Server::getGameWorld() const
{
    return gameWorld;
}

void Server::setGameWorld(const QString& value)
{
    if ( !Helper::cmpStrings( gameWorld, value ) )
    {
        gameWorld = value;
        emit this->serverWorldChangedSignal( gameWorld );
    }
}

Games Server::getGameId() const
{
    return gameId;
}

void Server::setGameId(const Games& game)
{
    emit this->serverGameChangedSignal( game );
    gameId = game;
}

QString Server::getGameName() const
{
    return gameName;
}

void Server::setGameName(const QString& value)
{
    gameName = value;
    Settings::setSetting( value, SKeys::Setting, SSubKeys::GameName, this->getServerName() );
}

QHostInfo Server::getHostInfo() const
{
    return hostInfo;
}

int Server::getVersionID() const
{
    return versionID;
}

void Server::setVersionID(const int& value)
{
    versionID = value;
}

quint16 Server::getMasterPort() const
{
    return masterPort;
}

void Server::setMasterPort(const quint16& value)
{
    masterPort = value;
}

QString Server::getMasterIP() const
{
    return masterIP;
}

void Server::setMasterIP(const QString& value, const quint16& port)
{
    if ( !value.isEmpty() )
    {
        //Prevent newly created Server instances from forcing all servers to re-ping the MasterMix.
        //Only when the MasterIP is being changed should we force the Server Instance to ping.
        if ( !Helper::cmpStrings( masterIP, value ) )
        {
            //Port should be set before the IP Address.
            this->setMasterPort( port );

            masterIP = value;
            emit this->recvMasterInfoSignal();
            this->setMasterInfoRecv( true );
        }
        else
        {
            //Info did not change.
            //The CheckinTimer was halted, restart it.
            if ( !masterCheckIn.isActive() )
                masterCheckIn.start( *Globals::MIN_MASTER_STARTUP_TIME );
        }
    }
}

bool Server::getMasterInfoRecv() const
{
    return masterInfoRecv;
}

void Server::setMasterInfoRecv(bool infoRecv)
{
    masterInfoRecv = infoRecv;
}

bool Server::getIsPublic() const
{
    return isPublic;
}

void Server::setIsPublic(const bool& value, const QString& netInterface)
{
    isPublic = value;
    Settings::setSetting( value, SKeys::Setting, SSubKeys::IsPublic, this->getServerName() );

    this->setMasterUDPResponse( false );
    this->setSentUDPCheckIn( false );

    if ( value )
    {
        if ( !this->getIsSetUp() )
            this->setupInfo( netInterface );

        if (( !this->getUpnpPortAdded() && !this->getUseUPNP() )
          || ( this->getUpnpPortAdded() && this->getUseUPNP() )
          || ( !this->getUpnpPortAdded() && this->getUseUPNP() ) )
        {
            if ( !masterCheckIn.isActive() )
                this->startMasterCheckIn();

            emit this->initializeServerSignal();
        }
    }
    else
    {
        //Disconnect from the Master Server if applicable.
        this->stopMasterCheckIn();
        this->sendMasterInfo( true );
        this->setUpnpPortAdded( false );
    }

    Settings::setSetting( isPublic, SKeys::Setting, SSubKeys::IsRunning, serverName );
}

bool Server::getUseUPNP() const
{
    return useUPNP;
}

void Server::setUseUPNP(const bool& value)
{
    //Tell the server to use a UPNP Port Forward.
    Settings::setSetting( value, SKeys::Setting, SSubKeys::UseUPNP, this->getServerName() );
    if ( useUPNP != value )
    {
        if ( !this->getIsSetUp() )
        {
            //Catch a possible race condition with a signal connection.
            QObject::connect( this, &Server::serverIsSetupSignal, this,
            [=, this]()
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

QString Server::getServerID() const
{
    return serverID;
}

void Server::setServerID(const QString& value)
{
    if ( !Helper::cmpStrings( serverID, value ) )
    {
        serverID = value;
        emit this->serverIDChangedSignal( serverID );
    }
}

qint32 Server::getMaxPlayerCount() const
{
    return maxPlayerCount;
}

void Server::setMaxPlayerCount(const qint32& value)
{
    maxPlayerCount = value;
}

qint32 Server::getPlayerCount() const
{
    return playerCount;
}

void Server::setPlayerCount(const qint32& value)
{
    if ( value == 0 )
    {
        if ( this->getGameId() == Games::WoS )
            this->setGameWorld( Settings::getSetting( SKeys::Rules, SSubKeys::WorldName, this->getServerName() ).toString() );
        else if ( this->getGameId() == Games::ToY )
            this->setGameWorld( Settings::getSetting( SKeys::Rules, SSubKeys::ToYName, this->getServerName() ).toString() );

        playerCount = 0;
    }
    else
        playerCount = value;

    this->setPeakPlayerCount( playerCount );
    this->sendMasterInfo();
}

qint32 Server::getPeakPlayerCount() const
{
    return peakPlayerCount;
}

void Server::setPeakPlayerCount(const qint32& value)
{
    if ( value > peakPlayerCount )
        peakPlayerCount = value;
}

qint32 Server::getQuarantinedPlayerCount() const
{
    qint32 count{ 0 };
    for ( QSharedPointer<Player> plr : this->getPlayerVector() )
    {
        if ( plr != nullptr
          && plr->getIsQuarantined() )
        {
            ++count;
        }
    }
    return count;
}

QString Server::getQuarantinedPlayerList() const
{
    QString list{ "" };
    QString filler{ "%1," };

    for ( QSharedPointer<Player> plr : this->getPlayerVector() )
    {
        if ( plr != nullptr
          && plr->getIsQuarantined() )
        {
            list += filler.arg( plr->getSernum_s() );
        }

        if ( filler.length() > 800 )
            break;
    }
    return list;
}

qint32 Server::getMutedPlayerCount() const
{
    qint32 count{ 0 };
    for ( QSharedPointer<Player> plr : this->getPlayerVector() )
    {
        if ( plr != nullptr
          && plr->getIsMuted() )
        {
            ++count;
        }
    }
    return count;
}

QString Server::getMutedPlayerList() const
{
    QString list{ "" };
    QString filler{ "%1," };

    for ( QSharedPointer<Player> plr : this->getPlayerVector() )
    {
        if ( plr != nullptr
          && plr->getIsMuted() )
        {
            list += filler.arg( plr->getSernum_s() );
        }

        if ( filler.length() > 800 )
            break;
    }
    return list;
}

quint16 Server::getPublicPort() const
{
    return publicPort;
}

void Server::setPublicPort(const quint16& value)
{
    publicPort = value;
}

QString Server::getPublicIP() const
{
    return publicIP;
}

void Server::setPublicIP(const QString& value)
{
    publicIP = value;
}

quint16 Server::getPrivatePort() const
{
    return privatePort;
}

void Server::setPrivatePort(const quint16& value)
{
    privatePort = value;
    Settings::setSetting( value, SKeys::Setting, SSubKeys::PortNumber, this->getServerName() );
}

QString Server::getPrivateIP() const
{
    return privateIP;
}

void Server::setPrivateIP(const QString& value)
{
    privateIP = value;
    Settings::setSetting( value, SKeys::Setting, SSubKeys::NetInterface, this->getServerName() );
}

QString Server::getServerName() const
{
    return serverName;
}

void Server::setServerName(const QString& value)
{
    if ( !Helper::cmpStrings( serverName, value ) )
    {
        serverName = value;
        if ( this->getMasterUDPResponse()
          && this->getIsPublic() )
        {
            this->sendMasterInfo( true ); //Disconnect from the Master Mix.
            this->sendMasterInfo( false ); //Reconnect to the Master Mix using the new name.
        }
        emit this->serverNameChangedSignal( serverName );
        this->setGameWorld( "" );
    }
}

bool Server::getIsSetUp() const
{
    return isSetUp;
}

void Server::setIsSetUp(const bool& value)
{
    isSetUp = value;
    if ( isSetUp )
        emit this->serverIsSetupSignal();
}

quint32 Server::getUserCalls() const
{
    return userCalls;
}

void Server::setUserCalls(const quint32& value)
{
    userCalls = value;
}

quint32 Server::getUserPings() const
{
    return userPings;
}

void Server::setUserPings(const quint32& value)
{
    userPings = value;
}

quint32 Server::getDupDc() const
{
    return dupDc;
}

void Server::setDupDc(const quint32& value)
{
    dupDc = value;
}

quint32 Server::getPktDc() const
{
    return pktDc;
}

void Server::setPktDc(const quint32& value)
{
    pktDc = value;
}

quint32 Server::getIpDc() const
{
    return ipDc;
}

void Server::setIpDc(const quint32& value)
{
    ipDc = value;
}

quint64 Server::getBytesIn() const
{
    return bytesIn;
}

void Server::setBytesIn(const quint64& value)
{
    bytesIn = value;
}

quint64 Server::getBytesOut() const
{
    return bytesOut;
}

void Server::setBytesOut(const quint64& value)
{
    bytesOut = value;
}

bool Server::getSentUDPCheckin() const
{
    return sentUDPCheckin;
}

void Server::setSentUDPCheckIn(const bool& value)
{
    sentUDPCheckin = value;
}

bool Server::getMasterUDPResponse() const
{
    return masterUDPResponse;
}

void Server::setMasterUDPResponse(const bool& value)
{
    masterUDPResponse = value;
    if ( masterUDPResponse )
    {
        this->setMasterTimedOut( false );
        masterCheckIn.setInterval( *Globals::MAX_MASTER_CHECKIN_TIME );
    }
    else if ( this->getMasterTimedOut() )
    {
        masterCheckIn.setInterval( *Globals::MIN_MASTER_CHECK_IN_TIME );
        if ( this->getGameId() == Games::W97 )
            masterCheckIn.setInterval( *Globals::MAX_MASTER_CHECKIN_TIME );
    }
}

bool Server::getMasterTimedOut() const
{
    return masterTimedOut;
}

void Server::setMasterTimedOut(const bool& value)
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

void Server::startMasterCheckIn()
{
    //Every 500 Milliseconds we will attempt to Obtain Master Info.
    //This will be set to 300000 (5-Minutes) once Master info is obtained.

    masterCheckIn.setInterval( *Globals::MIN_MASTER_STARTUP_TIME );
    masterCheckIn.start();
}

void Server::stopMasterCheckIn()
{
    masterCheckIn.stop();
}

qint64 Server::getMasterPingSendTime() const
{
    return masterPingSendTime;
}

void Server::setMasterPingSendTime(const qint64& value)
{
    masterPingSendTime = value;
}

qint64 Server::getMasterPingRespTime() const
{
    return masterPingRespTime;
}

void Server::setMasterPingRespTime(const qint64& value)
{
    masterPingRespTime = value;
    //Store the Master Server's Response Count.
    this->setMasterPingCount( this->getMasterPingCount() + 1 );
    this->setMasterPing();
}

double Server::getMasterPingTrend() const
{
    return masterPingTrend;
}

void Server::setMasterPingTrend(const double& value)
{
    masterPingTrend = masterPingTrend * 0.9 + value * 0.1;
}

double Server::getMasterPingAvg() const
{
    if ( masterPingAvg > 0 )
        return masterPingAvg / this->getMasterPingCount();

    return masterPingAvg;
}

void Server::setMasterPingAvg(const double& value)
{
    masterPingAvg += value;
}

qint32 Server::getMasterPingFailCount() const
{
    return masterPingFailCount;
}

void Server::setMasterPingFailCount(const qint32& value)
{
    masterPingFailCount = value;
}

qint32 Server::getMasterPingCount() const
{
    return masterPingCount;
}

void Server::setMasterPingCount(const qint32& value)
{
    masterPingCount = value;
}

double Server::getMasterPing() const
{
    return masterPing;
}

void Server::setMasterPing()
{
    masterPing = static_cast<double>( this->getMasterPingRespTime() - this->getMasterPingSendTime() );

    this->setMasterPingAvg( masterPing );
    this->setMasterPingTrend( masterPing );
}

QString Server::getUsageString()
{
    static const QString usage{ "%1.%2.%3" };
    return usage.arg( this->getUsageMins() )
                .arg( this->getUsageHours() )
                .arg( this->getUsageDays() );
}

qint32 Server::getUsageHours() const
{
    return usageHours;
}

qint32 Server::getUsageDays() const
{
    return usageDays;
}

qint32 Server::getUsageMins() const
{
    return usageMins;
}

void Server::updateBytesOut(QSharedPointer<Player> plr, const qint64 bOut)
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

bool Server::getUpnpPortAdded() const
{
    return upnpPortAdded;
}

void Server::setUpnpPortAdded(bool value)
{
    upnpPortAdded = value;
}

bool Server::getUpnpTimedOut() const
{
    return upnpTimedOut;
}

void Server::setUpnpTimedOut(bool newUpnpTimedOut)
{
    upnpTimedOut = newUpnpTimedOut;
}

qint64 Server::getMaxAFKTime()
{
    QVariant val{ Settings::getSetting( SKeys::Rules, SSubKeys::MaxAFK, this->getServerName() ) };
    qint64 maxAFK{ *Globals::MAX_AFK_TIME };
    if ( val.isValid()
      && val.toBool() )
    {
        maxAFK = val.toLongLong() * *TimeMultiply::Seconds
                                  * *TimeMultiply::Milliseconds;
    }
    return maxAFK;
}

qint64 Server::getInitializeDate() const
{
    return initializeDate;
}

void Server::setInitializeDate(const qint64& value)
{
    initializeDate = value;
}

//Slots
void Server::dataOutSizeSlot(const quint64& size)
{
    this->setBytesOut( this->getBytesOut() + size );
}

void Server::refreshAFKTimersSlot()
{
    emit this->refreshAFKTimersSignal( this->getMaxAFKTime() );
}

void Server::masterMixIPChangedSlot()
{
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
            emit this->insertLogSignal( this->getServerName(), msg, LKeys::MasterMixLog, true, true );
        }
    }
}

void Server::masterMixInfoSlot(const Games& game, const QString& ip, const quint16& port)
{
    if ( game == this->getGameId() )
        this->setMasterIP( ip, port );
}

void Server::setBytesInSignal(const quint64& bytes)
{
    this->setBytesIn( this->getBytesIn() + bytes );
}

void Server::recvMasterInfoResponseSlot(const QString& masterIP, const quint16& masterPort, const QString& userIP, const quint16& userPort)
{
    bool isMaster{ false };
    //Prevent Spoofing a MasterMix response.
    if ( Helper::cmpStrings( this->getMasterIP(), masterIP ) )
        isMaster = true;

    if ( isMaster )
    {
        this->setPublicIP( userIP );
        this->setPublicPort( userPort );

        //Store the Master Server's Response Time.
        this->setMasterPingRespTime( QDateTime::currentMSecsSinceEpoch() );

        //We've obtained a Master response.
        this->setMasterUDPResponse( true );
        QString msg{ "Got Response from Master [ %1:%2 ]; it thinks we are [ %3:%4 ]. ( Ping: %5 ms, Avg: %6 ms, Trend: %7 ms, Dropped: %8 )" };
                msg = msg.arg( masterIP )
                         .arg( masterPort )
                         .arg( this->getPublicIP() )
                         .arg( this->getPublicPort() )
                         .arg( this->getMasterPing() )
                         .arg( this->getMasterPingAvg() )
                         .arg( this->getMasterPingTrend() )
                         .arg( this->getMasterPingFailCount() );

        emit this->insertLogSignal( this->getServerName(), msg, LKeys::MasterMixLog, true, true );
    }
}

void Server::recvPlayerGameInfoSlot(const QString& info, const QString& ip)
{
    //Check if the IP Address is a properly connected User. Or at least is in the User list...
    bool connected{ false };

    for ( QSharedPointer<Player> tmpPlayer : this->getPlayerVector() )
    {
        if ( tmpPlayer != nullptr )
        {
            if ( Helper::cmpStrings( tmpPlayer->getIPAddress(), ip ) )
            {
                connected = true;
                break;
            }
        }
        connected = false;
    }

    if ( connected )
    {
        QString svrInfo{ this->getGameWorld() };
        QString usrInfo{ info.mid( 1 ) };
        if ( svrInfo.isEmpty() && !usrInfo.isEmpty() )
        {
            //Check if the World String starts with "world=" And remove the substring.
            if ( Helper::strStartsWithStr( usrInfo, "world=" ) )
                usrInfo = Helper::getStrStr( usrInfo, "world", "=", "=" );

            if ( usrInfo.contains( '\u0000' ) )
                usrInfo = usrInfo.remove( '\u0000' );

            //Enforce a 256 character limit on GameNames.
            if ( usrInfo.length() > *Globals::MAX_GAME_NAME_LENGTH )
                this->setGameWorld( usrInfo.left( *Globals::MAX_GAME_NAME_LENGTH ).toLatin1() ); //Truncate the User's GameInfo String.
            else
                this->setGameWorld( usrInfo.toLatin1() ); //Length was less than 256, set without issue.
        }
    }
}

void Server::gameInfoChangedSlot(const QString& info)
{
    this->setGameWorld( info );
}

void Server::ipDCIncreaseSlot(const DCTypes& type)
{
    switch ( type )
    {
        case DCTypes::IPDC:
            {
                this->setIpDc( this->getIpDc() + 1 );
            }
        break;
        case DCTypes::DupDC:
            {
                this->setDupDc( this->getDupDc() + 1 );
            }
        break;
        case DCTypes::PktDC:
            {
                this->setPktDc( this->getPktDc() + 1 );
            }
        break;
        case DCTypes::Invalid:
        default:
        break;
    }
}

void Server::setVisibleStateSlot(const bool& state)
{
    qint32 count{ this->getPlayerCount() };
    if ( count >= 0 )
    {
        if ( state )
            count++;
        else
            count--;
    }
    else
        count = 0;

    //Ensure Player Count cannot be negative.
    this->setPlayerCount( qMax( count, static_cast<qint32>( 0 ) ) );
}

void Server::recvMasterInfoSlot()
{
    this->startMasterCheckIn();
}

void Server::setMaxPlayersSlot(const qint32& maxPlayers)
{
    //Ensure all possible User slots are fillable.
    this->setMaxPendingConnections( maxPlayers );
    players.resize( maxPlayers );

    this->setMaxPlayerCount( maxPlayers );
}

void Server::masterMixInfoSyncSlot()
{
    //Prevent servers from checking in to the MasterMix while an info sync is in progress.
    if ( masterCheckIn.isActive() )
        masterCheckIn.stop();
}

void Server::sendUserListSlot(const QHostAddress& addr, const quint16& port, const UserListResponse& type)
{
    this->sendUserList( addr, port, type );
}

void Server::increaseServerPingSlot()
{
    this->setUserPings( this->getUserPings() + 1 );
}

void Server::upnpPortAddedSlot(const quint16& port, const QString& protocol)
{
    if ( port != this->getPrivatePort() )
        return;

    if ( Helper::cmpStrings( protocol, "UDP" ) )
    {
        emit this->initializeServerSignal();
        this->setUpnpPortAdded( true );

        this->startMasterCheckIn();

        upnpPortRefresh.start();
    }
    upnpPortTimeOut.stop();
}

//Private Timer Slots.
void Server::updateUsageTimeOutSlot()
{
    usageArray[ usageCounter ] = this->getPlayerCount();

    usageDays = 0;
    usageHours = 0;
    usageMins = 0;

    for ( qint32 i = 0; i < *Globals::SERVER_USAGE_48_HOURS; ++i )
    {
        const qint32 code{ usageArray[ ( i + usageCounter ) % *Globals::SERVER_USAGE_48_HOURS ] };
        const qint32 usageCap{ ( *Globals::SERVER_USAGE_48_HOURS - 1 ) - i };
        if ( usageCap < *Globals::SERVER_USAGE_DAYS )
        {
            usageDays += code;
            if ( usageCap < *Globals::SERVER_USAGE_HOURS )
            {
                usageHours += code;
                if ( usageCap < *Globals::SERVER_USAGE_MINUTES )
                    usageMins += code;
            }
        }
    }
    emit this->serverUsageChangedSignal( usageMins, usageDays, usageHours );

    ++usageCounter;
    usageCounter %= *Globals::SERVER_USAGE_48_HOURS;
}

void Server::masterCheckInTimeOutSlot()
{
    if ( this->getMasterInfoRecv() )
    {
        if (( !this->getUpnpPortAdded() && !this->getUseUPNP() )
          || ( this->getUpnpPortAdded() && this->getUseUPNP() )
          || ( !this->getUpnpPortAdded() && this->getUseUPNP() )
          || this->getUpnpTimedOut() )
        {
            this->sendMasterInfo();
            if ( !masterTimeOut.isActive() )
                masterTimeOut.start();
        }
    }
}

void Server::upnpTimeOutSlot()
{
    this->setUpnpTimedOut( true );
}

void Server::upTimerTimeOutSlot()
{
    this->connectionTimeUpdateSignal();
}

void Server::upnpPortRefreshTimeOutSlot()
{
    this->upnpPortForwardSignal( this->getPrivateIP(), this->getPrivatePort(), this->getUseUPNP() );
}

void Server::masterTimeOutSlot()
{
    this->setMasterTimedOut( true );
}
