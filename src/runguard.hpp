#ifndef RUNGUARD_HPP
#define RUNGUARD_HPP

//Required Qt includes.
#include <QApplication>
#include <QtCore>
#include <QtGui>

class RunGuard : public QApplication
{
    Q_OBJECT

    QSharedMemory sharedMemory{ "ReMix_Game_Server_SharedMem" };
    bool isRunning{ false };

    public:
        explicit RunGuard(int& argc, char** argv);
        bool getIsRunning() const;
};

#endif //RUNGUARD_HPP
