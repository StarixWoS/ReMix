
//Class includes.
#include "player.hpp"

//ReMix Widget includes.
#include "widgets/plrlistwidget.hpp"

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
#include <QtCore>

Player::Player(qintptr socketDescriptor, QSharedPointer<Server> svr)
    : server( svr )
{
    this->setSocketDescriptor( socketDescriptor );

    //Connect the Player to the PlrListWidget Slots.
    QObject::connect( this, &Player::updatePlrViewSignal, PlrListWidget::getInstance( server ), &PlrListWidget::updatePlrViewSlot );

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &Player::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );

    //Connect the Player to the ReadyRead slot.
    QObject::connect( this, &Player::readyRead, this, &Player::readyReadSlot, Qt::DirectConnection );

    //Connect to the CampExemptions slot.
    QObject::connect( this, &Player::hexSerNumSetSignal, CampExemption::getInstance(), &CampExemption::hexSerNumSetSlot );

    //All connections start as active and not AFK.
    this->setIsAFK( false );

    //Connect Timers to Slots.
    QObject::connect( &vanishStateTimer, &QTimer::timeout, this, &Player::vanishStateTimerTimeOutSlot );
    QObject::connect( &serNumKillTimer, &QTimer::timeout, this, &Player::serNumKillTimerTimeOutSlot );
    QObject::connect( &afkTimer, &QTimer::timeout, this, &Player::afkTimerTimeOutSlot );
    QObject::connect( &killTimer, &QTimer::timeout, this, &Player::killTimerTimeOutSlot );

    //Start Timers.
    vanishStateTimer.setInterval( *Globals::PLAYER_VISIBLE_LOAD_TIMEOUT );
    serNumKillTimer.start( *Globals::MAX_SERNUM_TTL );
    floodTimer.start();
    idleTime.start();

    //Connect the Player Object to a User UI signal.
    QObject::connect( User::getInstance(), &User::mutedSerNumDurationSignal, this, &Player::mutedSerNumDurationSlot );
}

Player::~Player()
{
    //Ensure that the Server's playercount remains valid.
    if ( !this->getIsVisible() )
        emit this->setVisibleStateSignal( true );

    killTimer.stop();
    killTimer.disconnect();

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &Player::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );

    QString msg{ "Player( 0x%1 ) < %2 [ %3 ] > deconstructed." };
            msg = msg.arg( Helper::intToStr( reinterpret_cast<quintptr>( this ), IntBase::HEX, IntFills::DblWord ) )
                     .arg( this->getPlrName() )
                     .arg( this->getSernum_s() );

    emit this->insertLogSignal( server->getServerName(), msg, LKeys::MiscLog, true, true );

    this->disconnect();
}

void Player::customDeconstruct(Player* plr)
{
    if ( plr != nullptr )
        plr->deleteLater();
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

    User::setIsInVisible( this->getSernumHex_s(), !value );
    emit this->setVisibleStateSignal( isVisible );
}

bool Player::getHasSerNum() const
{
    return hasSerNum;
}

void Player::setHasSerNum(const bool& value)
{
    hasSerNum = value;
}

qint32 Player::getSernum_i() const
{
    return sernum_i;
}

