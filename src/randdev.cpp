
#include "includes.hpp"
#include "randdev.hpp"

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
    randDevice.seed( QDateTime::currentMSecsSinceEpoch() );
    setInitialized( true );
}
