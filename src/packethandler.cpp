
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

    QObject::connect( CmdHandler::getInstance( server ), &CmdHandler::newUserCommentSignal, this, &PacketHandler::newUserCommentSignal );
    QObject::connect( this, &PacketHandler::insertChatMsgSignal, ChatView::getInstance( server ), &ChatView::insertChatMsgSlot );

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
    QChar opCode{ pkt.at( 4 ) };
    QString data{ pkt };

    this->detectFlooding( plr );

    if ( Helper::strStartsWithStr( packet, ":MIX" ) )
    {
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
            case '5':   //Handle Server password login and User Comments.
                this->readMIX5( data, plr );
            break;
            case '6':   //Handle Remote Admin Commands.
                this->readMIX6( data, plr );
            break;
            case '7':   //Set the User's HB ID.
                this->readMIX7( data, plr );
            break;
            case '8':   //Set/Read SSV Variable.
                this->readMIX8( data, plr );
            break;
            case '9':   //Set/Read SSV Variable.
                this->readMIX9( data, plr );
            break;
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
            plr->validateSerNum( server, Helper::serNumtoInt( sernum, true ) );

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
                    emit this->sendPacketToPlayerSignal( plr, static_cast<qint32>( plr->getTargetType() ), plr->getTargetSerNum(),
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
    //The Player object is invalid, return.
    if ( plr == nullptr )
        return false;

    Colors serNumColor{ Colors::WhiteSoul };
    Colors nameColor{ Colors::PlayerName };
    if ( plr->getAdminRank() >= GMRanks::GMaster )
    {
        nameColor = Colors::AdminName;
        if ( plr->getAdminRank() > GMRanks::Admin )
            nameColor = Colors::OwnerName;
    }

    if ( plr->getIsGoldenSerNum() )
        serNumColor = Colors::GoldenSoul;

    bool retn{ true };
    QString pkt{ packet };
    if ( server->getGameId() != Games::W97 )
    {
        //WoS and Arcadia distort Packets in the same manner.
        pkt = PacketForge::getInstance()->decryptPacket( packet );
        if ( !pkt.isEmpty() )
        {

            //Remove checksum from Arcadia chat packet.
            if ( server->getGameId() == Games::ToY )
            {
                //Arcadia Packets have a longer checksum than WoS packets.
                //Remove the extra characters.
                pkt = pkt.left( pkt.length() - 4 );
            }

            //WoS and Arcadia both use the opCode 'C' at position '3' in the packet to denote Chat packets.
            if ( pkt.at( 3 ) == 'C' )
            {
                plr->setIsAFK( false );
                retn = chatView->parseChatEffect( pkt );
            }
            else if ( pkt.at( 3 ) == '3'
                   || ( ( server->getGameId() == Games::ToY ) && ( pkt.at( 3 ) == 'N' ) ) )
            {
                QStringList varList;
                if ( server->getGameId() == Games::ToY )
                    varList = pkt.mid( 39 ).split( "," );
                else
                    varList = pkt.mid( 47 ).split( "," );

                QString plrName{ varList.at( 0 ) };
                if ( !plrName.isEmpty() )
                    plr->setPlrName( plrName );

                //Check that the User is actually incarnating.
                int type{ pkt.at( 14 ).toLatin1() - 0x41 };
                if ( type >= 1 && !Helper::cmpStrings( plr->getPlrName(), "Unincarnated" ) )
                {
                    bool isIncarnated{ plr->getIsIncarnated() };
                    plr->setIsIncarnated( true );
                    plr->setIsGhosting( false );

                    QString msg{ "" };
                    switch ( type )
                    {
                        case 1:
                            {
                                msg = "has incarnated into this world! ";
                            }
                        break;
                        case 2:
                            {
                                plr->setIsGhosting( true );
                                msg = "walks the land as an apparition! ";
                            }
                        break;
                        case 4:
                            {
                                plr->setIsIncarnated( false );
                                isIncarnated = false;
                                msg = "has returned to the Well of Souls! ";
                            }
                        break;
                        default:
                        break;
                    }

                    if ( !msg.isEmpty()
                      && !isIncarnated )
                    {
                        emit this->insertChatMsgSignal( ChatView::getTimeStr(), Colors::TimeStamp, true );
                        emit this->insertChatMsgSignal( "*** ", Colors::SoulIncarnated, false );
                        emit this->insertChatMsgSignal( plr->getPlrName(), nameColor, false );
                        emit this->insertChatMsgSignal( " [ " % plr->getSernum_s() % " ] ", serNumColor, false );
                        emit this->insertChatMsgSignal( msg, Colors::SoulIncarnated, false );
                        emit this->insertChatMsgSignal( "***", Colors::SoulIncarnated, false );
                    }
                }

                if ( type >= 1 && type != 4 )
                {
                    //Send Camp packets to the newly connecting User.
                    if ( server->getGameId() == Games::WoS )
                    {
                        for ( int i = 0; i < server->getMaxPlayerCount(); ++i )
                        {
                            QSharedPointer<Player> tmpPlr{ server->getPlayer( i ) };
                            if ( tmpPlr != nullptr
                              && plr != tmpPlr )
                            {
                                if ( tmpPlr->getIsVisible() ) //Do not force Invisible Users to send camp packets.
                                {
                                    if ( plr->getSceneHost() != tmpPlr->getSernum_i()
                                      || plr->getSceneHost() <= 0 )
                                    {
                                        if ( !tmpPlr->getCampPacket().isEmpty()
                                          && tmpPlr->getTargetType() == PktTarget::ALL )
                                        {
                                            tmpPlr->setTargetSerNum( plr->getSernum_i() );
                                            tmpPlr->setTargetType( PktTarget::PLAYER );
                                            tmpPlr->forceSendCampPacket();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if ( server->getGameId() == Games::WoS )
            {
                QString trgSerNum{ pkt.left( 21 ).mid( 13 ) };
                QSharedPointer<Player> targetPlayer{ server->getPlayer( trgSerNum ) };

                Colors targetNameColor{ Colors::PlayerName };
                if ( targetPlayer != nullptr )
                {
                    targetNameColor = Colors::AdminName;
                    if ( targetPlayer->getAdminRank() > GMRanks::Admin )
                        targetNameColor = Colors::OwnerName;
                }

                switch ( pkt.at( 3 ).toLatin1() )
                {
                    case '5': //Player Leaves Server.
                        {
                            emit this->insertChatMsgSignal( ChatView::getTimeStr(), Colors::TimeStamp, true );
                            emit this->insertChatMsgSignal( "*** ", Colors::SoulLeftWorld, false );
                            emit this->insertChatMsgSignal( plr->getPlrName(), nameColor, false );
                            emit this->insertChatMsgSignal( " [ " % plr->getSernum_s() % " ] ", serNumColor, false );
                            emit this->insertChatMsgSignal( " has left this world! ***", Colors::SoulLeftWorld, false );
                        }
                    break;
                    case 'k': //PK Attack.
                        {
                            if ( targetPlayer != nullptr )
                            {
                                emit this->insertChatMsgSignal( ChatView::getTimeStr(), Colors::TimeStamp, true );
                                emit this->insertChatMsgSignal( "*** ", Colors::PKChallenge, false );
                                emit this->insertChatMsgSignal( plr->getPlrName(), nameColor, false );
                                emit this->insertChatMsgSignal( " has challenged ", Colors::PKChallenge, false );
                                emit this->insertChatMsgSignal( targetPlayer->getPlrName(), targetNameColor, false );
                                emit this->insertChatMsgSignal( " to a PK fight! ***", Colors::PKChallenge, false );
                            }
                        }
                    break;
                    case 'p':
                        {
                            bool isJoining = true;

                            int leader = trgSerNum.toInt( nullptr, 16 );
                            if ( leader == 0 )
                            {
                                leader = packet.left( 29 ).mid( 21 ).toInt( nullptr, 16 );
                                if ( leader == 0 )
                                    break;

                                isJoining = false;
                            }

                            QSharedPointer<Player> partyLeader{ server->getPlayer( Helper::intToStr( leader, static_cast<int>( IntBase::HEX ), 8 ) ) };
                            if ( partyLeader != nullptr )
                            {
                                if ( partyLeader->getAdminRank() >= GMRanks::GMaster )
                                {
                                    targetNameColor = Colors::AdminName;
                                    if ( partyLeader->getAdminRank() > GMRanks::Admin )
                                        targetNameColor = Colors::OwnerName;
                                }

                                emit this->insertChatMsgSignal( ChatView::getTimeStr(), Colors::TimeStamp, true );
                                emit this->insertChatMsgSignal( "*** ", Colors::PartyJoin, false );
                                emit this->insertChatMsgSignal( plr->getPlrName(), nameColor, false );
                                emit this->insertChatMsgSignal( " [ " % plr->getSernum_s() % " ] ", serNumColor, false );
                                emit this->insertChatMsgSignal( ( isJoining ? " joins " : " leaves " ), Colors::PartyJoin, false );

                                emit this->insertChatMsgSignal( partyLeader->getPlrName() % "'s [ " % partyLeader->getSernum_s() % " ] ",
                                                                targetNameColor, false );
                                emit this->insertChatMsgSignal( "party. ***", Colors::PartyJoin, false );
                            }
                        }
                    break;
                    case 'F':
                        {  //Save the User's camp packet. --Send to newly connecting Users.
                            if ( plr->getCampPacket().isEmpty() )
                            {
                                qint32 sceneID{ Helper::strToInt( pkt.left( 17 ).mid( 13 ) ) };
                                if ( sceneID >= 1 ) //If is 0 then it is the well scene and we can ignore the 'camp' packet.
                                {
                                    plr->setCampPacket( packet );
                                    plr->setCampCreatedTime( QDateTime::currentDateTime().toSecsSinceEpoch() );
                                }
                            }
                        }
                    break;
                    case 'f':
                        {  //User un-camp. Remove camp packet.
                            if ( !plr->getCampPacket().isEmpty() )
                            {
                                plr->setCampPacket( "" );
                                plr->setCampCreatedTime( 0 );
                            }
                        }
                    break;
                    case 's': //Parse Player Level and AFK status.
                        {
                            plr->setPlrLevel( Helper::strToInt( pkt.mid( 21 ).left( 4 ) ) );
                            plr->setIsAFK( Helper::strToInt( pkt.mid( 89 ).left( 2 ) ) & 1 );
                        }
                    break;
                    case 'K':  //If pet level exceess the Player's level then discard the packet.
                        {
                            QString msg{ "You may not call a pet stronger than yourself within a camp (scene) hosted by another Player!" };
                            qint32 petLevel{ Helper::strToInt( pkt.mid( 19 ).left( 4 ) ) };

                            if ( plr->getPlrLevel() >= 1
                              && petLevel >= plr->getPlrLevel() )
                            {
                                server->sendMasterMessage( msg, plr, false );
                                retn = false;
                            }
                        }
                    break;
                    case 'J':
                        {
                            QSharedPointer<Player> tmpPlr{ nullptr };
                            for ( int i = 0; i < server->getMaxPlayerCount(); ++i )
                            {
                                tmpPlr = server->getPlayer( i );
                                if ( tmpPlr != nullptr )
                                {
                                    if ( Helper::cmpStrings( tmpPlr->getSernumHex_s(), trgSerNum ) )
                                        break;
                                }
                                tmpPlr = nullptr;
                            }

                            if ( tmpPlr != nullptr )
                            {
                                QString message{ "The Camp Hosted by [ %1 ] is currently locked and you may not enter!" };
                                if ( !CampExemption::getInstance()->getIsWhitelisted( tmpPlr->getSernumHex_s(), plr->getSernumHex_s() ) )
                                {   //The Player is not exempted from further checking.
                                    if ( tmpPlr->getIsCampLocked() )
                                    {
                                        retn = false;
                                    }
                                    else if ( tmpPlr->getIsCampOptOut() )
                                    {
                                        //The Camp was created before the Player connected. Mark it as old.
                                        if (( tmpPlr->getCampCreatedTime() - plr->getPlrConnectedTime() ) < 0 )
                                        {
                                            message = "The Camp Hosted by [ %1 ] is considered \"Old\" to your client and you can not enter!";
                                            retn = false;
                                        }
                                        else
                                            retn = true;
                                    }
                                }

                                if ( !retn )
                                {
                                    message = message.arg( tmpPlr->getPlrName() );
                                    server->sendMasterMessage( message, plr, false );
                                }
                            }
                        }
                    break;
                }
            }
        }
    }
    else //Handle Warpath97 and Warpath 21st Century Chat.
    {
        pkt = pkt.trimmed();

        //Warpath denotes Chat Packets with opCode 'D' at position '7'.
        if ( pkt.at( 7 ) == 'D' )
        {
            //Remove the checksum.
            pkt = pkt.left( pkt.length() - 2 );
            retn = chatView->parseChatEffect( pkt );

            plr->setIsAFK( false );
        }
        else if ( pkt.at( 7 ) == '4' )
        {
            QString plrName{ pkt.mid( 20 ) };
            plrName = plrName.left( plrName.length() - 2 );
            if ( !plrName.isEmpty() )
                plr->setPlrName( plrName );
        }
    }
    return retn;
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
            if ( ( tmpPlr != nullptr )
              && ( tmpPlr->getSernum_i() == plr->getSernum_i() ) )
            {
                if ( tmpPlr != plr
                  && !plr->getIsDisconnected() )
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
        if ( time <= static_cast<int>( Globals::PACKET_FLOOD_TIME ) )
        {
            if ( floodCount >= static_cast<int>( Globals::PACKET_FLOOD_LIMIT )
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
        else if ( time >= static_cast<qint64>( Globals::PACKET_FLOOD_TIME ) )
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
    if ( plrPktSlot != Helper::strToInt( recvSlotPos, static_cast<int>( IntBase::HEX ) ) )
    {
        qint32 exemptCount{ plr->getPktHeaderExemptCount() + 1 };
        if ( exemptCount >= static_cast<int>( Globals::MAX_PKT_HEADER_EXEMPT ) )
        {
            disconnect = true;

            message = "Auto-Disconnect; Maximum alowed < 5 > Packet Header Exemptions exceeded.";
            server->sendMasterMessage( message, plr, false );

            reason = "Automatic Disconnect of <[ %1 ][ %2 ] BIO [ %3 ]> User exceeded the maximum allowed < %4 > Packet Header Exemptions.";
            reason = reason.arg( plr->getSernum_s() )
                           .arg( plr->getIPAddress() )
                           .arg( plr->getBioData() )
                           .arg( Helper::intToStr( static_cast<int>( Globals::MAX_PKT_HEADER_EXEMPT ) ) );

            emit this->insertLogSignal( server->getServerName(), reason, LKeys::PunishmentLog, true, true );
        }
        else
        {
            plr->setPktHeaderExemptCount( exemptCount );

            message = "Error; Received Packet with Header [ :SR1%1 ] while assigned [ :SR1%2 ]. Exemptions remaining: [ %3 ].";
            message = message.arg( recvSlotPos )
                             .arg( Helper::intToStr( plrPktSlot, static_cast<int>( IntBase::HEX ), 2 ) )
                             .arg( static_cast<int>( Globals::MAX_PKT_HEADER_EXEMPT ) - exemptCount );

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
    QString sernum = packet.mid( 2 ).left( 8 );

    //Send the next Packet to the Scene's Host.
    plr->setTargetScene( Helper::serNumtoInt( sernum, true ) );
    plr->setTargetType( PktTarget::SCENE );
}

void PacketHandler::readMIX1(const QString& packet, QSharedPointer<Player> plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );
    plr->setSceneHost( Helper::serNumtoInt( sernum, true ) );
}

void PacketHandler::readMIX2(const QString&, QSharedPointer<Player> plr)
{
    plr->setSceneHost( 0 );
    plr->setTargetType( PktTarget::ALL );
}

void PacketHandler::readMIX3(const QString& packet, QSharedPointer<Player> plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );

    plr->validateSerNum( server, Helper::serNumtoInt( sernum, true ) );
    this->checkBannedInfo( plr );
}

void PacketHandler::readMIX4(const QString& packet, QSharedPointer<Player> plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );

    plr->setTargetSerNum( Helper::serNumtoInt( sernum, true ) );
    plr->setTargetType( PktTarget::PLAYER );
}

void PacketHandler::readMIX5(const QString& packet, QSharedPointer<Player> plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );
    if ( plr != nullptr )
    {
        if ( plr->getSernum_i() <= 0 )
            plr->validateSerNum( server, Helper::serNumtoInt( sernum, true ) );

        //Do not accept comments from User who have been muted.
        if ( !plr->getIsMuted() )
            CmdHandler::getInstance( server )->parseMix5Command( plr, packet );
    }
}

void PacketHandler::readMIX6(const QString& packet, QSharedPointer<Player> plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );
    if ( plr != nullptr )
    {
        if ( plr->getSernum_i() <= 0 )
            plr->validateSerNum( server, Helper::serNumtoInt( sernum, true ) );

        //Do not accept commands from User who have been muted.
        if ( !plr->getIsMuted() )
        {
            static const QString message{ "This command syntax is no longer supported. Please use the syntax \"/command\" e.g. \"/help version\" instead." };
            server->sendMasterMessage( message, plr );
        }
    }
}

void PacketHandler::readMIX7(const QString& packet, QSharedPointer<Player> plr)
{
    if ( plr == nullptr )
        return;

    QString pkt{ packet };
            pkt = pkt.mid( 2 );
            pkt = pkt.left( pkt.length() - 2 );

    //Check if the User is banned or requires authentication.
    plr->validateSerNum( server, Helper::serNumtoInt( pkt, true ) );
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
        QStringList vars{ pkt.split( ',' ) };

        QSettings ssv( "mixVariableCache/" % vars.value( 0 ) % ".ini", QSettings::IniFormat );
        QString sernum{ pkt.mid( 2 ).left( 8 ) };

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