void Player::setSernum_i(const qint32& value)
{
    if ( value != this->getSernum_i() )
    {
        sernum_i = value;
        QString sernum_s{ Helper::serNumToIntStr( Helper::intToStr( value, IntBase::HEX, IntFills::DblWord ), true ) };
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
        if ( sernum_i == 0 )
            sernumHex_s = "00000000"; //Special case.

        this->setSernumHex_s( sernumHex_s );

        quint64 muteDuration{ 0 };
        bool mutedSerNum{ true };
        muteDuration = User::getIsPunished( PunishTypes::Mute, sernumHex_s, PunishTypes::SerNum );

        if ( muteDuration == 0 )
        {
            muteDuration = User::getIsPunished( PunishTypes::Mute, this->getIPAddress(), PunishTypes::IP );
            mutedSerNum = false;
        }

        if ( muteDuration >= 1 )
        {
            this->setMuteDuration( muteDuration );
            QString muteMsg{ "Automatic-Mute; Mute restored from file. You are muted until [ %1 ] server time." };
                    muteMsg = muteMsg.arg( Helper::getTimeAsString( muteDuration ) );

            server->sendMasterMessage( muteMsg, this->getThisPlayer(), false );

            QString logMessage{ "Automatic-Mute; Mute restored from file for User [ %1 ]. Muted on [ %2 ] until [ %3 ]; With the reason [ %4 ]." };
            if ( mutedSerNum )
                logMessage = logMessage.arg( this->getSernum_s() );
            else
                logMessage = logMessage.arg( this->getIPAddress() );

            logMessage = logMessage.arg( Helper::getTimeAsString( User::getData( this->getSernumHex_s(), UKeys::Muted ).toInt() ) )
                                   .arg( Helper::getTimeAsString( User::getData( this->getSernumHex_s(), UKeys::MuteDuration ).toInt() ) )
                                   .arg( User::getData( this->getSernumHex_s(), UKeys::MuteReason ).toString() );

            emit this->insertLogSignal( server->getServerName(), logMessage, LKeys::PunishmentLog, true, true );
        }

        if ( User::getIsInVisible( sernumHex_s ) )
        {
            this->setIsVisible( false );
            vanishStateTimer.start( *Globals::PLAYER_VISIBLE_LOAD_TIMEOUT );

            static const QString muteMsg{ "Vanish State restored from file. You are currently invisible to other Players. "
                                          "You must Authenticate within < 5 > minutes to remain invisible!" };
            server->sendMasterMessage( muteMsg, this->getThisPlayer(), false );
        }
        server->sendPlayerSocketInfo();
    }

    this->setHasSerNum(( sernum_i != 0 ));
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

    this->setAdminRank( static_cast<GMRanks>( User::getAdminRank( sernumHex_s ) ) );

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

QByteArray& Player::getCampPacket()
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

        pktHandle = nullptr;
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
    if ( value
      && vanishStateTimer.isActive() )
    {
        vanishStateTimer.stop();
    }

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
    return isAdmin;
}

void Player::setIsAdmin(const bool& value)
{
    isAdmin = value;
}

GMRanks Player::getAdminRank() const
{
    return adminRank;
}

void Player::setAdminRank(const GMRanks& value)
{
    if ( adminRank > GMRanks::User
      && value == GMRanks::User )
    {
        this->resetAdminAuth();
    }

    adminRank = value;
    this->setIsAdmin( adminRank > GMRanks::User );
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
        killTimer.start( *Globals::MAX_DISCONNECT_TTL );
    }
}

bool Player::getQuarantineOverride() const
{
    return isQuarantineOverride;
}

void Player::setQuarantineOverride(const bool& value)
{
    isQuarantineOverride = value;
}

bool Player::getIsQuarantined() const
{
    //Override manually applied by the Server Owner.
    if ( this->getQuarantineOverride() )
        return false;

    return isQuarantined;
}

void Player::setQuarantined(const bool& value)
{
    isQuarantined = value;
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
    this->updateIconState();
}

bool Player::getIsGhosting() const
{
    return isGhosting;
}

void Player::setIsGhosting(bool newIsGhosting)
{
    isGhosting = newIsGhosting;
    this->updateIconState();
}

quint64 Player::getMuteDuration()
{
    return muteDuration;
}

