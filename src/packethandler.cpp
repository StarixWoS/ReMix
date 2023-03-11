
//Class includes.
#include "packethandler.hpp"

//ReMix includes.
#include "campexemption.hpp"
#include "packetforge.hpp"
#include "cmdhandler.hpp"
#include "settings.hpp"
#include "chatview.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "player.hpp"
#include "server.hpp"
#include "user.hpp"

//ReMix Specialized Packet Handlers.
#include "packethandlers/wospackethandler.hpp"
#include "packethandlers/toypackethandler.hpp"
#include "packethandlers/w97packethandler.hpp"

//Qt Includes.
#include <QTcpSocket>
#include <QtCore>
#include <QTime>
#include <QHash>

QHash<QSharedPointer<Server>, PacketHandler*> PacketHandler::pktHandleInstanceMap;

PacketHandler::PacketHandler(QSharedPointer<Server> svr, ChatView* chat)
    : server( svr )
{
    chatView = chat;

    if ( server->getGameId() == Games::WoS )
        QObject::connect( WoSPacketHandler::getInstance( svr ), &WoSPacketHandler::sendPacketToPlayerSignal, this, &PacketHandler::sendPacketToPlayerSignal );

    QObject::connect( CmdHandler::getInstance( server ), &CmdHandler::newUserCommentSignal, this, &PacketHandler::newUserCommentSignal );

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &PacketHandler::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );
}

PacketHandler::~PacketHandler()
{
    server = nullptr;
}

PacketHandler* PacketHandler::getInstance(QSharedPointer<Server> server)
{
    PacketHandler* instance{ pktHandleInstanceMap.value( server, nullptr ) };
    if ( instance == nullptr )
    {
        instance = new PacketHandler( server, ChatView::getInstance( server ) );
        if ( instance != nullptr )
            pktHandleInstanceMap.insert( server, instance );
    }
    return instance;
}

void PacketHandler::deleteInstance(QSharedPointer<Server> server)
{
    PacketHandler* instance{ pktHandleInstanceMap.take( server ) };
    if ( instance != nullptr )
    {
        instance->disconnect();
        instance->deleteLater();
    }
}

void PacketHandler::parsePacketSlot(const QByteArray& packet, QSharedPointer<Player> plr)
{
    //Do not parse packets from Muted, Disconnected, or Null Users.
    if ( plr == nullptr
      || plr->getIsMuted()
      || plr->getIsDisconnected() )
    {
        return;
    }

    QByteArray pkt{ packet };
    QString data{ pkt };

    this->detectFlooding( plr );

    if ( Helper::strStartsWithStr( packet, ":MIX" ) )
    {
        QChar opCode{ ' ' };
        if ( data.size() > 4 )
            opCode = data.at( 4 );

        data = Helper::getStrStr( data, "", ":MIX", "" ).mid( 1 );
        switch ( opCode.toLatin1() )
        {
            case '0':   //Send Next Packet to Scene.
                this->readMIX0( data, plr );
            break;
            case '1':   //Register Player within SerNum's Scene.
                this->readMIX1( data, plr );
            break;
            case '2':   //Unknown.
                this->readMIX2( data, plr );
            break;
            case '3':   //Attune a Player to thier SerNum for private messaging.
                this->readMIX3( data, plr );
            break;
            case '4':   //Send the next Packet from the User to SerNum's Socket.
                this->readMIX4( data, plr );
            break;
            case '5':   //Handle Server password login, remote admin commands, and User Comments.
                this->readMIX5( data, plr );
            break;
            //case '6':   //Handle Remote Admin Commands. Mix6 packets contain "/cmd command". ReMix no longer supports this packet.
            //    this->readMIX6( data, plr );
            //break;
            case '7':   //Set the User's HB ID.
                this->readMIX7( data, plr );
            break;
            case '8':   //Set/Read SSV Variable.
                this->readMIX8( data, plr );
            break;
            case '9':   //Set/Read SSV Variable.
                this->readMIX9( data, plr );
            break;
            default:
                return;
        }
    }
    else if ( Helper::strStartsWithStr( packet, ":SR" )
           && !this->checkBannedInfo( plr ) )
    {
        //Prevent Users from Impersonating the Server Admin.
        //Prevent Users from changing the Server's rules.
        if ( Helper::strStartsWithStr( pkt, ":SR@" )
          || Helper::strStartsWithStr( pkt, ":SR$" ) )
        {
            return;
        }
        else if ( Helper::strStartsWithStr( pkt, ":SR?" ) ) //User is requesting Slot information for their packet headers.
        {
            QString sernum{ packet.mid( 4 ).left( 8 ) };
            plr->validateSerNum( server, Helper::serNumToInt( sernum, true ) );

            server->sendPlayerSocketPosition( plr, false );
            return; //No need to continue parsing. Return now.
        }

        //Ensure Players are sending packets using their assigned Header/Slot Position.
        if ( !this->validatePacketHeader( plr, pkt ) )
            return;

        if ( !plr->getIsMuted() )
        {
            bool parsePkt{ true };

            //Warpath doesn't send packets using SerNums.
            //Check and skip the validation if this is Warpath.
            if ( server->getGameId() != Games::W97 )
            {
                if ( !PacketForge::getInstance()->validateSerNum( plr, packet ) )
                    parsePkt = false;
                else
                    parsePkt = this->parseTCPPacket( packet, plr );
            }
            else
                parsePkt = this->parseTCPPacket( packet, plr );

            if ( parsePkt
              && plr->getIsVisible() )
            {
                if ( !pkt.isEmpty() )
                    pkt.append( "\r\n" );

                if ( plr->getSvrPwdReceived()
                  || !plr->getSvrPwdRequested() )
                {
                    emit this->sendPacketToPlayerSignal( plr, *plr->getTargetType(), plr->getTargetSerNum(),
                                                         plr->getTargetScene(), pkt );
                    //Reset the User's target information.
                    plr->setTargetType( PktTarget::ALL );
                    plr->setTargetSerNum( 0 );
                    plr->setTargetScene( 0 );
                }
            }
        }
    }
    return;
}

