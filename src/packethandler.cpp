
//Class includes.
#include "packethandler.hpp"

//ReMix includes.
#include "packetforge.hpp"
#include "cmdhandler.hpp"
#include "serverinfo.hpp"
#include "settings.hpp"
#include "chatview.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "player.hpp"
#include "server.hpp"
#include "user.hpp"

//Qt Includes.
#include <QTcpSocket>
#include <QTime>
#include <QtCore>

PacketHandler::PacketHandler(ServerInfo* svr, ChatView* chat)
{
    pktForge = PacketForge::getInstance();
    chatView = chat;
    server = svr;

    cmdHandle = new CmdHandler( this, server );
    chatView->setCmdHandle( cmdHandle );
    QObject::connect( cmdHandle, &CmdHandler::newUserCommentSignal, this, &PacketHandler::newUserCommentSignal, Qt::QueuedConnection );

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &PacketHandler::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot, Qt::QueuedConnection );
}

PacketHandler::~PacketHandler()
{
    cmdHandle->deleteLater();
}

void PacketHandler::parsePacketSlot(const QByteArray& packet, Player* plr)
{
    QByteArray pkt{ packet };
    QChar opCode{ pkt.at( 4 ) };
    QString data{ pkt };
    if ( plr == nullptr )
        return;

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
            plr->validateSerNum( server, sernum.toUInt( nullptr, 16 ) );
            server->sendPlayerSocketPosition( plr );
            return; //No need to continue parsing. Return now.
        }

        //Ensure Players are sending packets using their assigned Header/Slot Position.
        if ( !this->validatePacketHeader( plr, pkt ) )
            return;

        if ( !plr->getIsMuted()
          && plr->getIsVisible() )
        {
            bool parsePkt{ true };

            //Warpath doesn't send packets using SerNums.
            //Check and skip the validation if this is Warpath.
            if ( server->getGameId() != Games::W97
              && pktForge != nullptr )
            {
                if ( !pktForge->validateSerNum( plr, packet ) )
                    parsePkt = false;
                else
                    parsePkt = chatView->parsePacket( packet, plr );
            }

            if ( parsePkt )
            {
                if ( !pkt.isEmpty() )
                    pkt.append( "\r\n" );

                if ( plr->getSvrPwdReceived() || !plr->getSvrPwdRequested() )
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

void PacketHandler::parseUDPPacket(const QByteArray& udp, const QHostAddress& ipAddr, const quint16& port)
{
    QString logTxt{ "Ignoring UDP from banned %1: [ %2 ] sent command: [ %3 ]" };

    QString data{ udp };
    QString sernum{ "" };
    QString dVar{ "" };
    QString wVar{ "" };

    qint32 index{ 0 };
    if ( !data.isEmpty() )
    {
        QChar opCode{ data.at( 0 ).toLatin1() };
        bool isMaster{ false };
        if ( opCode == 'M' )
        {
            //Prevent Spoofing a MasterMix response.
            if ( Helper::cmpStrings( server->getMasterIP(), ipAddr.toString() ) )
                isMaster = true;
        }

        switch ( opCode.toLatin1() )
        {
            case 'G':   //Set the Server's gameInfoString.
            {
                //Check if the IP Address is a properly connected User. Or at least is in the User list...
                Player* tmpPlr{ nullptr };
                bool connected{ false };
                for ( int i = 0; i < MAX_PLAYERS; ++i )
                {
                    tmpPlr = server->getPlayer( i );
                    if ( tmpPlr != nullptr )
                    {
                        if ( Helper::cmpStrings( tmpPlr->peerAddress().toString(), ipAddr.toString() ) )
                        {
                            connected = true;
                            break;
                        }
                    }
                    connected = false;
                }

                if ( !connected )
                    break;

                QString svrInfo{ server->getGameInfo() };
                QString usrInfo{ data.mid( 1 ) };
                if ( svrInfo.isEmpty() && !usrInfo.isEmpty() )
                {
                    //Check if the World String starts with "world=" And remove the substring.
                    if ( Helper::strStartsWithStr( usrInfo, "world=" ) )
                        usrInfo = Helper::getStrStr( usrInfo, "world", "=", "=" );

                    //Enforce a 256 character limit on GameNames.
                    if ( usrInfo.length() > MAX_GAME_NAME_LENGTH )
                        server->setGameInfo( usrInfo.left( MAX_GAME_NAME_LENGTH ) ); //Truncate the User's GameInfo String to 256.
                    else
                        server->setGameInfo( usrInfo ); //Length was less than 256, set without issue.
                }
            }
            break;
            case 'M':   //Parse the Master Server's response.
            {
                if ( !isMaster )   //Prevent Spoofing a MasterMix response.
                    break;

                QString msg{ "Got Response from Master [ %1:%2 ]; it thinks we are [ %3:%4 ]. "
                            "( Ping: %5 ms, Avg: %6 ms, Trend: %7 ms, Dropped: %8 )" };

                msg = msg.arg( ipAddr.toString() )
                         .arg( port );

                //Store the Master Server's Response Time.
                server->setMasterPingRespTime( QDateTime::currentMSecsSinceEpoch() );

                //We've obtained a Master response.
                server->setMasterUDPResponse( true );

                quint32 pubIP{ 0 };
                int pubPort{ 0 };
                int opcode{ 0 };

                if ( !data.isEmpty() )
                {
                    QDataStream mDataStream( udp );
                                mDataStream >> opcode;
                                mDataStream >> pubIP;
                                mDataStream >> pubPort;

                    server->setPublicIP( QHostAddress( pubIP ).toString() );
                    server->setPublicPort( static_cast<quint16>( qFromBigEndian( pubPort ) ) );

                    msg = msg.arg( server->getPublicIP() )
                             .arg( server->getPublicPort() )
                             .arg( server->getMasterPing() )
                             .arg( server->getMasterPingAvg() )
                             .arg( server->getMasterPingTrend() )
                             .arg( server->getMasterPingFailCount() );

                    emit this->insertLogSignal( server->getServerName(), msg, LogTypes::USAGE, true, true );
                }
            }
            break;
            case 'P':   //Store the Player information into a struct.
            {
                index = Helper::getStrIndex( data, "sernum=" );
                if ( index >= 0 )
                {
                    sernum = data.mid( index + 7 );
                    index = Helper::getStrIndex( sernum, "," );
                    if ( index >= 0 )
                    {
                        sernum = sernum.left( index );
                        if ( !sernum.isEmpty() )
                            sernum = Helper::serNumToHexStr( sernum );
                    }
                }

                QString msg{ "Recieved ping from User [ %1:%2 ] with SoulID [ %3 ] and BIO data; %4" };
                        msg = msg.arg( ipAddr.toString() )
                                 .arg( port )
                                 .arg( Helper::serNumToIntStr( sernum ) )
                                 .arg( data );
                emit this->insertLogSignal( server->getServerName(), msg, LogTypes::USAGE, true, true );

                Settings::insertBioHash( ipAddr, udp.mid( 1 ) );
                User::logBIO( sernum, ipAddr, data );
            }
            break;      //Q and R packet types are handled within the UdpThread class.
            case 'Q':   //Send Online User Information.
            case 'R':   //Send Online User Information.
            break;
        }
    }
}

bool PacketHandler::checkBannedInfo(Player* plr) const
{
    if ( plr == nullptr )
        return true;

    //The Player is already in a disconnected state. Return as true.
    if ( plr->getIsDisconnected() )
        return true;

    Player* tmpPlr{ nullptr };

    bool badInfo{ false };

    QString logMsg{ "Auto-Disconnect; %1: [ %2 ], [ %3 ]" };
    QString plrMessage{ "Auto-%1; %2" };
    QString plrSerNum{ plr->getSernumHex_s() };
    QString reason{ logMsg };

    //Prevent Banned IP's or SerNums from remaining connected.
    if ( this->getIsBanned( plr->getSernumHex_s(), plr->peerAddress().toString(), plrSerNum ) )
    {
        reason = reason.arg( "Banned Info" )
                       .arg( plr->peerAddress().toString() )
                       .arg( plr->getBioData() );

        emit this->insertLogSignal( server->getServerName(), reason, LogTypes::PUNISHMENT, true, true );

        plrMessage = plrMessage.arg( "Disconnect" )
                               .arg( "Banned Info" );
        server->sendMasterMessage( plrMessage, plr, false );

        plr->setDisconnected( true, DCTypes::IPDC );
        badInfo = true;
    }

    //Disconnect and ban duplicate IP's if required.
    if ( !Settings::getSetting( SKeys::Setting, SSubKeys::AllowDupe ).toBool() )
    {
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            tmpPlr = server->getPlayer( i );
            if ( tmpPlr != nullptr
              && tmpPlr != plr )
            {
                if ( ( tmpPlr->peerAddress().toString() == plr->peerAddress().toString() )
                  && ( !tmpPlr->getIsDisconnected() || !plr->getIsDisconnected() ) )
                {
                    auto disconnect = [=]( Player* plr, const QString& logMsg,
                                           QString& plrMessage )
                    {
                        QString reason{ logMsg };
                                reason = reason.arg( "Duplicate IP" )
                                               .arg( plr->peerAddress().toString() )
                                               .arg( plr->getBioData() );

                        emit this->insertLogSignal( server->getServerName(), reason, LogTypes::PUNISHMENT, true, true );

                        if ( Settings::getSetting( SKeys::Setting, SSubKeys::BanDupes ).toBool() )
                        {
                            reason = "Auto-Banish; Duplicate IP Address: [ %1 ], %2";
                            reason = reason.arg( plr->peerAddress().toString() )
                                           .arg( plr->getBioData() );

                            //Ban for only half an hour.
                            User::addBan( nullptr, plr, reason, false, PunishDurations::THIRTY_MINUTES );

                            emit this->insertLogSignal( server->getServerName(), reason, LogTypes::PUNISHMENT, true, true );

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
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            tmpPlr = server->getPlayer( i );
            if ( ( tmpPlr != nullptr )
              && ( tmpPlr->getSernum_i() == plr->getSernum_i() ) )
            {
                if ( ( tmpPlr != plr )
                     && !plr->getIsDisconnected() )
                {
                    reason = logMsg;
                    reason = reason.arg( "Duplicate SerNum" )
                                   .arg( plr->peerAddress().toString() )
                                   .arg( plr->getBioData() );

                    emit this->insertLogSignal( server->getServerName(), reason, LogTypes::PUNISHMENT, true, true );

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
    if ( banned <= 0 )
        banned = User::getIsPunished( PunishTypes::Ban, ipAddr, PunishTypes::IP, plrSerNum );

    return ( banned >= 1 );
}

void PacketHandler::detectFlooding(Player* plr)
{
    if ( plr == nullptr )
        return;

    int floodCount = plr->getPacketFloodCount();
    if ( floodCount >= 1 )
    {
        qint64 time = plr->getFloodTime();
        if ( time <= PACKET_FLOOD_TIME )
        {
            if ( floodCount >= PACKET_FLOOD_LIMIT
              && !plr->getIsDisconnected() )
            {
                QString logMsg{ "Auto-Mute; Packet Flooding: [ %1 ] sent %2 packets in %3 MS, they are disconnected: [ %4 ]" };
                        logMsg = logMsg.arg( plr->peerAddress().toString() )
                                       .arg( floodCount )
                                       .arg( time )
                                       .arg( plr->getBioData() );

                User::addMute( nullptr, plr, logMsg, false, true, PunishDurations::THIRTY_MINUTES );
                emit this->insertLogSignal( server->getServerName(), logMsg, LogTypes::PUNISHMENT, true, true );

                QString plrMessage{ "Auto-Mute; Packet Flooding." };
                server->sendMasterMessage( plrMessage, plr, false );
            }
        }
        else if ( time >= PACKET_FLOOD_TIME )
        {
            plr->restartFloodTimer();
            plr->setPacketFloodCount( 0 );
        }
    }
}

bool PacketHandler::validatePacketHeader(Player* plr, const QByteArray& pkt)
{
    QString msg{ "Auto-Disconnect; Received Packet with Header [ :SR1%1 ] while assigned [ :SR1%2 ]." };

    QString recvSlotPos{ pkt.mid( 4 ).left( 2 ) };
    qint32 plrPktSlot = plr->getPktHeaderSlot();
    if ( plrPktSlot != Helper::strToInt( recvSlotPos, 16 ) )
    {
        msg = msg.arg( recvSlotPos )
                 .arg( Helper::intToStr( plrPktSlot, 16, 2 ) );
    }
    else //Slot Matched. Return true;
        return true;

    if ( !msg.isEmpty() )
        server->sendMasterMessage( msg, plr, false );

    QString logMsg{ "Auto-Disconnect of [ %1 ] due to an Invalid Packet Header [ :SR1%2 ]< %3 > while assigned [ :SR1%4 ]." };
            logMsg = logMsg.arg( plr->getSernum_s() )
                           .arg( recvSlotPos )
                           .arg( QString( pkt ) )
                           .arg( Helper::intToStr( plrPktSlot, 16, 2 ) );
    emit this->insertLogSignal( server->getServerName(), logMsg, LogTypes::PUNISHMENT, true, true );

    plr->setDisconnected( true, DCTypes::PktDC );
    return false;
}

void PacketHandler::readMIX0(const QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );

    //Send the next Packet to the Scene's Host.
    plr->setTargetScene( sernum.toUInt( nullptr, 16 ) );
    plr->setTargetType( PktTarget::SCENE );
}

void PacketHandler::readMIX1(const QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );
    plr->setSceneHost( sernum.toUInt( nullptr, 16 ) );
}

void PacketHandler::readMIX2(const QString&, Player* plr)
{
    plr->setSceneHost( 0 );
    plr->setTargetType( PktTarget::ALL );
}

void PacketHandler::readMIX3(const QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );

    plr->validateSerNum( server, sernum.toUInt( nullptr, 16 ) );
    this->checkBannedInfo( plr );
}

void PacketHandler::readMIX4(const QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );

    plr->setTargetSerNum( sernum.toUInt( nullptr, 16 ) );
    plr->setTargetType( PktTarget::PLAYER );
}

void PacketHandler::readMIX5(const QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );
    if ( plr != nullptr )
    {
        if ( plr->getSernum_i() <= 0 )
            plr->validateSerNum( server, sernum.toUInt( nullptr, 16 ) );

        //Do not accept comments from User who have been muted.
        if ( !plr->getIsMuted() )
            cmdHandle->parseMix5Command( plr, packet );
    }
}

void PacketHandler::readMIX6(const QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );
    if ( plr != nullptr )
    {
        if ( plr->getSernum_i() <= 0 )
            plr->validateSerNum( server, sernum.toUInt( nullptr, 16 ) );

        //Do not accept commands from User who have been muted.
        if ( !plr->getIsMuted() )
            cmdHandle->parseMix6Command( plr, packet );
    }
}

void PacketHandler::readMIX7(const QString& packet, Player* plr)
{
    if ( plr == nullptr )
        return;

    QString pkt = packet;
            pkt = pkt.mid( 2 );
            pkt = pkt.left( pkt.length() - 2 );

    //Check if the User is banned or requires authentication.
    plr->validateSerNum( server, pkt.toUInt( nullptr, 16 ) );
    this->checkBannedInfo( plr );
}

void PacketHandler::readMIX8(const QString& packet, Player* plr)
{
    this->handleSSVReadWrite( packet, plr, false );
}

void PacketHandler::readMIX9(const QString& packet, Player* plr)
{
    this->handleSSVReadWrite( packet, plr, true );
}

void PacketHandler::handleSSVReadWrite(const QString& packet, Player* plr, const bool write)
{
    if ( plr == nullptr || packet.isEmpty() )
        return;

    qint64 bOut{ 0 };

    QString accessType{ "Read" };
    if ( Settings::getSetting( SKeys::Setting, SSubKeys::AllowSSV ).toBool() )
    {
        QString pkt = packet;
        pkt = pkt.mid( 10 );
        pkt = pkt.left( pkt.length() - 2 );
        QStringList vars = pkt.split( ',' );

        QSettings ssv( "mixVariableCache/" % vars.value( 0 ) % ".ini", QSettings::IniFormat );
        QString sernum = pkt.mid( 2 ).left( 8 );

        QString val{ ":SR@V%1%2,%3,%4,%5\r\n" };
        QDir ssvDir( "mixVariableCache" );
        if ( write )
        {
            accessType = "Write";
            if ( !ssvDir.exists() )
                ssvDir.mkpath( "." );

            ssv.setValue( vars.value( 1 ) % "/" % vars.value( 2 ), vars.value( 3 ) );
        }

        if ( ssvDir.exists() )
        {
            val = val.arg( sernum )
                     .arg( vars.value( 0 ) ) //file name
                     .arg( vars.value( 1 ) ) //category
                     .arg( vars.value( 2 ) ) //variable
                     .arg( ssv.value( vars.value( 1 ) % "/" % vars.value( 2 ), "" ).toString() ); //value

            if ( write )
            {
                Player* tmpPlr{ nullptr };
                for ( int i = 0; i < MAX_PLAYERS; ++i )
                {
                    tmpPlr = server->getPlayer( i );
                    if ( tmpPlr != nullptr
                      && plr != tmpPlr )
                    {
                        bOut = tmpPlr->write( val.toLatin1(), val.length() );
                        server->updateBytesOut( tmpPlr, bOut );
                    }
                }
            }
            else if ( !val.isEmpty()
                   && !write )
            {
                bOut = plr->write( val.toLatin1(), val.length() );
                server->updateBytesOut( plr, bOut );
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
                 .arg( vars.value( 3 ) ); //value

        emit this->insertLogSignal( server->getServerName(), msg, LogTypes::QUEST, true, true );
    }
}
