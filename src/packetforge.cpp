
#include "includes.hpp"
#include "packetforge.hpp"

PacketForge* PacketForge::forge{ nullptr };
PacketForge::PacketForge()
{
    pktDecrypt.setFileName( "PacketForge.dll" );
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
    //Player positioning packets, return an empty string.
    if ( packet.startsWith( ":SR?" ) || packet.startsWith( ":SR!" ) )
        return QString( "" );

    if ( decryptPkt )
    {
        return decryptPkt( packet );
    }
    return QString( "" );
}

bool PacketForge::validateSerNum(Player* plr, QString packet)
{
    QString pkt{ this->decryptPacket( packet ) };

    //The packet is empty or is a positioning packet,
    //mark as a valid comparison.
    if ( pkt.isEmpty() )
        return true;

    QString srcSerNum = pkt.left( 12 ).mid( 4 );
    //Unable to extract a SerNum from the incoming packet,
    //mark as valid.
    if ( srcSerNum.isEmpty() )
        return true;

    if ( srcSerNum.compare( plr->getSernumHex_s(), Qt::CaseInsensitive ) == 0 )
        return true;

    QString msg{ "Automatic Network Mute of [ %1 ] due to a "
                 "SerNum Missmatch; Tried sending [ %2 ] as "
                 "[ %3 ] while connected as [ %4 ]." };
            msg = msg.arg( plr->getSernum_s() )
                     .arg( pkt )
                     .arg( srcSerNum )
                     .arg( plr->getSernumHex_s() );
    plr->setNetworkMuted( true, msg );

    return false;
}
