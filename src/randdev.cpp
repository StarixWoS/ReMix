
//Class includes.
#include "randdev.hpp"

//Required STD includes.
#include <random>

//Required Qt includes.
#include <QDateTime>

RandDev* RandDev::device{ nullptr };
std::mt19937_64 RandDev::randDevice;

RandDev::RandDev()
{
    randDevice.seed( this->getSeed() );
}

RandDev::~RandDev(){}

RandDev* RandDev::getDevice()
{
    if ( device == nullptr )
        device = new RandDev();

    return device;
}

qint64 RandDev::getSeed()
{
    return QDateTime::currentMSecsSinceEpoch();
}
