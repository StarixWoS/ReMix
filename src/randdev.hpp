
#ifndef RANDDEV_HPP
#define RANDDEV_HPP

#include "prototypes.hpp"

//Qt Includes.
#include <QtCore>

//C++/11 Standard Headers
#include <random>

class RandDev
{
<<<<<<< HEAD
    std::mt19937 randDevice;
    bool deviceInitialized{ false };
=======
    static RandDev* device;
    static std::mt19937_64 randDevice;
>>>>>>> develop

    public:
        RandDev();
        ~RandDev();

<<<<<<< HEAD
        template<typename T>
        T genRandNum(T min, T max)
        {
            if ( !this->getInitialized() )
                this->initializeDevice();

=======
        static RandDev* getDevice();
        static qint64 getSeed();

        template<typename T>
        T genRandNum(T min, T max)
        {
>>>>>>> develop
            std::uniform_int_distribution<T> randInt( min, max );
            return randInt( randDevice );
        }

    private:
        bool getInitialized();
        void setInitialized(const bool& value);
        void initializeDevice();
};

#endif // RANDDEV_HPP
