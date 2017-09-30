
//Class includes.
#include "packetforge.hpp"

//ReMix includes.
#include "player.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QDebug>

PacketForge* PacketForge::instance{ nullptr };
PacketForge::PacketForge()
{
    pktDecrypt.setFileName( "PacketForge.dll" );
    if ( pktDecrypt.load() )
    {
        decryptPkt = reinterpret_cast<Decrypt>(
                            pktDecrypt.resolve( "decryptPacket" ) );
        initialized = true;
    }
    else
        qDebug() << pktDecrypt.errorString();
}

PacketForge::~PacketForge()
{
}

PacketForge* PacketForge::getInstance()
{
    if ( instance == nullptr )
        instance = new PacketForge();

    return instance;
}

QString PacketForge::decryptPacket(const QString& packet)
{
    //Player positioning packets, return an empty string.
    if ( !Helper::strStartsWithStr( packet, ":SR?" )
      && !Helper::strStartsWithStr( packet, ":SR!" ) )
    {
        if ( initialized )
        {
            return decryptPkt( packet );
        }
    }
    return QString( "" );
}

bool PacketForge::validateSerNum(Player* plr, const QString& packet)
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

    if ( Helper::cmpStrings( srcSerNum, plr->getSernumHex_s() ) )
        return true;

    QString msg{ "Automatic Network Mute of [ %1 ] due to a "
                 "SerNum Missmatch; Tried sending [ %2 ] as "
                 "[ %3 ] while connected as [ %4 ]." };
            msg = msg.arg( plr->getSernum_s(),
                           pkt,
                           srcSerNum,
                           plr->getSernumHex_s() );
    plr->setNetworkMuted( true, msg );

    return false;
}
