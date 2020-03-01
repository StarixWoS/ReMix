
//Class includes.
#include "packetforge.hpp"

//ReMix includes.
#include "logger.hpp"
#include "player.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QDebug>

PacketForge* PacketForge::instance{ nullptr };
PacketForge::PacketForge() = default;
PacketForge::~PacketForge() = default;

PacketForge* PacketForge::getInstance()
{
    if ( instance == nullptr )
        instance = new PacketForge();

    return instance;
}

QString PacketForge::decryptPacket(const QByteArray& packet)
{
    QByteArray pkt{ packet };

    //Player positioning packets, return an empty string.
    if ( !Helper::strStartsWithStr( packet, ":SR?" )
      && !Helper::strStartsWithStr( packet, ":SR!" ) )
    {
        if ( pkt.startsWith( ":SR" ) )
        {
            pkt = pkt.left( pkt.length() - 2 ).mid( 6 );

            int chrKey{ 0x82381AC + 0x11 * pkt.length( ) };
            for ( auto&& chr : pkt )
            {
                chr = chr ^ ( chrKey & 0x1F );
                chrKey = 0xD * chrKey ^ 0x43B;
            }

            pkt = pkt.left( pkt.length() - 4 );
            return QString( pkt );
        }
    }
    return QString( "" );
}

bool PacketForge::validateSerNum(Player* plr, const QByteArray& packet)
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

    QString msg{ "Automatic Network Mute of <[ %1 ][ %2 ]> due to a SerNum Missmatch; Tried sending [ %3 ] as [ %4 ] while connected as [ %5 ]." };
            msg = msg.arg( plr->getSernum_s() )
                     .arg( plr->getPublicIP() )
                     .arg( QString( packet ) )  //Encrypted packet into the log file.
                     .arg( srcSerNum )
                     .arg( plr->getSernumHex_s() );

   Logger::getInstance()->insertLog( "PacketForge", msg, LogTypes::MUTE, true, true );

    plr->setNetworkMuted( true );

    return false;
}
