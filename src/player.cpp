
//Class includes.
#include "player.hpp"

//ReMix includes.
#include "packethandler.hpp"
#include "serverinfo.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "theme.hpp"
#include "user.hpp"

//Qt Includes.
#include <QStandardItemModel>
#include <QApplication>
#include <QTcpSocket>
#include <QDateTime>

Player::Player(qintptr socketDescriptor)

{
    this->setSocketDescriptor( socketDescriptor );

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &Player::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );

    //Connect the Player to the ReadyRead slot.
    QObject::connect( this, &Player::readyRead, this, &Player::readyReadSlot, Qt::DirectConnection );

    //All connections start as ascive and not AFK.
    this->setIsAFK( false );

    //Connect to the afkTimer.
    QObject::connect( &afkTimer, &QTimer::timeout, &afkTimer,
    [this]()
    {
        this->setIsAFK( true );
    } );

    //Start the AFK timer.
    afkTimer.start( MAX_AFK_TIME );

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
            this->flush();
            this->close();
        }
    });
    floodTimer.start();
    idleTime.start();
}

Player::~Player()
{
    killTimer.stop();
    killTimer.disconnect();

    this->disconnect();
    this->deleteLater();
}

qint64 Player::getConnTime() const
{
    return (( QDateTime::currentDateTime().currentMSecsSinceEpoch() - this->getPlrConnectedTime() ) / 1000 );
}

QStandardItem* Player::getTableRow() const
{
    return tableRow;
}

void Player::setTableRow(QStandardItem* value)
{
    tableRow = value;
}

void Player::setTableRowData(QStandardItem* model, const qint32& row, const qint32& column, const QVariant& data,
                             const qint32& role, const bool& isColor)
{
    if ( model != nullptr )
    {
        QStandardItemModel* sModel = model->model();
        if ( model != nullptr )
        {
            if ( isColor )
                sModel->setData( model->model()->index( row, column ), Theme::getThemeColor( static_cast<Colors>( data.toInt() ) ), role );
            else
                sModel->setData( model->model()->index( row, column ), data, role );
        }
    }
}

ServerInfo* Player::getServerInfo() const
{
    return serverInfo;
}

void Player::setServerInfo(ServerInfo* value)
{
    serverInfo = value;
}

bool Player::getIsVisible() const
{
    return isVisible;
}

void Player::setIsVisible(const bool& value)
{
    isVisible = value;
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
            QStandardItem* row{ this->getTableRow() };
            if ( row != nullptr )
            {
                QStandardItemModel* model{ row->model() };
                if ( model != nullptr )
                {
                    model->setData( model->index( row->row(), 1 ), sernum_s, Qt::DisplayRole );

                    //Correct the User's BIO Data based on their serNum. This will only succeed if the User is on file.
                    bool newBioData{ false };
                    QByteArray data{ "" };

                    if ( this->getHasBioData()
                      && !Helper::strContainsStr( this->getBioData(), sernum_s ) )
                    {
                        data = User::getBIOData( sernumHex_s );
                        if ( !sernum_s.isEmpty() )
                        {
                            if ( Helper::strContainsStr( QString( data ), sernum_s ) )
                            {
                                newBioData = true;
                                this->setBioData( data );
                            }
                        }
                    }

                    if ( newBioData && !data.isEmpty() )
                        model->setData( model->index( row->row(), 7 ), data, Qt::DisplayRole );
                }
            }
        }

        this->setSernum_s( sernum_s );
        this->setSernumHex_s( sernumHex_s );
        this->loadCampOptOut();

        quint64 muteDuration{ 0 };
        muteDuration = User::getIsPunished( PunishTypes::Mute, sernumHex_s, PunishTypes::SerNum, sernumHex_s );

        if ( muteDuration <= 0 )
            muteDuration = User::getIsPunished( PunishTypes::Mute, this->peerAddress().toString(), PunishTypes::IP, sernumHex_s );

        if ( muteDuration >= 1 )
            this->setMuteDuration( muteDuration );

        serverInfo->sendPlayerSocketInfo();
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

