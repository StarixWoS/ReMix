
#include "player.hpp"
#include "helper.hpp"

Player::Player()
{
    this->startConnectionTime();
    this->startLastPacketTime();
}

qint64 Player::getConnectionTime() const
{
    return connectionTime.elapsed();
}

void Player::startConnectionTime()
{
    connectionTime.restart();
}

qint64 Player::getLastPacketTime() const
{
    return lastPacketTime.elapsed();
}

void Player::startLastPacketTime()
{
    lastPacketTime.restart();;
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

quint32 Player::getSernum() const
{
    return sernum;
}

void Player::setSernum(quint32 value)
{
    if ( Helper::getReqSernums() && value <= 0 )
    {
        this->getSocket()->abort();
        return;
    }
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
    return publicIP;
}

void Player::setPublicIP(const QString& value)
{
    publicIP = value;
}

bool Player::getEnteredPwd() const
{
    return enteredPwd;
}

void Player::setEnteredPwd(bool value)
{
    enteredPwd = value;
}

int Player::getPacketsIn() const
{
    return packetsIn;
}

void Player::setPacketsIn(int value)
{
    packetsIn = value;
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


bool Player::getAdminPwdRequested() const
{
    return adminPwdRequested;
}

void Player::setAdminPwdRequested(bool value)
{
    adminPwdRequested = value;
}

bool Player::getAdminPwdEntered() const
{
    return adminPwdEntered;
}

void Player::setAdminPwdEntered(bool value)
{
    adminPwdEntered = value;
}

int Player::getAdminRank() const
{
    return adminRank;
}

void Player::setAdminRank(int value)
{
    adminRank = value;
}
