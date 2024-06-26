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

    static QHash<QSharedPointer<Server>, WoSPacketHandler*> handlerInstanceMap;

    public:
        WoSPacketHandler(QSharedPointer<Server> server);
        ~WoSPacketHandler();

        static WoSPacketHandler* getInstance(QSharedPointer<Server> server);
        static void deleteInstance(QSharedPointer<Server> server);

        bool handlePacket(QSharedPointer<Server> server, ChatView* chatView, QByteArray& packet, QSharedPointer<Player> plr);
        void forgePacket(QSharedPointer<Server> server, QSharedPointer<Player> plr, const WoSPacketTypes& type);

    signals:
        void insertChatMsgSignal(const QString& msg, const Colors& color, const bool& newLine);
        void sendPacketToPlayerSignal(QSharedPointer<Player> plr, const qint32& targetType, const qint32& trgSerNum,
                                      const qint32& trgScene, const QByteArray& packet);
};

#endif // WOSPACKETHANDLER_HPP