void Player::setMuteDuration(const quint64& value)
{
    if ( muteDuration == value ) //Duration unchanged.
        return;

    muteDuration = value;
    if ( value == 0 )
        User::removePunishment( this->getSernumHex_s(), PunishTypes::Mute, PunishTypes::SerNum );
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

bool Player::getIsMuted()
{
    quint64 date{ static_cast<quint64>( QDateTime::currentDateTimeUtc().toSecsSinceEpoch() ) };
    if ( this->getMuteDuration() <= date
      && this->getMuteDuration() >= 1 )
    {
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

qint32 Player::getPlrCheatCount() const
{
    return plrCheatCount;
}

void Player::setPlrCheatCount(const qint32& value)
{
    static const QString quarantine{ "You have been quarantined due to to the Rule \"noCheat\" being *Strictly Enforced*. "
                                     "You may only interact with other Quarantined Users." };
    static const QString unQuarantine{ "You have been un-quarantined." };
    const bool quarantineed{ this->getIsQuarantined() };
    if ( this->getIsIncarnated() )
    {
        if ( value > 0
          && Settings::getSetting( SKeys::Rules, SSubKeys::StrictRules, server->getServerName() ).toBool() )
        {
            if ( !quarantineed ) //Only quarantine once.
            {
                server->sendMasterMessage( quarantine, this->getThisPlayer(), false );
                if ( !this->getIsQuarantined() )
                    this->setQuarantined( true );
            }
        }
        else
        {
            if ( quarantineed ) //Inform unquarantine.
                server->sendMasterMessage( unQuarantine, this->getThisPlayer(), false );

            this->setQuarantined( false ); //Ensure unquarantined state.
        }
        plrCheatCount = value;
    }
}

qint32 Player::getPlrModCount() const
{
    return plrModCount;
}

void Player::setPlrModCount(const qint32& value)
{
    static const QString quarantine{ "You have been quarantined due to to the Rule \"noMod\" being *Strictly Enforced*. "
                                     "You may only interact with other Quarantined Users." };
    static const QString unQuarantine{ "You have been un-quarantined." };
    const bool quarantineed{ this->getIsQuarantined() };

    if ( this->getIsIncarnated() )
    {
        if ( ( value & 2 )
          && Settings::getSetting( SKeys::Rules, SSubKeys::StrictRules, server->getServerName() ).toBool() )
        {
            if ( !quarantineed ) //Only quarantine once.
            {
                server->sendMasterMessage( quarantine, this->getThisPlayer(), false );
                if ( !this->getIsQuarantined() )
                    this->setQuarantined( true );
            }
        }
        else
        {
            if ( quarantineed ) //Inform unquarantine.
                server->sendMasterMessage( unQuarantine, this->getThisPlayer(), false );

            this->setQuarantined( false ); //Ensure unquarantined state.
        }
        plrModCount = value;
    }
}

qint32 Player::getIsPartyLocked() const
{
    return isPartyLocked;
}

void Player::setIsPartyLocked(const qint32& value)
{
    isPartyLocked = value;
}

void Player::updateIconState()
{
    const bool incarnated{ this->getIsIncarnated() };

    IconRoles role{ IconRoles::SoulWell };
    if ( isAFK )
    {
        role = ( incarnated == true ? IconRoles::SoulAFK : IconRoles::SoulAFKWell );
    }
    else
    {
        if ( incarnated )
        {
            const bool golden{ this->getIsGoldenSerNum() };
            if ( !this->getIsGhosting() )
            {
                if ( this->getPlrCheatCount() > 0 )
                {
                    role = IconRoles::SoulCheater;
                }
                else if ( ( this->getPlrModCount() & 2 ) == 2 )
                {
                    role = IconRoles::SoulModder;
                }
                else if ( this->getIsPK() )
                {
                    if ( golden )
                    {
                        role = IconRoles::GSoulPK;
                    }
                    else
                        role = IconRoles::SoulPK;
                }
                else if ( golden )
                {
                    role = IconRoles::GSoulNPK;
                }
                else
                    role = IconRoles::SoulNPK;
            }
            else if ( golden )
            {
                role = IconRoles::GSoulGhost;
            }
            else
                role = IconRoles::SoulGhost;
        }
        else
            role = IconRoles::SoulWell;
    }
    emit this->updatePlrViewSignal( this->getThisPlayer(), *PlrCols::SerNum, *role, Qt::DecorationRole, false );
}

bool Player::getIsAFK()
{
    return isAFK;
}

void Player::setIsAFK(const bool& value)
{
    isAFK = value;
    if ( isAFK )
        afkTimer.start( server->getMaxAFKTime() );
    else
        afkTimer.stop();
}

bool Player::getIsPK()
{
    return isPK;
}

void Player::setIsPK(const bool& value)
{
    if ( this->getIsIncarnated() )
    {
        if ( Settings::getSetting( SKeys::Rules, SSubKeys::StrictRules, server->getServerName() ).toBool() )
        {
            if ( Settings::getSetting( SKeys::Rules, SSubKeys::AllPK, server->getServerName() ).toBool() )
            {
                if ( !value )
                {
                    static const QString msg{ "You have been disconnected due to the Rule \"allPK\" being *Strictly Enforced*." };
                    server->sendMasterMessage( msg, this->getThisPlayer(), false );
                    this->setDisconnected( true, DCTypes::PktDC );
                }
            }
        }
        else if ( Settings::getSetting( SKeys::Rules, SSubKeys::NoPK, server->getServerName() ).toBool() )
        {
            if ( value )
            {
                static const QString msg{ "You have been disconnected due to the Rule \"noPK\" being *Strictly Enforced*." };
                server->sendMasterMessage( msg, this->getThisPlayer(), false );
                this->setDisconnected( true, DCTypes::PktDC );
            }
        }

        isPK = value;
        this->updateIconState();
    }
}

void Player::validateSerNum(const qint32& id)
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
            QString sernum{ Helper::serNumToIntStr( Helper::intToStr( id, IntBase::HEX, IntFills::DblWord ), true ) };
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
    return !( this->getSernum_i() & *Globals::MIN_HEX_SERNUM );
}

//Slots
void Player::vanishStateTimerTimeOutSlot()
{
    const QString message{ "The grace period of < 5 > minutes for your Vanish state has expired. You are now visible to other Players!" };
    if ( !this->getIsVisible() )
    {
        this->setIsVisible( true );
        server->sendMasterMessage( message, this->getThisPlayer(), false );
    }
}

void Player::sendPacketToPlayerSlot(QSharedPointer<Player> plr, const qint32& targetType, const qint32& trgSerNum,
                                    const qint32& trgScene, const QByteArray& packet)
{
    //Source Player is this Player Object. Return without further processing.
    if ( plr == this )
        return;

    if ( plr != nullptr )
    {
        if ( !this->getIsAdmin() )
        {
            if ( !plr->getIsVisible() )
                return;
        }

        //Quarantined players can only communicate with the quarantined.
        if ( !this->getIsQuarantined()
          && plr->getIsQuarantined() )
        {
            if ( !plr->getQuarantineOverride() )
                return;
        }
    }

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

void Player::refreshAFKTimersSlot(const qint64& maxAFK)
{
    if ( maxAFK > 0 )
        afkTimer.start( maxAFK );
    else
        afkTimer.stop();
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
                   .arg( QString::number( this->getPacketsIn() ) );

    qint64 bufferSize{ this->getOutBuff().size() };
    if ( bufferSize > 0 )
    {
        QString bufferStr{ ", Buffer Size %1 %2" };
        QString bytesInBufferUnit{ "" };
        QString bytesInBuffer{ "" };

        Helper::sanitizeToFriendlyUnits( bufferSize, bytesInBuffer, bytesInBufferUnit );
        bufferStr = bufferStr.arg( bytesInBuffer )
                             .arg( bytesInBufferUnit );
        baudIn = baudIn.append( bufferStr );
    }

    baudOut = baudOut.arg( bytesOut )
                     .arg( bytesOutUnit )
                     .arg( QString::number( this->getPacketsOut() ) );

    emit this->updatePlrViewSignal( this->getThisPlayer(), *PlrCols::Time, Helper::getTimeFormat( this->getConnTime() ), Qt::DisplayRole );
    emit this->updatePlrViewSignal( this->getThisPlayer(), *PlrCols::BytesIn, baudIn, Qt::DisplayRole );
    emit this->updatePlrViewSignal( this->getThisPlayer(), *PlrCols::BytesOut, baudOut, Qt::DisplayRole );

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

    emit this->updatePlrViewSignal( this->getThisPlayer(), *PlrCols::SerNum, *color, Qt::ForegroundRole, true );
    this->updateIconState();

    //Color the User's IP address Red if the User's is muted or quarantined. Otherwise, color as Green.
    if ( !this->getIsMuted() )
    {
        if ( !this->getIsVisible() )
            color = Colors::IPVanished;
        else if ( this->getIsQuarantined() )
            color = Colors::IPQuarantined;
        else
            color = Colors::IPValid;
    }
    else
        color = Colors::IPInvalid;

    //Color the User's IP/Port.
    emit this->updatePlrViewSignal( this->getThisPlayer(), *PlrCols::IPPort, this->getIPPortAddress(), Qt::DisplayRole, false );
    emit this->updatePlrViewSignal( this->getThisPlayer(), *PlrCols::IPPort, *color, Qt::ForegroundRole, true );

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::AllowIdle ).toBool()
      && !this->getIsDisconnected() ) //Do not attempt to disconnect a previously disconnected user.
    {
        if ( idleTime.elapsed() >= *Globals::MAX_IDLE_TIME )
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
                    this->setAdminPwdRequested( true );
                    server->sendMasterMessage( msg, this->getThisPlayer(), false );
                }
            }
        }
    }
}

