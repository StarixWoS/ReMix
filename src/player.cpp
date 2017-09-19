
//Class includes.
#include "player.hpp"

//ReMix includes.
#include "serverinfo.hpp"
#include "settings.hpp"
#include "sendmsg.hpp"
#include "helper.hpp"
#include "user.hpp"

//Qt Includes.
#include <QStandardItemModel>
#include <QApplication>
#include <QTcpSocket>

Player::Player()
{
    //Update the User's UI row. --Every 1000MS.
    connTimer.start( 1000 );

    QObject::connect( &connTimer, &QTimer::timeout, &connTimer,
    [=]()
    {
        ++connTime;

        QStandardItem* row = this->getTableRow();
        if ( row != nullptr )
        {
            QStandardItemModel* model = row->model();
            if ( model != nullptr )
            {
                model->setData( row->model()->index( row->row(), 4 ),
                                QString( "%1:%2:%3" )
                                    .arg( connTime / 3600, 2, 10, QChar( '0' ) )
                                    .arg(( connTime / 60 ) % 60, 2,
                                         10, QChar( '0' ) )
                                    .arg( connTime % 60, 2, 10, QChar( '0' ) ),
                                Qt::DisplayRole );

                this->setAvgBaud( this->getBytesIn(), false );
                model->setData( row->model()->index( row->row(), 5 ),
                                QString( "%1Bd, %2B, %3 Pkts" )
                                    .arg( this->getAvgBaud( false ) )
                                    .arg( this->getBytesIn() )
                                    .arg( this->getPacketsIn() ),
                                Qt::DisplayRole );

                this->setAvgBaud( this->getBytesOut(), true );
                model->setData( row->model()->index( row->row(), 6 ),
                                QString( "%1Bd, %2B, %3 Pkts" )
                                    .arg( this->getAvgBaud( true ) )
                                    .arg( this->getBytesOut() )
                                    .arg( this->getPacketsOut() ),
                                Qt::DisplayRole );

                //Color the User's IP address Green if the Admin is authed
                //Otherwise, color as Red.
                if ( this->getIsAdmin() )
                {
                    if ( this->getAdminPwdReceived() )
                    {
                        model->setData( row->model()->index( row->row(), 1 ),
                                        QBrush( QColor( "limegreen" ) ),
                                        Qt::ForegroundRole );
                    }
                    else
                        model->setData( row->model()->index( row->row(), 1 ),
                                        QBrush( QColor( "red" ) ),
                                        Qt::ForegroundRole );
                }
                else
                {
                    model->setData( row->model()->index( row->row(), 1 ),
                                    qApp->palette().windowText(),
                                    Qt::ForegroundRole );
                }

                //Color the User's IP address Red if the User's is muted.
                //Otherwise, color as Green.
                if ( this->getNetworkMuted() )
                {
                    model->setData( row->model()->index( row->row(), 0 ),
                                    QBrush( QColor( "red" ) ),
                                    Qt::ForegroundRole );
                }
                else
                {
                    model->setData( row->model()->index( row->row(), 0 ),
                                    QBrush( QColor( "limegreen" ) ),
                                    Qt::ForegroundRole );
                }
            }
        }

        if ( Settings::getDisconnectIdles()
          && idleTime.elapsed() >= MAX_IDLE_TIME )
        {
            this->setDisconnected( true );
        }

        //Authenticate Remote Admins as required.
        if ( Settings::getReqAdminAuth()
          && this->getIsAdmin() )
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
                        emit newAdminPwdRequestedSignal( this );
                }
            }
        }
    });

    QObject::connect( &killTimer, &QTimer::timeout, [this]()
    {
        QTcpSocket* soc{ this->getSocket() };
        if ( soc != nullptr )
        {
            if ( this->getDisconnected() )
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

qint64 Player::getConnTime() const
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

quint32 Player::getSernum_i() const
{
    return sernum_i;
}

void Player::setSernum_i(quint32 value)
{
    //The User has no serNum, and we require a serNum;
    //forcibly remove the User from the server.
    if ( Settings::getReqSernums() && value == 0 )
    {
        this->setDisconnected( true );
        return;
    }

    hasSernum = true;
    if ( value != sernum_i )
    {
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
                }
            }
        }

        this->setSernum_s( sernum_s );
        this->setSernumHex_s( sernumHex_s );

        sernum_i = value;
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

QString Player::getAlias() const
{
    return alias;
}

void Player::setAlias(const QString& value)
{
    alias = value;
}

QString Player::getBioData() const
{
    return bioData;
}

void Player::setBioData(const QByteArray& value)
{
    bioData = value;
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

qint64 Player::getBytesIn() const
{
    return bytesIn;
}

void Player::setBytesIn(const qint64& value)
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

qint64 Player::getBytesOut() const
{
    return bytesOut;
}

void Player::setBytesOut(const qint64& value)
{
    bytesOut = value;
    this->setAvgBaud( bytesOut, true );
}

qint64 Player::getAvgBaud(const bool& out) const
{
    if ( out )
        return avgBaudOut;
    else
        return avgBaudIn;
}

void Player::setAvgBaud(const qint64& bytes, const bool& out)
{
    qint64 time = this->getConnTime();
    qint64 baud{ 0 };

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

bool Player::getIsAdmin()
{
    QString sernum{ this->getSernumHex_s() };
    return User::getIsAdmin( sernum );
}

qint32 Player::getAdminRank()
{
    QString sernum{ this->getSernumHex_s() };
    return User::getAdminRank( sernum );
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
        emit newRemoteAdminRegisterSignal( this );
}

bool Player::getNewAdminPwdReceived() const
{
    return newAdminPwdReceived;
}

void Player::setNewAdminPwdReceived(const bool& value)
{
    newAdminPwdReceived = value;
}

bool Player::getDisconnected() const
{
    return pendingDisconnect;
}

void Player::setDisconnected(const bool& value)
{
    pendingDisconnect = value;
    if ( pendingDisconnect )
    {
        killTimer.start( 250 );
    }
    else  //The User is no longer being disconnected. Kill the timer.
    {
        if ( killTimer.isActive() )
            killTimer.stop();
    }
}

bool Player::getNetworkMuted() const
{
    return networkMuted;
}

void Player::setNetworkMuted(const bool& value, const QString& msg)
{
    if ( Settings::getLogFiles() )
    {
        QString log{ "logs/muteLog.txt" };
        if ( !msg.isEmpty() )
        {
            Helper::logToFile( log, msg, true, true );
        }
    }
    networkMuted = value;
}

void Player::validateSerNum(ServerInfo* server, const quint32& id)
{
    if (( this->getSernum_i() != id
       && id > 0 )
      || this->getSernum_i() == 0 )
    {
        if ( this->getSernum_i() == 0 )
        {
            //Disconnect the User if they have no SerNum, as we require SerNums.
            if ( Settings::getReqSernums()
              && id == 0 )
            {
                this->setDisconnected( true );
            }
            this->setSernum_i( id );
        }
        else if (( id > 0 && this->getSernum_i() != id )
               && this->getSernum_i() > 0 )
        {
            //User's sernum has somehow changed. Disconnect them.
            //This is a possible Ban event.
            //TODO: Add Setting to enable banning.
            this->setDisconnected( true );
        }
    }

    if ( id == 1 || this->getSernum_i() == 1 )
    {

        QString masterIP{ server->getMasterIP() };
        QString socketIP{ this->getPublicIP() };
        if ( !Helper::cmpStrings( masterIP, socketIP ) )
        {
            //Ban IP?
            QString msg{ "Automatic Network Mute of <[ %1 ][ %2 ]> due to the "
                         "usage of <[ Soul 1 ][ %1 ]> while connecting from an "
                         "improper IP Address." };
                    msg = msg.arg( this->getSernum_s() )
                             .arg( socketIP )
                             .arg( masterIP );
            this->setNetworkMuted( true, msg );
        }
    }

    if ( this->getDisconnected()
      && server != nullptr )
    {
        server->setIpDc( server->getIpDc() + 1 );
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
