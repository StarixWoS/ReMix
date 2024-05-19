#ifndef WOSPACKETFORGE_HPP
#define WOSPACKETFORGE_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSharedPointer>
#include <QObject>
#include <QTimer>

class WoSPacketForge : QObject
{
    Q_OBJECT

    QTcpSocket* tcpSocket{ nullptr };

    public:
        WoSPacketForge();
        void sendPacketPrep(char opCode, QString srcSerNum, QString trgSerNum, QString message, QString subCode);
        void sendMixPacket(QString opCode, QString serNum, QString msg);
};

#endif // WOSPACKETFORGE_HPP
