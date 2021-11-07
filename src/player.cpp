
//Class includes.
#include "player.hpp"

//ReMix includes.
#include "packethandler.hpp"
#include "campexemption.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "server.hpp"
#include "theme.hpp"
#include "user.hpp"

//Qt Includes.
#include <QStandardItemModel>
#include <QApplication>
#include <QTcpSocket>
#include <QDateTime>

Player::Player(qintptr socketDescriptor, QSharedPointer<Server> svr)
    : server( svr )
{
    this->setSocketDescriptor( socketDescriptor );

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &Player::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );

    //Connect the Player to the ReadyRead slot.
    QObject::connect( this, &Player::readyRead, this, &Player::readyReadSlot, Qt::DirectConnection );

    //Connect to the CampExemptions slot.
    QObject::connect( this, &Player::hexSerNumSetSignal, CampExemption::getInstance(), &CampExemption::hexSerNumSetSlot );

    //All connections start as ascive and not AFK.
    this->setIsAFK( false );

    //Connect to the afkTimer.
    QObject::connect( &afkTimer, &QTimer::timeout, &afkTimer,
    [this]()
    {
        this->setIsAFK( true );
    } );

    //Start the AFK timer.
    afkTimer.start( static_cast<int>( Globals::MAX_AFK_TIME ) );

    //Connect the Player Object to a User UI signal.
    QObject::connect( User::getInstance(), &User::mutedSerNumDurationSignal, this,
    [=, this](const QString& sernum, const quint64& duration)
    {
        if ( !sernum.isEmpty() || duration > 0 )
        {
            if ( Helper::cmpStrings( sernum, this->getSernumHex_s() ) )
                this->setMuteDuration( duration );
        }
    });

    QObject::connect( &killTimer, &QTimer::timeout, &killTimer,
    [this]()
    {
        if ( this->getIsDisconnected() )
        {
            //Gracefully Disconnect the User.
            emit this->disconnected();
        }
    });

    floodTimer.start();
    idleTime.start();
}

Player::~Player()
{
    killTimer.stop();
    killTimer.disconnect();
    qDebug() << "Player Deconstructed.";
}

void Player::customDeconstruct(Player* plr)
{
    if ( plr != nullptr )
    {
        plr->disconnect();
        plr->deleteLater();
    }
}

QSharedPointer<Player> Player::createPlayer(qintptr socketDescriptor, QSharedPointer<Server> svr)
{
    QSharedPointer<Player> plr( new Player( socketDescriptor, svr ), customDeconstruct );
    plr->setThisPlayer( plr );

    return plr;
}

void Player::setThisPlayer(QSharedPointer<Player> plr)
{
    thisPlayer = plr;
}

QSharedPointer<Player> Player::getThisPlayer()
{
    return thisPlayer;
}

void Player::clearThisPlayer()
{
    this->disconnect();
    thisPlayer = nullptr;
}

qint64 Player::getConnTime() const
{
    return (( QDateTime::currentDateTime().currentSecsSinceEpoch() - this->getPlrConnectedTime() ) );
}

QSharedPointer<Server> Player::getServer()
{
    return server;
}

void Player::clearServer()
{
    server.clear();
}

bool Player::getIsVisible() const
{
    return isVisible;
}

void Player::setIsVisible(const bool& value)
{
    isVisible = value;

    emit this->setVisibleStateSignal( isVisible );
}

bool Player::getHasSernum() const
{
    return this->getSernum_i() != 0;
}

qint32 Player::getSernum_i() const
{
    return sernum_i;
}

void Player::setSernum_i(qint32 value)
{
    if ( value != this->getSernum_i() )
    {
        sernum_i = value;
        QString sernum_s{ Helper::serNumToIntStr( Helper::intToStr( value, static_cast<int>( IntBase::HEX ), 8 ), true ) };
        QString sernumHex_s{ Helper::serNumToHexStr( sernum_s ) };

        if ( !sernum_s.isEmpty() )
        {
            emit this->updatePlrViewSignal( this->getThisPlayer(), 1, sernum_s, Qt::DisplayRole, false );

            //Correct the User's BIO Data based on their serNum. This will only succeed if the User is on file.
            bool newBioData{ false };
            QByteArray data{ "" };

            if ( this->getHasBioData()
              && !Helper::strContainsStr( this->getBioData(), sernum_s ) )
            {
                data = User::getBIOData( sernumHex_s );
                if ( Helper::strContainsStr( QString( data ), sernum_s ) )
                {
                    newBioData = true;
                    this->setBioData( data );
                }
            }

            if ( newBioData && !data.isEmpty() )
                emit this->updatePlrViewSignal( this->getThisPlayer(), 7, data, Qt::DisplayRole, false );
        }

        this->setSernum_s( sernum_s );
        this->setSernumHex_s( sernumHex_s );

        quint64 muteDuration{ 0 };
        muteDuration = User::getIsPunished( PunishTypes::Mute, sernumHex_s, PunishTypes::SerNum, sernumHex_s );

        if ( muteDuration == 0 )
            muteDuration = User::getIsPunished( PunishTypes::Mute, this->getIPAddress(), PunishTypes::IP, sernumHex_s );

        if ( muteDuration >= 1 )
            this->setMuteDuration( muteDuration );

        server->sendPlayerSocketInfo();
    }
}

