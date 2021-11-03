#ifndef PACKETFORGE_HPP
#define PACKETFORGE_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QObject>

class PacketForge : public QObject
{
    Q_OBJECT

    static PacketForge* instance;

    public:
        PacketForge();
        ~PacketForge();

        static PacketForge* getInstance();

        //Wrappers for our imported functions.
        QString decryptPacket(const QByteArray& packet);

        bool validateSerNum(QSharedPointer<Player> plr, const QByteArray& packet);

    signals:
        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine) const;
};

#endif // PACKETFORGE_HPP
