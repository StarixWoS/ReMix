
//Class includes.
#include "randdev.hpp"

//Required Qt includes.
#include <QRandomGenerator64>
#include <QRandomGenerator>
#include <QDateTime>

QRandomGenerator64 RandDev::genInt64;
QRandomGenerator RandDev::genInt;

RandDev RandDev::instance;
RandDev::RandDev()
{
    genInt64 = QRandomGenerator64::securelySeeded();
    genInt = QRandomGenerator::securelySeeded();
}

RandDev::~RandDev(){}

RandDev& RandDev::getInstance()
{
    return instance;
}