QString Player::getSernum_s() const
{
    return sernum_s;
}

void Player::setSernum_s(const QString& value)
{
    sernum_s = value;
}

QString Player::getSernumHex_s() const
{
    return sernumHex_s;
}

void Player::setSernumHex_s(const QString& value)
{
    sernumHex_s = value;

    emit this->hexSerNumSetSignal( this->getThisPlayer() );
}

qint32 Player::getTargetScene() const
{
    return targetHost;
}

void Player::setTargetScene(qint32 value)
{
    targetHost = value;
}

qint32 Player::getSceneHost() const
{
    return sceneHost;
}

void Player::setSceneHost(qint32 value)
{
    sceneHost = value;
}

qint32 Player::getTargetSerNum() const
{
    return targetSerNum;
}

void Player::setTargetSerNum(qint32 value)
{
    targetSerNum = value;
}

PktTarget Player::getTargetType() const
{
    return targetType;
}

void Player::setTargetType(const PktTarget& value)
{
    targetType = value;
}

QString Player::getPlrName() const
{
    if ( plrName.isEmpty() )
        return this->getSernum_s();
    return plrName;
}

void Player::setPlrName(const QString& value)
{
    plrName = value;
}

QByteArray Player::getCampPacket() const
{
    return campPacket;
}

void Player::setCampPacket(const QByteArray& value)
{
    campPacket = value;
}

void Player::forceSendCampPacket()
{
    QSharedPointer<Server> svr{ this->getServer() };
    if ( svr != nullptr )
    {
        PacketHandler* pktHandle{ PacketHandler::getInstance( svr ) };
        if ( pktHandle != nullptr )
            emit this->parsePacketSignal( this->getCampPacket(), this->getThisPlayer() );
    }
    svr = nullptr;
}

QString Player::getBioData() const
{
    return bioData;
}

void Player::setBioData(const QByteArray& value)
{
    bioData = value;
}

bool Player::getHasBioData() const
{
    return this->getBioData().isEmpty();
}

QByteArray Player::getOutBuff() const
{
    return outBuff;
}

void Player::setOutBuff(const QByteArray& value)
{
    outBuff = value;
}

qint32 Player::getPktHeaderExemptCount() const
{
    return pktHeaderExemptCount;
}

void Player::setPktHeaderExemptCount(const qint32& value)
{
    pktHeaderExemptCount = value;
}

QString Player::getIPAddress() const
{
    return this->peerAddress().toString();
}

QString Player::getIPPortAddress() const
{
    return QString( "%1:%2" ).arg( this->getIPAddress() ).arg( this->peerPort() );
}

qint32 Player::getPktHeaderSlot() const
{
    return pktHeaderSlot;
}

void Player::setPktHeaderSlot(const qint32& value)
{
    pktHeaderSlot = value;
}

qint64 Player::getFloodTime() const
{
    return floodTimer.elapsed();
}

void Player::restartFloodTimer()
{
    floodTimer.restart();
}

int Player::getPacketFloodCount() const
{
    return packetFloodCount;
}

void Player::setPacketFloodCount(const int& value)
{
    packetFloodCount = value;
}

int Player::getPacketsIn() const
{
    return packetsIn;
}

void Player::setPacketsIn(const int& value, const int& incr)
{
    packetsIn = value + incr;

    //Reset the Idle timer.
    idleTime.restart();

    this->setPacketFloodCount( this->getPacketFloodCount() + incr );
}

quint64 Player::getBytesIn() const
{
    return bytesIn;
}

void Player::setBytesIn(const quint64& value)
{
    bytesIn = value;
}

int Player::getPacketsOut() const
{
    return packetsOut;
}

