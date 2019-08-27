
//Class includes.
#include "packetforge.hpp"

//ReMix includes.
#include "logger.hpp"
#include "player.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QDebug>

PacketForge* PacketForge::instance{ nullptr };
PacketForge::PacketForge()
{
    QString message{ "Initializing PacketForge module." };
    Logger::getInstance()->insertLog( "PacketForge", message,
                                      LogTypes::PktForge, true, true );

    pktDecrypt.setFileName( "PacketForge.dll" );
    if ( pktDecrypt.load() )
    {
        decryptPkt = reinterpret_cast<Decrypt>(
                            pktDecrypt.resolve( "decryptPacket" ) );

        initialized = true;
        message =  "Initialized PacketForge module with method "
                   "[ decryptPacket ].";
        Logger::getInstance()->insertLog( "PacketForge", message,
                                          LogTypes::PktForge, true, true );
    }
    else
    {
        message = "Unable to initialize PacketForge; Error[ %1 ].";
        message = message.arg( pktDecrypt.errorString() );
        Logger::getInstance()->insertLog( "PacketForge", message,
                                          LogTypes::PktForge, true, true );
    }
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

QString PacketForge::decryptPacket(const QByteArray& packet)
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

    QString msg{ "Automatic Network Mute of <[ %1 ][ %2 ]> due to a "
                 "SerNum Missmatch; Tried sending [ %3 ] as "
                 "[ %4 ] while connected as [ %5 ]." };
            msg = msg.arg( plr->getSernum_s() )
                     .arg( plr->getPublicIP() )
                     .arg( QString( packet ) )  //Encrypted packet into the log file.
                     .arg( srcSerNum )
                     .arg( plr->getSernumHex_s() );

    plr->setNetworkMuted( true, msg );

    return false;
}
