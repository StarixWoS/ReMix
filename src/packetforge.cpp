
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

bool PacketForge::validateSerNum(QString packet, QString sernum)
{
    QString pkt{ this->decryptPacket( packet ) };

    //The packet is empty, mark as a valid comparison.
    if ( pkt.isEmpty() )
        return true;

    QString srcSerNum = pkt.left( 12 ).mid( 4 );
    if ( srcSerNum.compare( sernum, Qt::CaseInsensitive ) == 0 )
        return true;

    return false;
}