void Player::setPacketsOut(const int& value)
{
    packetsOut = value;
}

quint64 Player::getBytesOut() const
{
    return bytesOut;
}

void Player::setBytesOut(const quint64& value)
{
    bytesOut = value;
}

bool Player::getSvrPwdRequested() const
{
    return svrPwdRequested;
}

void Player::setSvrPwdRequested(bool value)
{
    svrPwdRequested = value;
}

bool Player::getSvrPwdReceived() const
{
    return svrPwdReceived;
}

void Player::setSvrPwdReceived(const bool& value)
{
    svrPwdReceived = value;
}

void Player::resetAdminAuth()
{
    this->setAdminPwdRequested( false );
    this->setAdminPwdReceived( false );
    this->setNewAdminPwdRequested( false );
    this->setNewAdminPwdReceived( false );
}

bool Player::getAdminPwdRequested() const
{
    return adminPwdRequested;
}

void Player::setAdminPwdRequested(const bool& value)
{
    adminPwdRequested = value;
}

bool Player::getAdminPwdReceived() const
{
    return adminPwdReceived;
}

void Player::setAdminPwdReceived(const bool& value)
{
    adminPwdReceived = value;
}

bool Player::getNewAdminPwdRequested() const
{
    return newAdminPwdRequested;
}

void Player::setNewAdminPwdRequested(const bool& value)
{
    newAdminPwdRequested = value;
    if ( newAdminPwdRequested )
    {
        static const QString msg{ "The Server Host is attempting to register you as an Admin with the server. Please reply to this message with "
                                  "(/register *YOURPASS). Note: The server Host and other Admins will not have access to this information." };

        if ( this->getIsAdmin() )
            this->getServer()->sendMasterMessage( msg, this->getThisPlayer(), false );
    }
}

bool Player::getNewAdminPwdReceived() const
{
    return newAdminPwdReceived;
}

void Player::setNewAdminPwdReceived(const bool& value)
{
    newAdminPwdReceived = value;
}

bool Player::getIsAdmin() const
{
    return User::getIsAdmin( this->getSernumHex_s() );
}

GMRanks Player::getAdminRank() const
{
    return static_cast<GMRanks>( User::getAdminRank( this->getSernumHex_s() ) );
}

qint32 Player::getCmdAttempts() const
{
    return cmdAttempts;
}

void Player::setCmdAttempts(const qint32& value)
{
    cmdAttempts = value;
}

bool Player::getIsDisconnected() const
{
    return isDisconnected;
}

void Player::setDisconnected(const bool& value, const DCTypes& dcType)
{
    isDisconnected = value;
    if ( isDisconnected )
    {
        emit this->ipDCIncreaseSignal( dcType );
        killTimer.start( static_cast<int>( Globals::MAX_DISCONNECT_TTL ) );
    }
}

bool Player::getIsCampLocked() const
{
    return isCampLocked;
}

void Player::setIsCampLocked(bool value)
{
    CampExemption::getInstance()->setIsLocked( this->getSernumHex_s(), value );

    isCampLocked = value;
}

bool Player::getIsCampOptOut() const
{
    return isCampOptOut;
}

void Player::setIsCampOptOut(bool value)
{
    CampExemption::getInstance()->setAllowCurrent( this->getSernumHex_s(), value );
    isCampOptOut = value;
}

bool Player::getIsIncarnated() const
{
    return isIncarnated;
}

void Player::setIsIncarnated(bool newIsIncarnated)
{
    isIncarnated = newIsIncarnated;
}

bool Player::getIsGhosting() const
{
    return isGhosting;
}

void Player::setIsGhosting(bool newIsGhosting)
{
    isGhosting = newIsGhosting;
}

quint64 Player::getMuteDuration()
{
    return muteDuration;
}

void Player::setMuteDuration(const quint64& value)
{
    muteDuration = value;
}

qint64 Player::getPlrConnectedTime() const
{
    return plrConnectedTime;
}

void Player::setPlrConnectedTime(const qint64& value)
{
    plrConnectedTime = value;
}

qint64 Player::getCampCreatedTime() const
{
    return campCreatedTime;
}

void Player::setCampCreatedTime(const qint64& value)
{
    campCreatedTime = value;
}

qint64 Player::getMaxIdleTime() const
{
    return maxIdleTime;
}

void Player::setMaxIdleTime(const qint64& value)
{
    maxIdleTime = value;
}

