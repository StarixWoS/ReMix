#ifndef UPNPUDPTHREAD_HPP
#define UPNPUDPTHREAD_HPP

#include <QThread>
#include <QObject>

class UPnPUDPThread : public QThread
{
        Q_OBJECT
    public:
        explicit UPnPUDPThread(QObject *parent = nullptr);
};

#endif // UPNPUDPTHREAD_HPP