QString Player::getPlayTime() const
{
    return playTime;
}

void Player::setPlayTime(const QString& value)
{
    playTime = value;
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

QString Player::getAlias() const
{
    return alias;
}

void Player::setAlias(const QString& value)
{
    alias = value;
}

QByteArray Player::getCampPacket() const
{
    return campPacket;
}

void Player::setCampPacket(const QByteArray& value)
{
    campPacket = value;
}

bool Player::getSentCampPacket() const
{
    return sentCampPacket;
}

void Player::setSentCampPacket(bool value)
{
    sentCampPacket = value;
}

void Player::forceSendCampPacket()
{
    ServerInfo* svr{ this->getServerInfo() };
    if ( svr != nullptr )
    {
        PacketHandler* pktHandle{ svr->getPktHandle() };
        if ( pktHandle != nullptr )
            emit this->parsePacketSignal( this->getCampPacket(), this );
    }
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

int Player::getSlotPos() const
{
    return slotPos;
}

void Player::setSlotPos(const int& value)
{
    slotPos = value;
}

qint32 Player::getPktHeaderExemptCount() const
{
    return pktHeaderExemptCount;
}

void Player::setPktHeaderExemptCount(const qint32& value)
{
    pktHeaderExemptCount = value;
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
        QString msg{ "The Server Host is attempting to register you as an Admin with the server. Please reply to this message with "
                     "(/register *YOURPASS). Note: The server Host and other Admins will not have access to this information." };

        if ( this->getIsAdmin() )
            this->getServerInfo()->sendMasterMessage( msg, this, false );
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

qint32 Player::getAdminRank() const
{
    return User::getAdminRank( this->getSernumHex_s() );
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
        ServerInfo* server{ this->getServerInfo() };
        if ( server != nullptr )
        {
            //Increment the disconnect count for the specific type.
            switch ( dcType )
            {
                case DCTypes::IPDC:
                    {
                        server->setIpDc( server->getIpDc() + 1 );
                    }
                break;
                case DCTypes::DupDC:
                    {
                        server->setDupDc( server->getDupDc() + 1 );
                    }
                break;
                case DCTypes::PktDC:
                    {
                        server->setPktDc( server->getPktDc() + 1 );
                    }
                break;
            }
        }
        killTimer.start( MAX_DISCONNECT_TTL );
    }
}

bool Player::getIsCampLocked() const
{
    return isCampLocked;
}

void Player::setIsCampLocked(bool value)
{
    isCampLocked = value;
}

bool Player::getIsCampOptOut() const
{
    return isCampOptOut;
}

void Player::setIsCampOptOut(bool value)
{
    User::setCampOptOut( this->getSernumHex_s(), value );
    isCampOptOut = value;
}

void Player::loadCampOptOut()
{
    bool optout{ User::getCampOptOut( this->getSernumHex_s() ) };
    this->setIsCampOptOut( optout );
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

bool Player::getIsAFK() const
{
    return isAFK;
}

void Player::setIsAFK(bool value)
{
    isAFK = value;

    if ( isAFK )
        this->setAfkIcon( "AFK" );
    else
    {
        afkTimer.start( MAX_AFK_TIME );
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

void Player::validateSerNum(ServerInfo* server, const qint32& id)
{
    if ( server == nullptr )
        return;

    QString masterIP{ server->getMasterIP() };
    QString socketIP{ this->peerAddress().toString() };

    QString message{ "" };
    QString reason{ "" };

    bool isBlueCoded{ Helper::isBlueCodedSerNum( id ) };
    bool serNumChanged{ false };
    bool zeroSerNum{ false };
    bool disconnect{ false };

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
                    zeroSerNum = true;
                    disconnect = true;
                }
                else if ( id < 0 ) //The Player is connecting with a negative Sernum, disconnect them as a BlueCode.
                {
                    disconnect = true;
                    isBlueCoded = true;
                }
                this->setSernum_i( id );
            }
            else if (( id > 0 && this->getSernum_i() != id )
                   && this->getSernum_i() > 0 )
            {
                //User's sernum has somehow changed. Disconnect them.
                serNumChanged = true;
                disconnect = true;
            }
        }

        if ( disconnect
          || ( isBlueCoded
            && Settings::getSetting( SKeys::Setting, SSubKeys::DCBlueCodedSerNums ).toBool() ) )
        {
            QString sernum{ Helper::serNumToIntStr( Helper::intToStr( id, static_cast<int>( IntBase::HEX ), 8 ), true ) };
            if ( serNumChanged )
            {
                message = "Auto-Disconnect; SerNum Changed";
                reason = "%1: [ %2 ] to [ %3 ], [ %4 ]";
                reason = reason.arg( message )
                               .arg( this->getSernum_s() )
                               .arg( sernum )
                               .arg( this->getBioData() );
            }
            else if ( zeroSerNum )
            {
                message = "Auto-Disconnect; Invalid SerNum";
                reason = "%1: [ %2 ], [ %3 ]";
                reason = reason.arg( message )
                               .arg( sernum )
                               .arg( this->getBioData() );
            }
            else if ( isBlueCoded )
            {
                message = "Auto-Disconnect; BlueCoded SerNum";
                reason = "%1: [ %2 ], [ %3 ]";
                reason = reason.arg( message )
                               .arg( sernum )
                               .arg( this->getBioData() );
            }

            server->sendMasterMessage( message, this, false );

            emit this->insertLogSignal( serverInfo->getServerName(), reason, LogTypes::PUNISHMENT, true, true );

            this->setDisconnected( true, DCTypes::IPDC );
        }
    }

    if ( id == 1 || this->getSernum_i() == 1 )
    {
        if ( !Helper::cmpStrings( masterIP, socketIP ) )
        {
            //Ban IP?
            message = "Auto-Mute; Attempting to use SerNum 1 with the incorrect IP address. Be warned that this may become a ban within future ReMix versions.";
            server->sendMasterMessage( message, this, false );

            reason = "Automatic Network Mute of <[ %1 ][ %2 ]> due to the usage of <[ Soul 1 ][ %3 ]> while connecting from an improper IP Address.";
            reason = reason.arg( this->getSernum_s() )
                           .arg( socketIP )
                           .arg( masterIP );

            User::addMute( nullptr, this, reason, false, true, PunishDurations::THIRTY_MINUTES );

            emit this->insertLogSignal( server->getServerName(), reason, LogTypes::PUNISHMENT, true, true );
        }
    }
}

//Slots
void Player::sendPacketToPlayerSlot(Player* plr, qint32 targetType, qint32 trgSerNum, qint32 trgScene, const QByteArray& packet)
{
    //Source Player is this Player Object. Return without further processing.
    if ( plr == this )
        return;

    qint64 bOut{ 0 };
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
    {
        bOut = this->write( packet, packet.length() );
        serverInfo->updateBytesOut( this, bOut );
    }
    return;
}

void Player::sendMasterMsgToPlayerSlot(Player* plr, const bool& all, const QByteArray& packet)
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
        serverInfo->updateBytesOut( this, bOut );
}

