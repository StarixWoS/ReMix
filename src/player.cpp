
#include "player.hpp"

Player::Player(QObject *parent) :
    QObject(parent)
{
    connectionTime.start();
}

Player::~Player()
{

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
