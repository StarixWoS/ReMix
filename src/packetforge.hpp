#ifndef PACKETFORGE_HPP
#define PACKETFORGE_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QLibrary>

class PacketForge
{
    static PacketForge* instance;
    bool initialized{ false };
    QLibrary pktDecrypt;

    //Typedefs for imported functions from our PacketDecrypt Library.
    //typedef QString (*Decrypt)(QByteArray);
    using Decrypt = QString(*)(QByteArray);

    //Function Pointers for imported functions from the PacketDecrypt Library.
    Decrypt decryptPkt{};

    public:
        PacketForge();
        ~PacketForge();

        static PacketForge* getInstance();

        //Wrappers for our imported functions.
        QString decryptPacket(const QByteArray& packet);

        bool validateSerNum(Player* plr, const QByteArray& packet);
};

#endif // PACKETFORGE_HPP