void Player::setMaxIdleTimeSlot(const qint64& maxAFK)
{
    if ( maxAFK != this->getMaxIdleTime() )
        this->setMaxIdleTime( maxAFK );
}

void Player::connectionTimeUpdateSlot()
{
    ++connTime;

    QStandardItem* row{ this->getTableRow() };
    QString baudIn{ "%1 %2, %3 Pkts" };
    QString baudOut{ "%1 %2, %3 Pkts" };
    if ( row != nullptr )
    {
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

        this->setTableRowData( row, row->row(), static_cast<int>( PlrCols::Time ), Helper::getTimeFormat( this->getConnTime() ), Qt::DisplayRole );
        this->setTableRowData( row, row->row(), static_cast<int>( PlrCols::BytesIn ), baudIn, Qt::DisplayRole );
        this->setTableRowData( row, row->row(), static_cast<int>( PlrCols::BytesOut ), baudOut, Qt::DisplayRole );

        //Color the User's IP address Green if the Admin is authed Otherwise, color as Red.
        Colors color{ Colors::Default };
        if ( this->getIsAdmin() )
        {
            if ( this->getAdminPwdReceived() )
                color = Colors::Valid;
            else
                color = Colors::Invalid;
        }
        else
        {
            color = Colors::Default;
            if ( !( this->getSernum_i() & MIN_HEX_SERNUM ) )
                color = Colors::GoldenSoul;
        }

        this->setTableRowData( row, row->row(), static_cast<int>( PlrCols::SerNum ), static_cast<int>( color ), Qt::ForegroundRole, true );

        //Color the User's IP address Red if the User's is muted. Otherwise, color as Green.
        color = Colors::Default;
        if ( !this->getIsMuted() )
        {
            if ( !this->getIsVisible() )
                color = Colors::Invisible;
            else
                color = Colors::Valid;
        }
        else
            color = Colors::Invalid;

        this->setTableRowData( row, row->row(), static_cast<int>( PlrCols::IPPort ), static_cast<int>( color ), Qt::ForegroundRole, true );

        //Set the NPK/AFK icon.
        this->setTableRowData( row, row->row(), static_cast<int>( PlrCols::SerNum ), this->getAfkIcon(), Qt::DecorationRole, false );
    }

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::AllowIdle ).toBool()
      && !this->getIsDisconnected() ) //Do not attempt to disconnect a previously aisconnected user.
    {
        bool defaultIdleTime{ false };
        qint64 maxIdle{ this->getMaxIdleTime() };
        if ( maxIdle == static_cast<qint64>( MAX_IDLE_TIME ) )
            defaultIdleTime = true;

        if ((( idleTime.elapsed() >= static_cast<qint64>( MAX_IDLE_TIME ) )
            && defaultIdleTime )
          || ( idleTime.elapsed() >= maxIdle ) )
        {
            QString reason{ "Auto-Disconnect; Idle timeout: [ %1 ], [ %2 ]" };
                    reason = reason.arg( this->getSernum_s() )
                                   .arg( this->getBioData() );

            emit this->insertLogSignal( serverInfo->getServerName(), reason, LogTypes::PUNISHMENT, true, true );

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
            else if (( Settings::getSetting( SKeys::Rules, SSubKeys::HasSvrPassword, serverInfo->getServerName() ).toString().isEmpty()
                   || this->getSvrPwdReceived() )
                   && !this->getNewAdminPwdRequested() )
            {
                if ( !this->getAdminPwdReceived() )
                {
                    QString msg{ "The server Admin requires all Remote Administrators to authenticate themselves "
                                 "with their password. Please enter your password with the command (/login *PASS) "
                                 "or be denied access to Remote Administrator privelages. Thank you!" };

                    if ( this->getIsAdmin() )
                    {
                        this->setAdminPwdRequested( true );
                        serverInfo->sendMasterMessage( msg, this, false );
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
    serverInfo->setBytesIn( serverInfo->getBytesIn() + static_cast<quint64>( data.length() - bIn ) );

    if ( data.contains( "\r" )
      || data.contains( "\n" ) )
    {
        int bytes{ static_cast<int>( data.indexOf( "\r\n" ) ) };
        if ( bytes <= 0 )
            bytes = static_cast<int>( data.indexOf( "\r\n" ) );
        if ( bytes <= 0 )
            bytes = static_cast<int>( data.indexOf( "\r\n" ) );

        if ( bytes > 0 )
        {
            QByteArray packet{ data.left( bytes + 1 ) };
                       packet = packet.left( packet.length() - 1 );

            data = data.mid( bytes + 1 ).data();

            this->setOutBuff( data );

            this->setPacketsIn( this->getPacketsIn(), 1 );
            this->setBytesIn( this->getBytesIn() + static_cast<quint64>( packet.length() ) );

            //Only parse Packets if the Player is not muted.
            emit this->parsePacketSignal( packet, this );

            if ( this->bytesAvailable() > 0
              || this->getOutBuff().size() > 0 )
            {
                emit this->readyRead();
            }
        }
    }
}
