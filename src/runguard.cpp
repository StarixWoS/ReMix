
//Class includes.
#include "runguard.hpp"

//ReMix includes.
#include "appeventfilter.hpp"

RunGuard::RunGuard(int& argc, char** argv)
    : QApplication( argc, argv )
{
#ifndef REMIX_DEVMODE
    // Create shared memory segment
    if ( sharedMemory.create( sizeof( quint32 ) ) == false )
    {
        // Segment already exists, meaning another instance of the application is running
        isRunning = true;
    }
    else
    {
        // Set the shared memory value to 0 to indicate that this instance is running
        sharedMemory.lock();
        quint32* sharedMemVal = static_cast<quint32*>( sharedMemory.data() );
        *sharedMemVal = 0;
        sharedMemory.unlock();
    }

#endif //REMIX_DEVMODE
    if ( !isRunning )
    {
        this->setApplicationName( QStringLiteral("ReMix") );
        this->setApplicationVersion( REMIX_VERSION );
        this->setQuitOnLastWindowClosed( false );
        this->installEventFilter( new AppEventFilter() );
    }
}

bool RunGuard::getIsRunning() const
{
    return isRunning;
}
