
#ifndef W97PACKETHANDLER_HPP
#define W97PACKETHANDLER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSharedPointer>
#include <QObject>
#include <QTimer>

class W97PacketHandler : public QObject
{
    Q_OBJECT

    static QHash<QSharedPointer<Server>, W97PacketHandler*> pktHandleInstanceMap;

    QSharedPointer<Server> server;
    ChatView* chatView{ nullptr };

    QTimer masterCheckIn;

    public:
        W97PacketHandler(QSharedPointer<Server> svr, ChatView* chat);
        ~W97PacketHandler() override;

    private:

    public slots:

    signals:
};

#endif // W97PACKETHANDLER_HPP
