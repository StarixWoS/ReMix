
#ifndef RANDDEV_HPP
#define RANDDEV_HPP

#include <QDateTime>

//C++/11 Standard Headers
#include <random>

class RandDev
{
    std::mt19937 randDevice;
    bool deviceInitialized{ false };

    public:
        RandDev();
        ~RandDev();

        template<typename T>
        T genRandNum(T min, T max)
        {
            if ( !this->getInitialized() )
                this->initializeDevice();

            std::uniform_int_distribution<T> randInt( min, max );
            return randInt( randDevice );
        }

    private:
        bool getInitialized();
        void setInitialized(bool value);
        void initializeDevice();
};

#endif // RANDDEV_HPP

