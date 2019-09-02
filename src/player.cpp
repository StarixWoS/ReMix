
//Class includes.
#include "player.hpp"

//ReMix includes.
#include "serverinfo.hpp"
#include "settings.hpp"
#include "sendmsg.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "theme.hpp"
#include "user.hpp"

//Qt Includes.
#include <QStandardItemModel>
#include <QApplication>
#include <QTcpSocket>

Player::Player()
{
    //Update the User's UI row. --Every 1000MS.
    connTimer.start( 1000 );
    
    //All connections start as ascive and not AFK.
    this->setIsAFK( false );

    //Connect to the afkTimer.
    QObject::connect( &afkTimer, &QTimer::timeout, [this]()
    {
        this->setIsAFK( true );
    });

    //Start the AFK timer. - 300,000 milliseconds - 5 minutes.
    afkTimer.start( MAX_AFK_TIME );

    QObject::connect( &connTimer, &QTimer::timeout, &connTimer,
    [=]()
    {
        ++connTime;

        QStandardItem* row = this->getTableRow();
        QString baudIn{ "%1Bd, %2B, %3 Pkts" };
        QString baudOut{ "%1Bd, %2B, %3 Pkts" };
        if ( row != nullptr )
        {
            this->setModelData( row, row->row(),
                                static_cast<int>( PlrCols::Time ),
                                Helper::getTimeFormat( this->getConnTime() ),
                                Qt::DisplayRole );

            this->setAvgBaud( this->getBytesIn(), false );
            baudIn = baudIn.arg( this->getAvgBaud( false ) )
                           .arg( this->getBytesIn() )
                           .arg( this->getPacketsIn() );

            this->setModelData( row, row->row(),
                                static_cast<int>( PlrCols::BytesIn ),
                                baudIn,
                                Qt::DisplayRole );

            this->setAvgBaud( this->getBytesOut(), true );

            baudOut = baudOut.arg( this->getAvgBaud( true ) )
                             .arg( this->getBytesOut() )
                             .arg( this->getPacketsOut() );
            this->setModelData( row, row->row(),
                                static_cast<int>( PlrCols::BytesOut ),
                                baudOut,
                                Qt::DisplayRole );

            //Color the User's IP address Green if the Admin is authed
            //Otherwise, color as Red.
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

            this->setModelData( row, row->row(),
                                static_cast<int>( PlrCols::SerNum ),
                                static_cast<int>( color ),
                                Qt::ForegroundRole, true );

            //Color the User's IP address Red if the User's is muted.
            //Otherwise, color as Green.
            color = Colors::Default;
            if ( !this->getNetworkMuted() )
            {
                if ( !this->getIsVisible() )
                    color = Colors::Invisible;
                else
                    color = Colors::Valid;
            }
            else
                color = Colors::Invalid;

            this->setModelData( row, row->row(),
                                static_cast<int>( PlrCols::IPPort ),
                                static_cast<int>( color ),
                                Qt::ForegroundRole, true );

            //Set the NPK/AFK icon.
            this->setModelData( row, row->row(),
                                static_cast<int>( PlrCols::SerNum ),
                                this->getAfkIcon(),
                                Qt::DecorationRole, false );
        }

        if ( Settings::getDisconnectIdles()
          && idleTime.elapsed() >= MAX_IDLE_TIME )
        {
            QString reason{ "Auto-Disconnect; Idle timeout: "
                            "[ %1 ], [ %2 ]" };
            reason = reason.arg( this->getSernum_s() )
                           .arg( this->getBioData() );

            Logger::getInstance()->insertLog( serverInfo->getName(), reason,
                                              LogTypes::DC, true, true );

            this->setDisconnected( true, DCTypes::IPDC );
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
                else if (( !Settings::getRequirePassword()
                        || this->getSvrPwdReceived() )
                       && !this->getNewAdminPwdRequested() )
                {
                    if ( !this->getAdminPwdReceived() )
                    {
                        QString msg{ "The server Admin requires all Remote "
                                     "Administrators to authenticate themselves "
                                     "with their password. Please enter your "
                                     "password with the command (/login *PASS) "
                                     "or be denied access to the server. "
                                     "Thank you!" };

                        if ( this->getIsAdmin() )
                        {
                            this->setAdminPwdRequested( true );
                            this->sendMessage( msg );
                        }
                    }
                }
            }
        }
    });

    QObject::connect( &killTimer, &QTimer::timeout, [this]()
    {
        QTcpSocket* soc{ this->getSocket() };
        if ( soc != nullptr )
        {
            if ( this->getIsDisconnected() )
            {
                //Gracefully Disconnect the User.
                soc->flush();
                soc->close();
            }
        }
    });
    floodTimer.start();
}

