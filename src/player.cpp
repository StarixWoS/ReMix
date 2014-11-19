
#include "player.hpp"


unsigned int Player::getHBID() const
{
    return hbID;
}

void Player::setHBID(unsigned int value)
{
    hbID = value;
}

int Player::getHbSlot() const
{
    return hbSlot;
}

void Player::setHBSlot(int value)
{
    hbSlot = value;
}
Player::Player()
{
    connectionTime.start();
}

QTcpSocket* Player::getSocket() const
{
    return socket;
}

void Player::setSocket(QTcpSocket* value)
{
    socket = value;
}

unsigned int Player::getSernum() const
{
    return sernum;
}

void Player::setSernum(unsigned int value)
{
    sernum = value;
}

unsigned int Player::getPrivateDest() const
{
    return privateDest;
}

void Player::setPrivateDest(unsigned int value)
{
    privateDest = value;
}

unsigned int Player::getSendDest() const
{
    return sendDest;
}

void Player::setSendDest(unsigned int value)
{
    sendDest = value;
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
