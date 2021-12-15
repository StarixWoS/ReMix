#ifndef WOSPACKETHANDLER_HPP
#define WOSPACKETHANDLER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSharedPointer>
#include <QObject>
#include <QTimer>

class WoSPacketHandler : public QObject
{
    Q_OBJECT

    public:
        WoSPacketHandler(){};
        ~WoSPacketHandler();

        static WoSPacketHandler* getInstance();
        bool handlePacket(QSharedPointer<Server> server, ChatView* chatView, const QByteArray& packet, QSharedPointer<Player> plr);

    signals:
        void insertChatMsgSignal(const QString& msg, const Colors& color, const bool& newLine);
};

#endif // WOSPACKETHANDLER_HPP