bool PacketHandler::parseTCPPacket(const QByteArray& packet, QSharedPointer<Player> plr)
{
    switch ( server->getGameId() )
    {
        case Games::WoS:
            {
                auto* object = WoSPacketHandler::getInstance( server );
                return object->handlePacket( server, chatView, packet, plr );
            }
        case Games::ToY:
            {
                auto* object = ToYPacketHandler::getInstance( server );
                return object->handlePacket( server, chatView, packet, plr );
            }
        case Games::W97:
            {
                return W97PacketHandler::getInstance()->handlePacket( server, chatView, packet, plr );
            }
        case Games::Invalid:
        default:
            return false;

    }
    return false;
}

bool PacketHandler::checkBannedInfo(QSharedPointer<Player> plr) const
{
    if ( plr == nullptr )
        return true;

    //The Player is already in a disconnected state. Return as true.
    if ( plr->getIsDisconnected() )
        return true;

    QSharedPointer<Player> tmpPlr{ nullptr };

    bool badInfo{ false };

    QString logMsg{ "Auto-Disconnect; %1: [ %2 ], [ %3 ]" };
    QString plrMessage{ "Auto-%1; %2" };
    QString plrSerNum{ plr->getSernumHex_s() };
    QString reason{ logMsg };

    //Prevent Banned IP's or SerNums from remaining connected.
    if ( this->getIsBanned( plr->getSernumHex_s(), plr->getIPAddress(), plrSerNum ) )
    {
        reason = reason.arg( "Banned Info" )
                       .arg( plr->getIPAddress() )
                       .arg( plr->getBioData() );

        emit this->insertLogSignal( server->getServerName(), reason, LKeys::PunishmentLog, true, true );

        plrMessage = plrMessage.arg( "Disconnect" )
                               .arg( "Banned Info" );
        server->sendMasterMessage( plrMessage, plr, false );

        plr->setDisconnected( true, DCTypes::IPDC );
        badInfo = true;
    }

    //Disconnect and ban duplicate IP's if required.
    if ( !Settings::getSetting( SKeys::Setting, SSubKeys::AllowDupe ).toBool() )
    {
        for ( int i = 0; i < server->getMaxPlayerCount(); ++i )
        {
            tmpPlr = server->getPlayer( i );
            if ( tmpPlr != nullptr
              && tmpPlr != plr )
            {
                if ( tmpPlr->getIPAddress() == plr->getIPAddress()
                  && !plr->getIsDisconnected() )
                {
                    auto disconnect =
                    [=, this]( QSharedPointer<Player> plr, const QString& logMsg, QString& plrMessage )
                    {
                        QString reason{ logMsg };
                                reason = reason.arg( "Duplicate IP" )
                                               .arg( plr->getIPAddress() )
                                               .arg( plr->getBioData() );

                        emit this->insertLogSignal( server->getServerName(), reason, LKeys::PunishmentLog, true, true );

                        if ( Settings::getSetting( SKeys::Setting, SSubKeys::BanDupes ).toBool() )
                        {
                            reason = "Auto-Banish; Duplicate IP Address: [ %1 ], %2";
                            reason = reason.arg( plr->getIPAddress() )
                                           .arg( plr->getBioData() );

                            //Ban for only half an hour.
                            User::addBan( plr, reason, PunishDurations::THIRTY_MINUTES );

                            emit this->insertLogSignal( server->getServerName(), reason, LKeys::PunishmentLog, true, true );

                            plrMessage = plrMessage.arg( "Banish" )
                                                   .arg( "Duplicate IP" );
                            server->sendMasterMessage( plrMessage, plr, false );
                        }
                        else
                        {
                            plrMessage = plrMessage.arg( "Disconnect" )
                                                   .arg( "Duplicate IP" );
                            server->sendMasterMessage( plrMessage, plr, false );
                        }
                        plr->setDisconnected( true, DCTypes::DupDC );
                    };

                    //Only disconnect the new Player. If [Settings::getBanDupedIP()] is true the second Player will be removed.
                    disconnect( plr, logMsg, plrMessage );

                    badInfo = true;
                }
            }
        }
    }

    //Disconnect only the newly connected Player.
    if ( plr != nullptr )
    {
        for ( int i = 0; i < server->getMaxPlayerCount(); ++i )
        {
            tmpPlr = server->getPlayer( i );
            if ( tmpPlr != nullptr
              && tmpPlr != plr )
            {
                if ( ( tmpPlr->getHasSerNum() && plr->getHasSerNum() )
                  && ( tmpPlr->getSernum_i() == plr->getSernum_i() ) )
                {
                    if ( !plr->getIsDisconnected() )
                    {
                        reason = logMsg;
                        reason = reason.arg( "Duplicate SerNum" )
                                       .arg( plr->getIPAddress() )
                                       .arg( plr->getBioData() );

                        emit this->insertLogSignal( server->getServerName(), reason, LKeys::PunishmentLog, true, true );

                        plrMessage = plrMessage.arg( "Disconnect" )
                                               .arg( "Duplicate SerNum" );
                        server->sendMasterMessage( plrMessage, plr, false );

                        plr->setDisconnected( true, DCTypes::DupDC );
                        badInfo = true;
                    }
                }
            }
        }
    }
    return badInfo;
}

