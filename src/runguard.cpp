
//Class includes.
#include "runguard.hpp"

RunGuard::RunGuard(const QString& key)
{
    memLockKey = generateKeyHash( key, "_memLockKey" );
    sharedmemKey = generateKeyHash( key, "_sharedmemKey" );
    sharedMem = new QSharedMemory( sharedmemKey );
    memLock = new QSystemSemaphore( memLockKey, -1 );

    memLock->acquire();
    {
        QSharedMemory fix( sharedmemKey );
        fix.attach();
    }
    memLock->release();
}

RunGuard::~RunGuard()
{
    release();
}

bool RunGuard::isAnotherRunning() const
{
    if ( sharedMem->isAttached() )
        return false;

    memLock->acquire();
    const bool isRunning{ sharedMem->attach() };
    if ( isRunning )
        sharedMem->detach();

    memLock->release();

    return isRunning;
}

bool RunGuard::tryToRun()
{
    if ( isAnotherRunning() )
        return false;

    memLock->acquire();
    const bool result = sharedMem->create( sizeof( quint64 ) );
    memLock->release();

    if ( !result )
    {
        release();
        return false;
    }
    return true;
}

void RunGuard::release()
{
    memLock->acquire();
    if ( sharedMem->isAttached() )
        sharedMem->detach();

    memLock->release();

    sharedMem->deleteLater();
    delete memLock;
}

QString RunGuard::generateKeyHash(const QString& key, const QString& salt)
{
    QByteArray data;
    data.append( key.toUtf8() );
    data.append( salt.toUtf8() );
    data = QCryptographicHash::hash( data, QCryptographicHash::Sha1 )
                         .toHex();
    return data;
}
