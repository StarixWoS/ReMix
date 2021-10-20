
#ifndef RANDDEV_HPP
#define RANDDEV_HPP

#include "prototypes.hpp"

//Qt Includes.
#include <QtCore>

//C++/11 Standard Headers
#include <random>

class RandDev
{
    static QRandomGenerator64 genInt64;
    static QRandomGenerator genInt;
    static RandDev instance;

    public:
        RandDev();
        ~RandDev();

        static RandDev& getInstance();

        template<typename T>
        static T getGen64(T min, T max)
        {
            return genInt64.bounded( min, max );
        }

        template<typename T>
        static T getGen(T min, T max)
        {
            return genInt.bounded( min, max );
        }
};

#endif // RANDDEV_HPP