bool PacketHandler::getIsBanned(const QString& serNum, const QString& ipAddr, const QString& plrSerNum) const
{
    quint64 banned{ 0 };
    banned = User::getIsPunished( PunishTypes::Ban, serNum, PunishTypes::SerNum, plrSerNum );
    if ( banned == 0 )
        banned = User::getIsPunished( PunishTypes::Ban, ipAddr, PunishTypes::IP, plrSerNum );

    return ( banned >= 1 );
}

void PacketHandler::detectFlooding(QSharedPointer<Player> plr)
{
    if ( plr == nullptr )
        return;

    int floodCount{ plr->getPacketFloodCount() };
    if ( floodCount >= 1 )
    {
        qint64 time{ plr->getFloodTime() };
        if ( time <= *Globals::PACKET_FLOOD_TIME )
        {
            if ( floodCount >= *Globals::PACKET_FLOOD_LIMIT
                 && !plr->getIsDisconnected() )
            {
                QString logMsg{ "Auto-Mute; Packet Flooding: [ %1 ] sent %2 packets in %3 MS, they are muted: [ %4 ]" };
                logMsg = logMsg.arg( plr->getIPAddress() )
                               .arg( floodCount )
                               .arg( time )
                               .arg( plr->getBioData() );

                User::addMute( plr, logMsg, true, PunishDurations::THIRTY_MINUTES );
                emit this->insertLogSignal( server->getServerName(), logMsg, LKeys::PunishmentLog, true, true );

                QString plrMessage{ "Auto-Mute; Packet Flooding." };
                server->sendMasterMessage( plrMessage, plr, false );
            }
        }
        else if ( time >= *Globals::PACKET_FLOOD_TIME )
        {
            plr->restartFloodTimer();
            plr->setPacketFloodCount( 0 );
        }
    }
}