bool Player::getIsMuted()
{
    quint64 date{ static_cast<quint64>( QDateTime::currentDateTimeUtc().toSecsSinceEpoch() ) };
    if ( this->getMuteDuration() <= date
      && this->getMuteDuration() >= 1 )
    {
        User::removePunishment( this->getSernumHex_s(), PunishTypes::Mute, PunishTypes::SerNum );
        this->setMuteDuration( 0 );
    }
    return this->getMuteDuration() >= date;
}

qint32 Player::getPlrLevel() const
{
    return plrLevel;
}

void Player::setPlrLevel(const qint32& value)
{
    plrLevel = value;
}

void Player::setIsAFK(bool value)
{
    isAFK = value;

    if ( isAFK )
        this->setAfkIcon( "AFK" );
    else
    {
        afkTimer.start( static_cast<int>( Globals::MAX_AFK_TIME ) );
        this->setAfkIcon( "NPK" );
    }
}

QIcon Player::getAfkIcon() const
{
    return afkIcon;
}

void Player::setAfkIcon(const QString& value)
{
    afkIcon = QIcon( ":/icon/" + value + ".png" );
}

void Player::validateSerNum(QSharedPointer<Server> server, const qint32& id)
{
    if ( server == nullptr )
        return;

    PlrDisconnectType type{ PlrDisconnectType::Invalid };
    bool isBlueCoded{ Helper::isBlueCodedSerNum( id ) };
    if ( isBlueCoded == true )
        type = PlrDisconnectType::BlueCodeSerNum;

    if (( this->getSernum_i() != id
      && id > 0 )
      || this->getSernum_i() == 0 )
    {
        if ( !isBlueCoded
          || !Settings::getSetting( SKeys::Setting, SSubKeys::DCBlueCodedSerNums ).toBool() )
        {
            if ( this->getSernum_i() == 0 )
            {
                //Disconnect the User if they have no SerNum, as we require SerNums.
                if ( Settings::getSetting( SKeys::Setting, SSubKeys::ReqSerNum ).toBool()
                  && id == 0 )
                {
                    type = PlrDisconnectType::InvalidSerNum;
                }
                else if ( id < 0 ) //The Player is connecting with a negative Sernum, disconnect them as a BlueCode.
                {
                    type = PlrDisconnectType::BlueCodeSerNum;
                }
                this->setSernum_i( id );
            }
            else if (( id > 0 && this->getSernum_i() != id )
                   && this->getSernum_i() > 0 )
            {
                //User's sernum has somehow changed. Disconnect them.
                type = PlrDisconnectType::SerNumChanged;
            }
        }

        if ( (( id == 1 ) || ( this->getSernum_i() == 1 ))
          || (( id == 0x4637B6ED ) || ( this->getSernum_i() == 0x4637B6ED )) )
        {
            type = PlrDisconnectType::SerNumOne;
        }

        if ( type != PlrDisconnectType::InvalidSerNum )
        {
            QString sernum{ Helper::serNumToIntStr( Helper::intToStr( id, static_cast<int>( IntBase::HEX ), 8 ), true ) };
            QString reason{ "Auto-Disconnect; %1: [ %2 ]" };

            bool disconnect{ false };
            switch ( type )
            {
                case PlrDisconnectType::SerNumChanged:
                    {
                        disconnect = true;

                        QString msg{ "%1 to %2" };
                                msg = msg.arg( this->getSernum_s() )
                                         .arg( sernum );

                        reason = reason.arg( "SerNum Changed" )
                                       .arg( msg );
                    }
                break;
                case PlrDisconnectType::InvalidSerNum:
                    {
                        disconnect = true;
                        reason = reason.arg( "Invalid SerNum" )
                                       .arg( sernum );
                    }
                break;
                case PlrDisconnectType::BlueCodeSerNum:
                    {
                        disconnect = false;
                        if ( Settings::getSetting( SKeys::Setting, SSubKeys::DCBlueCodedSerNums ).toBool() )
                            disconnect = true;

                        reason = reason.arg( "BlueCoded SerNum" )
                                       .arg( sernum );
                    }
                break;
                case PlrDisconnectType::SerNumOne:
                    {
                        const QString masterIP{ server->getMasterIP() };
                        const QString socketIP{ this->getIPAddress() };

                        QString message{ "" };
                        if ( !Helper::cmpStrings( masterIP, socketIP ) )
                        {
                            //Ban IP?
                            reason = "Auto-Disconnect; Unauthorized use of <SOUL 1>: Using IP [ %1 ] expected [ %2 ]";
                            reason = reason.arg( socketIP )
                                           .arg( masterIP );

                            disconnect = true;
                        }
                    }
                break;
                case PlrDisconnectType::Invalid:
                    {
                        disconnect = false;
                        break;
                    }
                break;
            }

            if ( disconnect == true )
            {
                server->sendMasterMessage( reason, this->getThisPlayer(), false );

                reason.append( ", [ %1 ]" );
                reason = reason.arg( this->getBioData() );

                emit this->insertLogSignal( server->getServerName(), reason, LKeys::PunishmentLog, true, true );

                this->setDisconnected( true, DCTypes::IPDC );
            }
        }
    }
}

