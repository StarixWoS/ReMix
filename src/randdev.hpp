
#ifndef RANDDEV_HPP
#define RANDDEV_HPP

#include "prototypes.hpp"

//Qt Includes.
#include <QtCore>

//C++/11 Standard Headers
#include <random>

class RandDev
{
    static RandDev* device;
    static std::mt19937_64 randDevice;

    public:
        RandDev();
        ~RandDev();

        static RandDev* getDevice();
        static qint64 getSeed();

        template<typename T>
        T genRandNum(T min, T max)
        {
            std::uniform_int_distribution<T> randInt( min, max );
            return randInt( randDevice );
        }
};

#endif // RANDDEV_HPP
