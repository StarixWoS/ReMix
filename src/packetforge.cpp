
//Class includes.
#include "packetforge.hpp"

//ReMix includes.
#include "server.hpp"
#include "logger.hpp"
#include "player.hpp"
#include "helper.hpp"
#include "user.hpp"

//Qt Includes.
#include <QObject>
#include <QtCore>
#include <QDebug>

PacketForge* PacketForge::instance{ nullptr };
PacketForge::PacketForge()
{
    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &PacketForge::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );
}

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
            //Allow decryption outside of the PacketForge library.
            pkt = pkt.left( pkt.length() - 2 ).mid( 6 );
            pkt = this->distortStr( pkt );
            pkt = pkt.left( pkt.length() - 4 );

            return QString( pkt );
        }
    }
    return QString( "" );
}

QByteArray PacketForge::encryptPacket(const QByteArray& packet, const qint32& plrSlot, const Games& game)
{
    QByteArray pkt{ packet };
    QString chkSum{ "" };
    if ( !packet.isEmpty() )
    {
        qint32 checksum{ this->calcPacketChkSum( pkt, game ) };
        if ( game == Games::WoS )
            chkSum = Helper::intToStr( checksum, IntBase::HEX, IntFills::Word );
        else if ( game == Games::ToY )
            chkSum = Helper::intToStr( checksum, IntBase::HEX, IntFills::DblWord );

        pkt += chkSum.toLatin1();
        pkt = "SR1" % Helper::intToStr( plrSlot, IntBase::HEX, IntFills::Byte ).toLatin1() % this->distortStr( pkt );
        pkt += Helper::intToStr( this->calcDistortedChkSum( pkt ), IntBase::HEX, IntFills::Byte ).toLatin1();
        pkt = ":" % pkt % "\r";
    }
    return pkt;
}

QByteArray PacketForge::distortStr(const QByteArray& packet)
{
    QByteArray pkt{ packet };

    qint32 chrKey{ static_cast<int>( 0x82381AC + 0x11 * pkt.length( ) ) };
    for ( auto&& chr : pkt )
    {
        chr = chr ^ ( chrKey & 0x1F );
        chrKey = 0xD * chrKey ^ 0x43B;
    }
    return pkt;
}

qint32 PacketForge::calcPacketChkSum(const QByteArray& packet, const Games& game)
{
    //WoS CheckSum.
    qint32 chkSum{ 0xD1E7 };

    //Swap to Arcadia's CheckSum.
    if ( game == Games::ToY )
        chkSum =  0x4D87D1E7;

    for ( int i = 0; i < static_cast<int>( packet.length() ); ++i )
    {
        char chr = packet[ i ] & 0x7F;
        qint32 chrKey = ( chr << i ) + ( ( chkSum * chr ) ^ chkSum );
        chkSum = chrKey - ( chrKey % 0x64 + bitKeys[ i & 0xF ] );
    }
    return ( chkSum & 0x7FFFFFFF ) % 0xFF2F;
}

qint32 PacketForge::calcDistortedChkSum(const QByteArray& packet)
{
    int chkSum{ 0 };
    for ( int i = 0; i < static_cast<int>( packet.length() ); ++i )
    {
        chkSum += packet[ i ];
    }
    return ( chkSum & 0xFF );
}

bool PacketForge::validateSerNum(QSharedPointer<Player> plr, const QByteArray& packet)
{
    QString pkt{ this->decryptPacket( packet ) };

    //The packet is empty or is a positioning packet, mark as a valid comparison.
    if ( pkt.isEmpty() )
        return true;

    QString srcSerNum{ pkt.left( 12 ).mid( 4 ) };

    //Unable to extract a SerNum from the incoming packet, mark as valid.
    if ( srcSerNum.isEmpty() )
        return true;

    if ( Helper::cmpStrings( srcSerNum, plr->getSernumHex_s() ) )
        return true;

    QString message{ "Auto-Mute; SerNum Missmatch; Tried sending a packet as [ %1 ] while connected as [ %2 ]." };
            message = message.arg( Helper::serNumToIntStr( srcSerNum, true ) )
                             .arg( plr->getSernum_s() );
    plr->getServer()->sendMasterMessage( message, plr, false );

    QString msg{ "Automatic Network Mute of <[ %1 ][ %2 ]> due to a SerNum Missmatch; Tried sending [ %3 ] as [ %4 ] while connected as [ %5 ]." };
            msg = msg.arg( plr->getSernum_s() )
                     .arg( plr->getIPAddress() )
                     .arg( pkt )  //Encrypted packet into the log file.
                     .arg( Helper::serNumToIntStr( srcSerNum, true ) )
                     .arg( plr->getSernum_s() );

    User::addMute( plr, msg, true, PunishDurations::THIRTY_MINUTES );
    emit this->insertLogSignal( "PacketForge", msg, LKeys::PunishmentLog, true, true );

    return false;
}
