#ifndef PACKETFORGE_HPP
#define PACKETFORGE_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QObject>

class PacketForge : public QObject
{
    Q_OBJECT

    static PacketForge* instance;
    const int bitKeys[ 16 ] = { 0x0B, 0x11, 0x03, 0x65, 0x07, 0x138, 0x0C, 0x0D,
                                0x13, 0x17, 0x0F, 0x1B, 0x12, 0x1F, 0x04, 0x1D, };
    public:
        PacketForge();
        ~PacketForge() = default;

        static PacketForge* getInstance();

        //Wrappers for our imported functions.
        QString decryptPacket(const QByteArray& packet);
        QByteArray encryptPacket(const QByteArray& packet, const qint32& plrSlot, const Games& game);

        QByteArray distortStr(const QByteArray& packet);

        bool validateSerNum(QSharedPointer<Player> plr, const QByteArray& packet);

        qint32 calcPacketChkSum(const QByteArray& packet, const Games& game);

        qint32 calcDistortedChkSum(const QByteArray& packet);

    signals:
        void insertLogSignal(const QString& source, const QString& message, const LKeys& type, const bool& logToFile, const bool& newLine) const;
};

#endif // PACKETFORGE_HPP