bool Player::getIsGoldenSerNum()
{
    return !( this->getSernum_i() & static_cast<int>( Globals::MIN_HEX_SERNUM ) );
}

//Slots
void Player::sendPacketToPlayerSlot(QSharedPointer<Player> plr, qint32 targetType, qint32 trgSerNum, qint32 trgScene, const QByteArray& packet)
{
    //Source Player is this Player Object. Return without further processing.
    if ( plr == this )
        return;

    bool isAuth{ false };
    bool send{ false };

    if ( this->getSvrPwdReceived()
      || !this->getSvrPwdRequested() )
    {
        isAuth = true;
        send = false;

        switch( static_cast<PktTarget>( targetType ) )
        {
            case PktTarget::ALL:
            {
                send = true;
            }
            break;
            case PktTarget::PLAYER:
            {
                if ( trgSerNum == this->getSernum_i() )
                    send = true;
            }
            break;
            case PktTarget::SCENE:
            {
                if (( trgScene == this->getSernum_i()
                  || trgScene == this->getSceneHost() ))
                {
                    send = true;
                }
            }
            break;
        }
    }

    if ( send && isAuth )
        server->updateBytesOut( this->getThisPlayer(), this->write( packet, packet.length() ) );

    return;
}

void Player::sendMasterMsgToPlayerSlot(const QSharedPointer<Player> plr, const bool& all, const QByteArray& packet)
{
    qint64 bOut{ 0 };
    if ( !all
      || plr != nullptr )
    {
        if ( this == plr )
            bOut = this->write( packet, packet.length() );
    }
    else
        bOut = this->write( packet, packet.length() );

    if ( bOut >= 0 )
        server->updateBytesOut( this->getThisPlayer(), bOut );
}

void Player::setMaxIdleTimeSlot(const qint64& maxAFK)
{
    if ( maxAFK != this->getMaxIdleTime() )
        this->setMaxIdleTime( maxAFK );
}

