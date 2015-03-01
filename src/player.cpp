
#include "includes.hpp"
#include "player.hpp"

Player::Player()
{
    //Update the User's UI row. --Every 1000MS.
    connTimer.start( 1000 );

    QObject::connect( &connTimer, &QTimer::timeout, [=]()
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

                this->setAvgBaudIn( this->getBytesIn() );
                model->setData( row->model()->index( row->row(), 5 ),
                                QString( "%1Bd, %2B, %3 Pkts" )
                                    .arg( this->getAvgBaudIn() )
                                    .arg( this->getBytesIn() )
                                    .arg( this->getPacketsIn() ),
                                Qt::DisplayRole );

                this->setAvgBaudOut( this->getBytesOut() );
                model->setData( row->model()->index( row->row(), 6 ),
                                QString( "%1Bd, %2B, %3 Pkts" )
                                    .arg( this->getAvgBaudOut() )
                                    .arg( this->getBytesOut() )
                                    .arg( this->getPacketsOut() ),
                                Qt::DisplayRole );

                //Color the User's IP address Green if the Admin is authed
                //Otherwise, color as Red.
                if ( this->getIsAdmin() )
                {
                    if ( this->getGotAuthPwd() )
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
            this->setSoftDisconnect( true );
        }

        //Authenticate Remote Admins as required.
        if ( Settings::getReqAdminAuth()
          && this->getIsAdmin() )
        {
            if ( this->getSernum_i() != 0
              && !this->getReqAuthPwd() )
            {
                if ( !Settings::getRequirePassword() || this->getEnteredPwd() )
                {
                    if ( !this->getGotAuthPwd() )
                        emit sendRemoteAdminPwdReqSignal( this );
                }
            }
        }
    });

    QObject::connect( &killTimer, &QTimer::timeout, [this]()
    {
        QTcpSocket* soc{ this->getSocket() };
        if ( soc != nullptr )
        {
            if ( this->getSoftDisconnect() )
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
    connTimer.stop();
    connTimer.disconnect();

    killTimer.stop();
    killTimer.disconnect();

    this->disconnect();
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
        this->setSoftDisconnect( true );
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

void Player::setTargetType(int value)
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

QByteArray Player::getBioData() const
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


bool Player::getPwdRequested() const
{
    return pwdRequested;
}

void Player::setPwdRequested(bool value)
{
    pwdRequested = value;
}

int Player::getSlotPos() const
{
    return slotPos;
}

void Player::setSlotPos(int value)
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

bool Player::getEnteredPwd() const
{
    return enteredPwd;
}

void Player::setEnteredPwd(bool value)
{
    enteredPwd = value;
}

quint64 Player::getFloodTime() const
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

void Player::setPacketFloodCount(int value)
{
    packetFloodCount = value;
}

int Player::getPacketsIn() const
{
    return packetsIn;
}

void Player::setPacketsIn(int value, int incr)
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
    this->setAvgBaudIn( bytesIn );
}

quint64 Player::getAvgBaudIn() const
{
    return avgBaudIn;
}

void Player::setAvgBaudIn(const quint64& bIn)
{
    quint64 time = this->getConnTime();
    quint64 baud{ 0 };

    if ( bIn > 0 && time > 0 )
        baud = 10 * bIn / time;

    if ( baud > 0 )
        avgBaudIn = baud;
}

int Player::getPacketsOut() const
{
    return packetsOut;
}

void Player::setPacketsOut(int value)
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

quint64 Player::getAvgBaudOut() const
{
    return avgBaudOut;
}

void Player::setAvgBaudOut(const quint64& bOut)
{
    quint64 time = this->getConnTime();
    quint64 baud{ 0 };

    if ( bOut > 0 && time > 0 )
        baud = 10 * bOut / time;

    if ( baud > 0 )
        avgBaudOut = baud;
}

void Player::resetAdminAuth()
{
    this->setReqAuthPwd( false );
    this->setGotAuthPwd( false );
    this->setReqNewAuthPwd( false );
    this->setGotNewAuthPwd( false );
}

bool Player::getReqAuthPwd() const
{
    return reqAuthPwd;
}

void Player::setReqAuthPwd(bool value)
{
    reqAuthPwd = value;
}

bool Player::getGotAuthPwd() const
{
    return gotAuthPwd;
}

void Player::setGotAuthPwd(bool value)
{
    gotAuthPwd = value;
}

bool Player::getIsAdmin()
{
    QString sernum{ this->getSernumHex_s() };
    return Admin::getIsRemoteAdmin( sernum );
}

qint32 Player::getAdminRank()
{
    QString sernum{ this->getSernumHex_s() };
    return Admin::getRemoteAdminRank( sernum );
}

qint32 Player::getCmdAttempts() const
{
    return cmdAttempts;
}

void Player::setCmdAttempts(const qint32& value)
{
    cmdAttempts = value;
}

bool Player::getReqNewAuthPwd() const
{
    return reqNewAuthPwd;
}

void Player::setReqNewAuthPwd(bool value)
{
    reqNewAuthPwd = value;
}

bool Player::getGotNewAuthPwd() const
{
    return gotNewAuthPwd;
}

void Player::setGotNewAuthPwd(bool value)
{
    gotNewAuthPwd = value;
}

bool Player::getSoftDisconnect() const
{
    return pendingDisconnect;
}

void Player::setSoftDisconnect(bool value)
{
    pendingDisconnect = value;
    if ( pendingDisconnect )
    {
        killTimer.start( 250 );
    }
}

bool Player::getNetworkMuted() const
{
    return networkMuted;
}

void Player::setNetworkMuted(bool value)
{
    networkMuted = value;
}

void Player::validateSerNum(ServerInfo* server, quint32 id)
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
                this->setSoftDisconnect( true );
            }
            this->setSernum_i( id );
        }
        else if (( id > 0 && this->getSernum_i() != id )
               && this->getSernum_i() > 0 )
        {
            //User's sernum has somehow changed. Disconnect them.
            //This is a possible Ban event.
            this->setSoftDisconnect( true );
        }
    }

    if ( this->getSoftDisconnect()
      && server != nullptr )
    {
        server->setIpDc( server->getIpDc() + 1 );
    }
}

quint32 Player::getDVar() const
{
    return dVar;
}

void Player::setDVar(const quint32& value)
{
    dVar = value;
}

quint32 Player::getWVar() const
{
    return wVar;
}

void Player::setWVar(const quint32& value)
{
    wVar = value;
}

#ifdef DECRYPT_PACKET_PLUGIN
QString Player::getGameInfo() const
{
    return gameInfo;
}

void Player::setGameInfo(const QString& value)
{
    gameInfo = value;
    if ( !gameInfo.isEmpty() )
        this->setHasGameInfo( true );
    else
        this->setHasGameInfo( false );
}

bool Player::getHasGameInfo() const
{
    return hasWorldInfo;
}

void Player::setHasGameInfo(bool value)
{
    hasWorldInfo = value;
}
#endif