Player::~Player()
{
    if ( messageDialog != nullptr )
    {
        if ( messageDialog->isVisible() )
        {
            messageDialog->close();
        }
        messageDialog->disconnect();
        messageDialog->deleteLater();
    }

    connTimer.stop();
    connTimer.disconnect();

    killTimer.stop();
    killTimer.disconnect();

    this->disconnect();
    this->deleteLater();
}

void Player::sendMessage(const QString& msg, const bool& toAll)
{
    ServerInfo* server{ this->getServerInfo() };
    if ( server == nullptr )
        return;

    if ( !msg.isEmpty() )
    {
        if ( toAll )
            server->sendMasterMessage( msg, nullptr, true );
        else
            server->sendMasterMessage( msg, this, false );
    }
    else
    {
        if ( messageDialog == nullptr )
        {
            messageDialog = new SendMsg( this->getSernum_s() );
            QObject::connect( messageDialog, &SendMsg::forwardMessage,
                              messageDialog,
                              [=](QString message)
            {
                if ( !message.isEmpty() )
                {
                    bool sendToAll{ messageDialog->sendToAll() };
                    if ( server != nullptr )
                    {
                        if ( sendToAll )
                            server->sendMasterMessage( message, nullptr, true );
                        else
                            server->sendMasterMessage( message, this, false );
                    }
                }
            } );
        }

        if ( !messageDialog->isVisible() )
            messageDialog->show();
        else
            messageDialog->hide();
    }
}

quint64 Player::getConnTime() const
{
    return connTime;
}

QStandardItem* Player::getTableRow() const
{
    return tableRow;
}

void Player::setTableRow(QStandardItem* value)
{
    tableRow = value;
}

QTcpSocket* Player::getSocket() const
{
    return socket;
}

