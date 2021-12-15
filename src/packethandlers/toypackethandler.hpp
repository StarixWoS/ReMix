#ifndef TOYPACKETHANDLER_HPP
#define TOYPACKETHANDLER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSharedPointer>
#include <QObject>
#include <QTimer>

class ToYPacketHandler : public QObject
{
    Q_OBJECT

    public:
        ToYPacketHandler();
        ~ToYPacketHandler();

        static ToYPacketHandler* getInstance();
        bool handlePacket(QSharedPointer<Server> server, ChatView* chatView, const QByteArray& packet, QSharedPointer<Player> plr);

    signals:
        void insertChatMsgSignal(const QString& msg, const Colors& color, const bool& newLine);
};

#endif // TOYPACKETHANDLER_HPP
