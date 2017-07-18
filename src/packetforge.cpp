
#include "includes.hpp"
#include "packetforge.hpp"

PacketForge* PacketForge::forge{ nullptr };
PacketForge::PacketForge()
{
    if ( pktDecrypt.load() )
    {
        decryptPkt = (Decrypt)pktDecrypt.resolve( "decryptPacket" );
    }
    else
        qDebug() << pktDecrypt.errorString();
}

PacketForge::~PacketForge()
{
}

PacketForge* PacketForge::getInstance()
{
    if ( forge == nullptr )
    {
        forge = new PacketForge();
    }
    return forge;
}

QString PacketForge::decryptPacket(QString packet)
{
    if ( decryptPkt )
    {
        return decryptPkt( packet );
    }
    return QString( "" );
}