void Player::setSocket(QTcpSocket* value)
{
    socket = value;
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

void Player::setModelData(QStandardItem* model, const qint32& row,
                          const qint32& column, const QVariant& data,
                          const qint32& role, const bool& isColor)
{
    if ( model != nullptr )
    {
        QStandardItemModel* sModel = model->model();
        if ( model != nullptr )
        {
            if ( !isColor )
            {
                sModel->setData( model->model()->index( row, column ),
                                 data, role );
            }
            else
            {
                sModel->setData( model->model()->index( row, column ),
                                 Theme::getThemeColor(
                                     static_cast<Colors>( data.toInt() ) ),
                                 role );
            }
        }
    }
}

bool Player::getHasSernum() const
{
    return hasSernum;
}

void Player::setHasSernum(bool value)
{
    hasSernum = value;
}

quint32 Player::getSernum_i() const
{
    return sernum_i;
}

void Player::setSernum_i(quint32 value)
{
    if ( value != this->getSernum_i() )
    {
        sernum_i = value;
        QString sernum_s{ Helper::serNumToIntStr(
                               Helper::intToStr(
                                    value, 16, 8 ) ) };
        QString sernumHex_s{ Helper::serNumToHexStr( sernum_s ) };

        if ( !sernum_s.isEmpty() )
        {
            QStandardItem* row = this->getTableRow();
            if ( row != nullptr )
            {
                QStandardItemModel* model = row->model();
                if ( model != nullptr )
                {
                    model->setData( model->index( row->row(), 1 ),
                                    sernum_s,
                                    Qt::DisplayRole );

                    //Correct the User's BIO Data based on their serNum.
                    //This will only succeed if the User is on file.
                    bool newBioData{ false };
                    QByteArray data{ "" };

                    if ( this->getHasBioData()
                      && !Helper::strContainsStr( this->getBioData(),
                                                  sernum_s ) )
                    {
                        data = User::getBIOData( sernumHex_s );
                        if ( !sernum_s.isEmpty() )
                        {
                            if ( Helper::strContainsStr( QString( data ),
                                                         sernum_s ) )
                            {
                                newBioData = true;
                                this->setHasBioData( true );
                                this->setBioData( data );
                            }
                            else
                                this->setHasBioData( false );
                        }
                    }

                    if ( newBioData
                      && !data.isEmpty() )
                    {
                        model->setData( model->index( row->row(), 7 ),
                                        data,
                                        Qt::DisplayRole );
                    }
                }
            }
        }

        this->setHasSernum( true );
        this->setSernum_s( sernum_s );
        this->setSernumHex_s( sernumHex_s );
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

quint32 Player::getTargetScene() const
{
    return targetHost;
}

void Player::setTargetScene(quint32 value)
{
    targetHost = value;
}

quint32 Player::getSceneHost() const
{
    return sceneHost;
}

void Player::setSceneHost(quint32 value)
{
    sceneHost = value;
}

quint32 Player::getTargetSerNum() const
{
    return targetSerNum;
}

void Player::setTargetSerNum(quint32 value)
{
    targetSerNum = value;
}

int Player::getTargetType() const
{
    return targetType;
}

void Player::setTargetType(const int& value)
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

QString Player::getCampPacket() const
{
    return campPacket;
}

void Player::setCampPacket(const QString& value)
{
    campPacket = value;
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
    return hasBioData;
}

void Player::setHasBioData(bool value)
{
    hasBioData = value;
}

QByteArray Player::getOutBuff() const
{
    return outBuff;
}

void Player::setOutBuff(const QByteArray& value)
{
    outBuff = value;
}

bool Player::getSvrPwdRequested() const
{
    return svrPwdRequested;
}

void Player::setSvrPwdRequested(bool value)
{
    svrPwdRequested = value;
}

int Player::getSlotPos() const
{
    return slotPos;
}

void Player::setSlotPos(const int& value)
{
    slotPos = value;
}

QString Player::getPublicIP() const
{
    if ( !publicIP.isEmpty() )
        return publicIP;

    return QString( "" );
}

void Player::setPublicIP(const QString& value)
{
    publicIP = value;
}

quint32 Player::getPublicPort() const
{
    return publicPort;
}

void Player::setPublicPort(const quint32& value)
{
    publicPort = value;
}

bool Player::getSvrPwdReceived() const
{
    return svrPwdReceived;
}

void Player::setSvrPwdReceived(const bool& value)
{
    svrPwdReceived = value;
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
    this->setAvgBaud( bytesIn, false );
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
    this->setAvgBaud( bytesOut, true );
}

quint64 Player::getAvgBaud(const bool& out) const
{
    if ( out )
        return avgBaudOut;
    else
        return avgBaudIn;
}

void Player::setAvgBaud(const quint64& bytes, const bool& out)
{
    quint64 time = this->getConnTime();
    quint64 baud{ 0 };

    if ( bytes > 0 && time > 0 )
        baud = 10 * bytes / time;

    if ( baud > 0 )
    {
        if ( out )
            avgBaudOut = baud;
        else
            avgBaudIn = baud;
    }
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

bool Player::getNewAdminPwdRequested() const
{
    return newAdminPwdRequested;
}

void Player::setNewAdminPwdRequested(const bool& value)
{
    newAdminPwdRequested = value;
    if ( newAdminPwdRequested )
    {
        QString msg{ "The Server Host is attempting to register you as an "
                     "Admin with the server. Please reply to this message with "
                     "(/register *YOURPASS). Note: The server Host and other "
                     "Admins will not have access to this information." };

        if ( this->getIsAdmin() )
            this->sendMessage( msg, false );
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

bool Player::getIsDisconnected() const
{
    return isDisconnected;
}

void Player::setDisconnected(const bool& value, const DCTypes& dcType)
{
    isDisconnected = value;
    if ( isDisconnected )
    {
        auto* server = this->getServerInfo();
        if ( server != nullptr )
        {
            //Increment the disconnect count for the specific type.
            switch ( dcType )
            {
                case DCTypes::IPDC:
                default:
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

bool Player::getNetworkMuted() const
{
    return networkMuted;
}

void Player::setNetworkMuted(const bool& value, const QString& msg)
{
    if ( !msg.isEmpty() )
    {
        Logger::getInstance()->insertLog( serverInfo->getName(), msg,
                                          LogTypes::MUTE, true, true );
    }

    networkMuted = value;
}

void Player::chatPacketFound()
{
    this->setIsAFK( false );
    afkTimer.start( MAX_AFK_TIME );
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
        this->setAfkIcon( "NPK" );
}

QIcon Player::getAfkIcon() const
{
    return afkIcon;
}

void Player::setAfkIcon(const QString& value)
{
    afkIcon = QIcon( ":/icon/" + value + ".png" );
}

void Player::validateSerNum(ServerInfo* server, const quint32& id)
{
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
          || !Settings::getDCBlueCodedSerNums() )
        {
            if ( this->getSernum_i() == 0 )
            {
                //Disconnect the User if they have no SerNum,
                //as we require SerNums.
                if ( Settings::getReqSernums()
                  && id == 0 )
                {
                    zeroSerNum = true;
                    disconnect = true;
                }
                this->setSernum_i( id );
            }
            else if (( id > 0 && this->getSernum_i() != id )
                   && this->getSernum_i() > 0 )
            {
                //User's sernum has somehow changed. Disconnect them.
                //This is a possible Ban event.
                //TODO: Add Setting to enable banning.
                serNumChanged = true;
                disconnect = true;
            }
        }

        if ( disconnect == true
          || ( isBlueCoded == true
            && Settings::getDCBlueCodedSerNums() ) )
        {
            message = "";
            reason = "";
            if ( serNumChanged )
            {
                message = "Auto-Disconnect; SerNum Changed";
                reason = "%1: [ %2 ] to [ %3 ], [ %4 ]";
                reason = reason.arg( message )
                               .arg( this->getSernum_s() )
                               .arg( Helper::serNumToIntStr(
                                         Helper::intToStr( id, 16, 8 ) ) )
                               .arg( this->getBioData() );
            }
            else if ( zeroSerNum )
            {
                message = "Auto-Disconnect; Invalid SerNum";
                reason = "%1: [ %2 ], [ %3 ]";
                reason = reason.arg( message )
                               .arg( Helper::serNumToIntStr(
                                         Helper::intToStr( id, 16, 8 ) ) )
                               .arg( this->getBioData() );
            }
            else if ( isBlueCoded )
            {
                message = "Auto-Disconnect; BlueCoded SerNum";
                reason = "%1: [ %2 ], [ %3 ]";
                reason = reason.arg( message )
                               .arg( Helper::serNumToIntStr(
                                         Helper::intToStr( id, 16, 8 ) ) )
                               .arg( this->getBioData() );
            }

            this->sendMessage( message, false );

            Logger::getInstance()->insertLog( serverInfo->getName(), reason,
                                              LogTypes::DC, true, true );

            this->setDisconnected( true, DCTypes::IPDC );
        }
    }

    if ( id == 1 || this->getSernum_i() == 1 )
    {
        QString masterIP{ server->getMasterIP() };
        QString socketIP{ this->getPublicIP() };
        if ( !Helper::cmpStrings( masterIP, socketIP ) )
        {
            //Ban IP?
            reason = "Automatic Network Mute of <[ %1 ][ %2 ]> due to the "
                     "usage of <[ Soul 1 ][ %3 ]> while connecting from an "
                     "improper IP Address.";
            reason = reason.arg( this->getSernum_s() )
                           .arg( socketIP )
                           .arg( masterIP );
            this->setNetworkMuted( true, reason );
        }
    }
}

QString Player::getDVar() const
{
    return dVar;
}

void Player::setDVar(const QString& value)
{
    dVar = value;
}

QString Player::getWVar() const
{
    return wVar;
}

void Player::setWVar(const QString& value)
{
    wVar = value;
}
