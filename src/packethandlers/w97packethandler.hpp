#ifndef W97PACKETHANDLER_HPP
#define W97PACKETHANDLER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSharedPointer>
#include <QObject>
#include <QTimer>

class W97PacketHandler
{
    public:
        W97PacketHandler();
        ~W97PacketHandler();

        static W97PacketHandler* getInstance();
        bool handlePacket(QSharedPointer<Server> server, ChatView* chatView, const QByteArray& packet, QSharedPointer<Player> plr);
};

#endif // W97PACKETHANDLER_HPP
