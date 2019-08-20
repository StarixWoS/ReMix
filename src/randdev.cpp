
//Class includes.
#include "randdev.hpp"

//Required STD includes.
#include <random>

//Required Qt includes.
#include <QDateTime>

<<<<<<< HEAD
=======
RandDev* RandDev::device{ nullptr };
std::mt19937_64 RandDev::randDevice;

>>>>>>> develop
RandDev::RandDev()
{
    this->initializeDevice();
}

RandDev::~RandDev(){}

bool RandDev::getInitialized()
{
    return deviceInitialized;
}

void RandDev::setInitialized(const bool& value)
{
    deviceInitialized = value;
}

<<<<<<< HEAD
void RandDev::initializeDevice()
{
    randDevice.seed( static_cast<uint>( QDateTime::currentMSecsSinceEpoch() ) );
    setInitialized( true );
=======
qint64 RandDev::getSeed()
{
    return QDateTime::currentMSecsSinceEpoch();
>>>>>>> develop
}
