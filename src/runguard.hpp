#ifndef RUNGUARD_HPP
#define RUNGUARD_HPP

//Required Qt includes.
#include <QCryptographicHash>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QObject>

class RunGuard
{
    QString sharedmemKey;
    QString memLockKey;

    QSystemSemaphore* memLock{ nullptr };
    QSharedMemory* sharedMem{ nullptr };

    public:
        RunGuard(const QString& key);
        ~RunGuard();

        bool isAnotherRunning();
        bool tryToRun();
        void release();

        static QString generateKeyHash(const QString& key, const QString& salt);

    private:

        Q_DISABLE_COPY( RunGuard )
};

#endif // RUNGUARD_HPP