bool PacketHandler::validatePacketHeader(QSharedPointer<Player> plr, const QByteArray& pkt)
{
    bool disconnect{ false };
    QString message{ "" };
    QString reason{ "" };

    //Increment the Packet Header Exemption by 1 and ignore the packet.

    QString recvSlotPos{ pkt.mid( 4 ).left( 2 ) };
    qint32 plrPktSlot{ plr->getPktHeaderSlot() };
    if ( plrPktSlot != Helper::strToInt( recvSlotPos, IntBase::HEX ) )
    {
        qint32 exemptCount{ plr->getPktHeaderExemptCount() + 1 };
        if ( exemptCount >= *Globals::MAX_PKT_HEADER_EXEMPT )
        {
            disconnect = true;

            message = "Auto-Disconnect; Maximum alowed < 5 > Packet Header Exemptions exceeded.";
            server->sendMasterMessage( message, plr, false );

            reason = "Automatic Disconnect of <[ %1 ][ %2 ] BIO [ %3 ]> User exceeded the maximum allowed < %4 > Packet Header Exemptions.";
            reason = reason.arg( plr->getSernum_s() )
                           .arg( plr->getIPAddress() )
                           .arg( plr->getBioData() )
                           .arg( Helper::intToStr( *Globals::MAX_PKT_HEADER_EXEMPT ) );

            emit this->insertLogSignal( server->getServerName(), reason, LKeys::PunishmentLog, true, true );
        }
        else
        {
            plr->setPktHeaderExemptCount( exemptCount );

            message = "Error; Received Packet with Header [ :SR1%1 ] while assigned [ :SR1%2 ]. Exemptions remaining: [ %3 ].";
            message = message.arg( recvSlotPos )
                             .arg( Helper::intToStr( plrPktSlot, IntBase::HEX, IntFills::Byte ) )
                             .arg( *Globals::MAX_PKT_HEADER_EXEMPT - exemptCount );

            //Attempt to re-issue the User a valid Packet Slot ID.
            //I'm not sure if WoS will allow this.
            server->sendPlayerSocketPosition( plr, true );
        }
    }
    else //Slot Matched. Return true;
        return true;

    if ( !message.isEmpty() )
        server->sendMasterMessage( message, plr, false );

    if ( disconnect )
        plr->setDisconnected( true, DCTypes::PktDC );

    return false;
}

void PacketHandler::readMIX0(const QString& packet, QSharedPointer<Player> plr)
{
    const QString sernum{ packet.mid( 2 ).left( 8 ) };

    //Send the next Packet to the Scene's Host.
    plr->setTargetScene( Helper::serNumToInt( sernum, true ) );
    plr->setTargetType( PktTarget::SCENE );
}

void PacketHandler::readMIX1(const QString& packet, QSharedPointer<Player> plr)
{
    const QString sernum{ packet.mid( 2 ).left( 8 ) };
    plr->setSceneHost( Helper::serNumToInt( sernum, true ) );
}

void PacketHandler::readMIX2(const QString&, QSharedPointer<Player> plr)
{
    plr->setSceneHost( 0 );
    plr->setTargetType( PktTarget::ALL );
}

void PacketHandler::readMIX3(const QString& packet, QSharedPointer<Player> plr)
{
    const QString sernum{ packet.mid( 2 ).left( 8 ) };

    plr->validateSerNum( server, Helper::serNumToInt( sernum, true ) );
    this->checkBannedInfo( plr );
}

void PacketHandler::readMIX4(const QString& packet, QSharedPointer<Player> plr)
{
    const QString sernum{ packet.mid( 2 ).left( 8 ) };

    plr->setTargetSerNum( Helper::serNumToInt( sernum, true ) );
    plr->setTargetType( PktTarget::PLAYER );
}

void PacketHandler::readMIX5(const QString& packet, QSharedPointer<Player> plr)
{
    const QString sernum{ packet.mid( 2 ).left( 8 ) };
    if ( plr != nullptr )
    {
        if ( plr->getSernum_i() <= 0 )
            plr->validateSerNum( server, Helper::serNumToInt( sernum, true ) );

        //Do not accept comments from User who have been muted.
        if ( !plr->getIsMuted() )
            CmdHandler::getInstance( server )->parseMix5Command( plr, packet );
    }
}