void Player::connectionTimeUpdateSlot()
{
    ++connTime;

    QString baudIn{ "%1 %2, %3 Pkts" };
    QString baudOut{ "%1 %2, %3 Pkts" };

    QString bytesOutUnit{ "" };
    QString bytesOut{ "" };

    QString bytesInUnit{ "" };
    QString bytesIn{ "" };

    Helper::sanitizeToFriendlyUnits( this->getBytesOut(), bytesOut, bytesOutUnit );
    Helper::sanitizeToFriendlyUnits( this->getBytesIn(), bytesIn, bytesInUnit );

    baudIn = baudIn.arg( bytesIn )
                   .arg( bytesInUnit )
                   .arg( this->getPacketsIn() );

    baudOut = baudOut.arg( bytesOut )
                     .arg( bytesOutUnit )
                     .arg( this->getPacketsOut() );

    emit this->updatePlrViewSignal( this->getThisPlayer(), static_cast<int>( PlrCols::Time ), Helper::getTimeFormat( this->getConnTime() ), Qt::DisplayRole );
    emit this->updatePlrViewSignal( this->getThisPlayer(), static_cast<int>( PlrCols::BytesIn ), baudIn, Qt::DisplayRole );
    emit this->updatePlrViewSignal( this->getThisPlayer(), static_cast<int>( PlrCols::BytesOut ), baudOut, Qt::DisplayRole );

    //Color the User's IP address Green if the Admin is authed Otherwise, color as Red.
    Colors color{ Colors::Default };
    if ( this->getIsAdmin() )
    {
        if ( this->getAdminPwdReceived() )
            color = Colors::AdminValid;
        else
            color = Colors::AdminInvalid;
    }
    else
    {
        color = Colors::Default;
        if ( this->getIsGoldenSerNum() )
            color = Colors::GoldenSoul;
    }

    emit this->updatePlrViewSignal( this->getThisPlayer(), static_cast<int>( PlrCols::SerNum ), static_cast<int>( color ), Qt::ForegroundRole, true );

    //Color the User's IP address Red if the User's is muted. Otherwise, color as Green.
    if ( !this->getIsMuted() )
    {
        if ( !this->getIsVisible() )
            color = Colors::IPVanished;
        else
            color = Colors::IPValid;
    }
    else
        color = Colors::IPInvalid;

    //Color the User's IP/Port.
    emit this->updatePlrViewSignal( this->getThisPlayer(), static_cast<int>( PlrCols::IPPort ), this->getIPPortAddress(), Qt::DisplayRole, false );
    emit this->updatePlrViewSignal( this->getThisPlayer(), static_cast<int>( PlrCols::IPPort ), static_cast<int>( color ), Qt::ForegroundRole, true );

    //Set the NPK/AFK icon.
    emit this->updatePlrViewSignal( this->getThisPlayer(), static_cast<int>( PlrCols::SerNum ), this->getAfkIcon(), Qt::DecorationRole, false );

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::AllowIdle ).toBool()
      && !this->getIsDisconnected() ) //Do not attempt to disconnect a previously aisconnected user.
    {
        bool defaultIdleTime{ false };
        qint64 maxIdle{ this->getMaxIdleTime() };
        if ( maxIdle == static_cast<qint64>( Globals::MAX_IDLE_TIME ) )
            defaultIdleTime = true;

        if ((( idleTime.elapsed() >= static_cast<qint64>( Globals::MAX_IDLE_TIME ) )
            && defaultIdleTime )
          || ( idleTime.elapsed() >= maxIdle ) )
        {
            QString reason{ "Auto-Disconnect; Idle timeout: [ %1 ], [ %2 ]" };
                    reason = reason.arg( this->getSernum_s() )
                                   .arg( this->getBioData() );

            emit this->insertLogSignal( server->getServerName(), reason, LKeys::PunishmentLog, true, true );

            this->setDisconnected( true, DCTypes::IPDC );
        }
    }

    //Authenticate Remote Admins as required.
    if ( this->getIsAdmin() )
    {
        if ( this->getSernum_i() != 0
          && !this->getAdminPwdRequested() )
        {
            if ( !User::getHasPassword( this->getSernumHex_s() )
              && !this->getNewAdminPwdRequested() )
            {
                if ( !this->getNewAdminPwdReceived() )
                    this->setNewAdminPwdRequested( true );
            }
            else if (( Settings::getSetting( SKeys::Rules, SSubKeys::HasSvrPassword, server->getServerName() ).toString().isEmpty()
                   || this->getSvrPwdReceived() )
                   && !this->getNewAdminPwdRequested() )
            {
                if ( !this->getAdminPwdReceived() )
                {
                    static const QString msg{ "Remote Administrators are required to authenticate themselves before using commands. "
                                              "Please enter your password with the command (/login *PASS). Thank you!" };

                    if ( this->getIsAdmin() )
                    {
                        this->setAdminPwdRequested( true );
                        server->sendMasterMessage( msg, this->getThisPlayer(), false );
                    }
                }
            }
        }
    }
}

void Player::readyReadSlot()
{
    QByteArray data{ this->getOutBuff() };
    qint64 bIn{ data.length() };

    data.append( this->readAll() );
    server->setBytesIn( server->getBytesIn() + static_cast<quint64>( data.length() - bIn ) );

    if ( data.contains( "\r" )
      || data.contains( "\n" ) )
    {
        int bytes{ static_cast<int>( data.indexOf( "\r\n" ) ) };
        if ( bytes <= 0 )
            bytes = static_cast<int>( data.indexOf( "\n" ) );
        if ( bytes <= 0 )
            bytes = static_cast<int>( data.indexOf( "\r" ) );

        if ( bytes > 0 )
        {
            QByteArray packet{ data.left( bytes + 1 ) };
                       packet = packet.left( packet.length() - 1 );

            data = data.mid( bytes + 1 ).data();

            this->setOutBuff( data );

            this->setPacketsIn( this->getPacketsIn(), 1 );
            this->setBytesIn( this->getBytesIn() + static_cast<quint64>( packet.length() ) );

            //Only parse Packets if the Player is not muted.
            emit this->parsePacketSignal( packet, this->getThisPlayer() );

            if ( this->bytesAvailable() > 0
              || this->getOutBuff().size() > 0 )
            {
                emit this->readyRead();
            }
        }
    }
}
