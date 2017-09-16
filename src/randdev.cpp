
//Class includes.
#include "randdev.hpp"

//Required STD includes.
#include <random>

//Required Qt includes.
#include <QDateTime>

RandDev::RandDev()
{
    this->initializeDevice();
}

RandDev::~RandDev(){}

bool RandDev::getInitialized()
{
    return deviceInitialized;
}

void RandDev::setInitialized(bool value)
{
    deviceInitialized = value;
}

void RandDev::initializeDevice()
{
    randDevice.seed( static_cast<uint>( QDateTime::currentMSecsSinceEpoch() ) );
    setInitialized( true );
}
