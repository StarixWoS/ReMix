
#ifndef RANDDEV_HPP
#define RANDDEV_HPP

#include "prototypes.hpp"

//C++/11 Standard Headers
#include <random>

class RandDev
{
    static RandDev* device;
    static std::mt19937 randDevice;

    public:
        RandDev();
        ~RandDev();

        static RandDev* getDevice();
        static unsigned int getSeed();

        template<typename T>
        T genRandNum(T min, T max)
        {
            //Re-Seed the device on every use.
            randDevice.seed( getSeed() );

            std::uniform_int_distribution<T> randInt( min, max );
            return randInt( randDevice );
        }
};

#endif // RANDDEV_HPP