//No longer a supported packet.
//void PacketHandler::readMIX6(const QString& packet, QSharedPointer<Player> plr)
//{
//    const QString sernum{ packet.mid( 2 ).left( 8 ) };
//    if ( plr != nullptr )
//    {
//        if ( plr->getSernum_i() <= 0 )
//            plr->validateSerNum( server, Helper::serNumToInt( sernum, true ) );

//        //Do not accept commands from User who have been muted.
//        if ( !plr->getIsMuted() )
//        {
//            static const QString message{ "This command syntax is no longer supported. Please use the syntax \"/command\" e.g. \"/help version\" instead." };
//            server->sendMasterMessage( message, plr );
//        }
//    }
//}

void PacketHandler::readMIX7(const QString& packet, QSharedPointer<Player> plr)
{
    if ( plr == nullptr )
        return;

    QString pkt{ packet };
            pkt = pkt.mid( 2 );
            pkt = pkt.left( pkt.length() - 2 );

    //Check if the User is banned or requires authentication.
    plr->validateSerNum( server, Helper::serNumToInt( pkt, true ) );
    this->checkBannedInfo( plr );
}

void PacketHandler::readMIX8(const QString& packet, QSharedPointer<Player> plr)
{
    this->handleSSVReadWrite( packet, plr, SSVModes::Read );
}

void PacketHandler::readMIX9(const QString& packet, QSharedPointer<Player> plr)
{
    this->handleSSVReadWrite( packet, plr, SSVModes::Write );
}

void PacketHandler::handleSSVReadWrite(const QString& packet, QSharedPointer<Player> plr, const SSVModes mode)
{
    if ( plr == nullptr || packet.isEmpty() )
        return;

    QString accessType{ "Read" };
    if ( Settings::getSetting( SKeys::Setting, SSubKeys::AllowSSV ).toBool() )
    {
        QString pkt{ packet };
                pkt = pkt.mid( 10 );
                pkt = pkt.left( pkt.length() - 2 );
        const QString sernum{ pkt.mid( 2 ).left( 8 ) };
        const QStringList vars{ pkt.split( ',' ) };

        QSettings ssv( "mixVariableCache/" % vars.value( 0 ) % ".ini", QSettings::IniFormat );

        QString val{ ":SR@V%1%2,%3,%4,%5\r\n" };
        QDir ssvDir( "mixVariableCache" );
        if ( mode == SSVModes::Write )
        {
            accessType = "Write";
            if ( !ssvDir.exists() )
                ssvDir.mkpath( "." );

            ssv.setValue( vars.value( 1 ) % "/" % vars.value( 2 ), vars.value( 3 ) );
        }

        QString value{ vars.value( 3 ) };
        if ( ssvDir.exists() )
        {
            value = ssv.value( vars.value( 1 ) % "/" % vars.value( 2 ), "" ).toString();
            val = val.arg( sernum )
                     .arg( vars.value( 0 ) ) //file name
                     .arg( vars.value( 1 ) ) //category
                     .arg( vars.value( 2 ) ) //variable
                     .arg( value ); //value

            if ( mode == SSVModes::Write )
            {
                for ( int i = 0; i < server->getMaxPlayerCount(); ++i )
                {
                    QSharedPointer<Player> tmpPlr = server->getPlayer( i );
                    if ( tmpPlr != nullptr
                      && plr != tmpPlr )
                    {
                        server->updateBytesOut( tmpPlr, tmpPlr->write( val.toLatin1(), val.length() ) );
                    }
                }
            }
            else if ( !val.isEmpty()
                   && ( mode == SSVModes::Read ) )
            {
                server->updateBytesOut( plr, plr->write( val.toLatin1(), val.length() ) );
            }
        }

        QString msg{ "Server Variable being Accessed[ %1 ]: SerNum[ %2 ], "
                     "FileName[ %3 ], Category[ %4 ], Variable[ %5 ], "
                     "Value[ %6 ]" };

        msg = msg.arg( accessType )
                 .arg( plr->getSernum_s() )
                 .arg( vars.value( 0 ) % ".ini" ) //file name
                 .arg( vars.value( 1 ) ) //category
                 .arg( vars.value( 2 ) ) //variable
                 .arg( value ); //value

        emit this->insertLogSignal( server->getServerName(), msg, LKeys::SSVLog, true, true );
    }
}
