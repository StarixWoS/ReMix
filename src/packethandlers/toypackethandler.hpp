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

    static QHash<QSharedPointer<Server>, ToYPacketHandler*> handlerInstanceMap;

    public:
        ToYPacketHandler(QSharedPointer<Server> server);
        ~ToYPacketHandler();

        static ToYPacketHandler* getInstance(QSharedPointer<Server> server);
        static void deleteInstance(QSharedPointer<Server> server);

        bool handlePacket(QSharedPointer<Server> server, ChatView* chatView, const QByteArray& packet, QSharedPointer<Player> plr);

    signals:
        void insertChatMsgSignal(const QString& msg, const Colors& color, const bool& newLine);
};

#endif // TOYPACKETHANDLER_HPP