void Player::setAdminRankSlot(const QString& hexSerNum, const GMRanks& rank)
{
    if ( Helper::cmpStrings( hexSerNum, this->getSernumHex_s() ) )
    {
        static const QString revoke{ "Your Remote Administrator privileges have been revoked by the Server Host. Please contact the Server Host "
                                    "if you believe this was in error." };
        static const QString changed{ "Your Remote Administrator privileges have been altered by the Server Host. Please contact the Server Host "
                                     "if you believe this was in error." };
        static const QString reinstated{ "Your Remote Administrator privelages have been partially reinstated by the Server Host." };

        GMRanks currentRank{ this->getAdminRank() };
        if ( rank != currentRank )
        {
            QString msg{ changed };
            if ( currentRank == GMRanks::User )
            {
                msg = reinstated;
            }
            else
            {
                if ( rank == GMRanks::User )
                    msg = revoke;
            }
            server->sendMasterMessage( msg, this->getThisPlayer(), false );
        }
        this->setAdminRank( rank );
    }
}

//Private Slots.
void Player::mutedSerNumDurationSlot(const QString& sernum, const quint64& duration, const QString& reason)
{
    if ( !sernum.isEmpty() )
    {
        if ( Helper::cmpStrings( sernum, this->getSernumHex_s() ) )
        {
            this->setMuteDuration( duration );
            if ( !reason.isEmpty() )
                server->sendMasterMessage( reason, this->getThisPlayer(), false );
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
        qint32 bytes{ 0 };
        auto findNewLine =
        [=](const QByteArray& cData)
        {
            qint32 tBytes{ static_cast<qint32>( cData.indexOf( "\r\n" ) ) };
            if ( tBytes <= 0 )
                tBytes = static_cast<qint32>( cData.indexOf( "\n" ) );
            if ( tBytes <= 0 )
                tBytes = static_cast<qint32>( cData.indexOf( "\r" ) );

            return tBytes;
        };

        bytes = findNewLine( data );
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

//Private Timer Slots.
void Player::serNumKillTimerTimeOutSlot()
{
    static const QString reason{ "Auto-Disconnect; SerNum Await Timeout." };
    static const QString message{ " ReMix requires a SerNum to be sent within 5 Minutes of connecting." };
    QString append{ ", [ %1 ][ %2 ]" };

    //The User has been given 5 minues to send a valid SerNum. Disconnect them.
    if ( !this->getHasSerNum() )
    {
        append = append.arg( this->getIPAddress() )
                       .arg( this->getBioData() );
        server->sendMasterMessage( reason + message, this->getThisPlayer(), false );

        emit this->insertLogSignal( server->getServerName(), reason + append, LKeys::PunishmentLog, true, true );

        this->setDisconnected( true, DCTypes::IPDC );
    }
}

void Player::killTimerTimeOutSlot()
{
    if ( this->getIsDisconnected() )
    {
        //Gracefully Disconnect the User.
        emit this->disconnected();
    }
}

void Player::afkTimerTimeOutSlot()
{
    //Player is not AFK, ignore.
    if ( !this->getIsAFK() )
        return;

    if ( Settings::getSetting( SKeys::Rules, SSubKeys::MaxAFK, server->getServerName() ).toBool()
      && !this->getIsDisconnected() ) //Do not attempt to disconnect a previously disconnected user.
    {
        QString reason{ "Auto-Disconnect; AFK timeout: [ %1 ], [ %2 ]" };
                reason = reason.arg( this->getSernum_s() )
                               .arg( this->getBioData() );

        emit this->insertLogSignal( server->getServerName(), reason, LKeys::PunishmentLog, true, true );

        this->setDisconnected( true, DCTypes::IPDC );
    }
}
