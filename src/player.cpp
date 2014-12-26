
#include "includes.hpp"

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
                                    .arg(( connTime / 60 ) % 60, 2, 10, QChar( '0' ) )
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
            }
        }

        //Disconnect Idle Users after 10 minutes (600000 MS).
        if ( Helper::getDisconnectIdles()
          && idleTime.elapsed() >= 600000 )
        {
            this->setHardDisconnect( true );
        }

        //Authenticate Remote Admins as required.
        if ( this->getSernum() != 0
          && this->getReqAuthPwd() )
        {
            QString sernum{ this->getSernum_s() };
            if ( this->getEnteredPwd() )
            {
                if ( !this->getGotAuthPwd() )
                    emit sendRemoteAdminPwdReqSignal( this, sernum );
            }
        }
    });

    //The Timer is initialized within the force-disconnect function to prevent unnecessary checks.
    QObject::connect( &hardKillTimer, &QTimer::timeout, [=]()
    {
        //Disconnect Users with pending (forced) disconnections. --Every 5MS
        //This is meant to disconnect the User in an ungraceful manner (aborting the connection itself)
        //in the event the server has detected something that could cause major issues for
        //the network or the actual experience of other Users. (e.g. packet flooding)

        if ( this->getHardDisconnect() )
            this->getSocket()->abort();
    });

    //The Timer is initialized within the soft-disconnect function to prevent unnecessary checks.
    QObject::connect( &softKillTimer, &QTimer::timeout, [=]()
    {
        //This is meant to disconnect the User in a graceful manner (flushing the output buffers)
        //in the event the User has broken a rule or was manually disconnected with a reason being sent to them.
        if ( this->getSoftDisconnect() )
        {
            this->getSocket()->flush();
            this->getSocket()->close();
        }
    });

    floodTimer.start();
}

Player::~Player()
{
    connTimer.stop();
    connTimer.disconnect();

    hardKillTimer.stop();
    hardKillTimer.disconnect();

    softKillTimer.stop();
    softKillTimer.disconnect();

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

qint32 Player::getSernum() const
{
    return sernum;
}

void Player::setSernum(qint32 value)
{
    //The User has no serNum, and we require a serNum;
    //forcibly remove the User from the server.
    if ( Helper::getReqSernums() && value <= 0 )
    {
        this->setHardDisconnect( true );
        return;
    }
    hasSernum = true;
    sernum = value;
}

QString Player::getSernum_s() const
{
    return sernum_s;
}

void Player::setSernum_s(const QString& value)
{
    sernum_s = value;
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

    QString sernum = this->getSernum_s();
    if ( gotAuthPwd )
        this->setAdminRank( AdminHelper::getRemoteAdminRank( sernum ) );
}

qint32 Player::getAdminRank() const
{
    return adminRank;
}

void Player::setAdminRank(const qint32& value)
{
    adminRank = value;
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

bool Player::getHardDisconnect() const
{
    return pendingHardDisconnect;
}

void Player::setHardDisconnect(bool value)
{
    pendingHardDisconnect = value;
    if ( pendingHardDisconnect )
    {
        hardKillTimer.start( 5 );
    }
}

bool Player::getSoftDisconnect() const
{
    return pendingSoftDisconnect;
}

void Player::setSoftDisconnect(bool value)
{
    pendingSoftDisconnect = value;
    if ( pendingSoftDisconnect )
    {
        softKillTimer.start( 250 );
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
