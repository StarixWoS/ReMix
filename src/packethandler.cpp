
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
#include "user.hpp"

//Qt Includes.
#include <QTcpSocket>
#include <QtCore>

PacketHandler::PacketHandler(ServerInfo* svr, ChatView* chat)
{
    pktForge = PacketForge::getInstance();
    chatView = chat;
    server = svr;

    cmdHandle = new CmdHandler( this, server );
    chatView->setCmdHandle( cmdHandle );
    QObject::connect( cmdHandle, &CmdHandler::newUserCommentSignal,
                      this, &PacketHandler::newUserCommentSignal );
}

PacketHandler::~PacketHandler()
{
    cmdHandle->deleteLater();
}

void PacketHandler::parsePacket(const QByteArray& packet, Player* plr)
{
    QByteArray pkt{ packet };
    if ( plr == nullptr )
        return;

    this->detectFlooding( plr );
    if ( Helper::strStartsWithStr( packet, ":SR" ) )
    {
        if ( !this->checkBannedInfo( plr ) )
        {
            //Prevent Users from Impersonating the Server Admin.
            if ( Helper::strStartsWithStr( pkt, ":SR@" ) )
                return;

            //Prevent Users from changing the Server's rules.
            if ( Helper::strStartsWithStr( pkt, ":SR$" ) )
                return;


            //Ensure Muted Users remain Muted.
            plr->setNetworkMuted( false );
            if ( this->getIsMuted( plr->getSernumHex_s(), plr->getWVar(),
                                   plr->getDVar(), plr->getPublicIP(),
                                   plr->getSernumHex_s() ) )
            {
                plr->setNetworkMuted( true );
            }
            else

            if ( !plr->getNetworkMuted()
              && plr->getIsVisible() )
            {
                //Warpath doesn't send packets using SerNums.
                //Check and skip the validation if this is Warpath.
                if ( server->getGameId() != Games::W97 )
                {
                    if ( !pktForge->validateSerNum( plr, packet ) )
                        return;
                }

                bool parsePkt{ true };
                if ( chatView->getGameID() != Games::Invalid )
                {
                    //Ingame Chat Commands will not be parsed and re-sent to
                    //other Players.
                    parsePkt = chatView->parsePacket( packet, plr );
                }

                if ( parsePkt )
                    this->parseSRPacket( pkt, plr );

            }
        }
    }

    if ( Helper::strStartsWithStr( packet, ":MIX" ) )
        this->parseMIXPacket( packet, plr );
}

void PacketHandler::parseSRPacket(const QByteArray& packet, Player* plr)
{
    QByteArray pkt{ packet };
    if ( plr == nullptr )
        return;

    if ( !pkt.isEmpty() )
        pkt.append( "\r\n" );

    QTcpSocket* tmpSoc{ nullptr };
    Player* tmpPlr{ nullptr };

    qint64 bOut{ 0 };

    //Only parse packets from Users that have entered the correct password.
    if (( plr->getSvrPwdReceived() || !plr->getSvrPwdRequested() )
      && ( plr->getAdminPwdReceived() || !plr->getAdminPwdRequested() ))
    {
        bool isAuth{ false };
        bool send{ false };
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            isAuth = false;
            tmpPlr = nullptr;
            tmpSoc = nullptr;

            tmpPlr = server->getPlayer( i );
            if ( tmpPlr != nullptr
              && tmpPlr->getSocket() != nullptr )
            {
                tmpSoc = tmpPlr->getSocket();
                if ( plr->getSocket() != tmpSoc )
                {
                    if ( ( tmpPlr->getSvrPwdReceived()
                       || !tmpPlr->getSvrPwdRequested() )
                      && ( tmpPlr->getAdminPwdReceived()
                       || !tmpPlr->getAdminPwdRequested() ))
                    {
                        isAuth = true;
                        send = false;

                        quint32 trgScene{ plr->getTargetScene() };
                        if ( plr->getTargetType() == Player::ALL )
                        {
                            send = true;
                        }
                        else if ( plr->getTargetType() == Player::PLAYER )
                        {
                            if ( plr->getTargetSerNum() == tmpPlr->getSernum_i() )
                                send = true;
                        }
                        else if ( plr->getTargetType() == Player::SCENE )
                        {
                            if ((( trgScene == tmpPlr->getSernum_i() )
                              || ( trgScene == tmpPlr->getSceneHost() )))
                            {
                                if (( plr != tmpPlr ) )
                                    send = true;
                            }
                        }
                        else
                            send = false;
                    }

                    if ( send && isAuth )
                    {
                        bOut = tmpSoc->write( pkt, pkt.length() );

                        tmpPlr->setPacketsOut( tmpPlr->getPacketsOut() + 1 );
                        tmpPlr->setBytesOut( tmpPlr->getBytesOut()
                                             + static_cast<quint64>( bOut ) );
                        server->setBytesOut( server->getBytesOut()
                                             + static_cast<quint64>( bOut ) );
                    }
                }
            }
        }
    }

    //Reset the User's target information.
    plr->setTargetType( Player::ALL );
    plr->setTargetSerNum( 0 );
    plr->setTargetScene( 0 );
}

void PacketHandler::parseMIXPacket(const QString& packet, Player* plr)
{
    if ( plr == nullptr )
        return;

    QChar opCode = packet.at( 4 );
    QString data = Helper::getStrStr( packet, "", ":MIX", "" ).mid( 1 );

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
        default:    //Do nothing. Unknown command.
        break;
    }
}

void PacketHandler::parseUDPPacket(const QByteArray& udp, const
                                   QHostAddress& ipAddr,
                                   const quint16& port,
                                   QHash<QHostAddress, QByteArray>* bioHash)
{
    QString logTxt{ "Ignoring UDP from banned %1: "
                    "[ %2 ] sent command: [ %3 ]" };
    bool logMsg{ false };

    QString data{ udp };
    if ( !User::getIsBanned( ipAddr.toString(), BanTypes::IP ) )
    {
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
                if ( Helper::cmpStrings( server->getMasterIP(),
                                         ipAddr.toString() ) )
                {
                    isMaster = true;
                }
            }

            switch ( opCode.toLatin1() )
            {
                case 'G':   //Set the Server's gameInfoString.
                    {
                        //Check if the IP Address is a properly connected User.
                        //Or at least is in the User list...
                        Player* tmpPlr{ nullptr };
                        bool connected{ false };
                        for ( int i = 0; i < MAX_PLAYERS; ++i )
                        {
                            tmpPlr = server->getPlayer( i );
                            if ( tmpPlr != nullptr )
                            {
                                if ( Helper::cmpStrings( tmpPlr->getPublicIP(),
                                                         ipAddr.toString() ) )
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
                            //Check if the World String starts with "world="
                            //And remove the substring.
                            if ( Helper::strStartsWithStr( usrInfo, "world=" ) )
                            {
                                //This event rarely happens, and mainly due to
                                //a certain 3rd party client...
                                //Ahem, ReBreather. *shifty eyes*
                                usrInfo = Helper::getStrStr( usrInfo, "world",
                                                             "=", "=" );
                            }

                            //Enforce a 256 character limit on GameNames.
                            if ( usrInfo.length() > MAX_GAME_NAME_LENGTH )
                            {
                                //Truncate the User's GameInfo String to 256.
                                server->setGameInfo(
                                            usrInfo.left(
                                                MAX_GAME_NAME_LENGTH ) );
                            }
                            else //Length was less than 256, set without issue.
                                server->setGameInfo( usrInfo );
                        }
                    }
                break;
                case 'M':   //Parse the Master Server's response.
                    {
                        //Prevent Spoofing a MasterMix response.
                        if ( !isMaster )
                            break;

                        QString msg{ "Got Response from Master [ %1:%2 ]; "
                                     "it thinks we are [ %3:%4 ]. "
                                     "( Ping: %5 ms, Avg: %6 ms, "
                                     "Trend: %7 ms, Dropped: %8 )" };

                                msg = msg.arg( ipAddr.toString() )
                                         .arg( port );

                        //Store the Master Server's Response Time.
                        server->setMasterPingRespTime(
                                    QDateTime::currentMSecsSinceEpoch() );

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

                            server->setPublicIP( QHostAddress( pubIP )
                                                      .toString() );
                            server->setPublicPort(
                                        static_cast<quint16>(
                                            qFromBigEndian( pubPort ) ) );

                            msg = msg.arg( server->getPublicIP() )
                                     .arg( server->getPublicPort() )
                                     .arg( server->getMasterPing() )
                                     .arg( server->getMasterPingAvg() )
                                     .arg( server->getMasterPingTrend() )
                                     .arg( server->getMasterPingFailCount() );

                            Logger::getInstance()->
                                    insertLog( server->getName(), msg,
                                               LogTypes::USAGE, true, true );
                        }
                    }
                break;
                case 'P':   //Store the Player information into a struct.
                    {
                        index = Helper::getStrIndex( data, "d=" );
                        if ( index >= 0 )
                            dVar = data.mid( index + 2 ).left( 8 );

                        index = Helper::getStrIndex( data, "w=" );
                        if ( index >= 0 )
                            wVar = data.mid( index + 2 ).left( 8 );

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

                        if (( Settings::getReqSernums()
                           && Helper::serNumtoInt( sernum ) )
                          || !Settings::getReqSernums() )
                        {
                            if ( this->getIsBanned( sernum, wVar, dVar,
                                                    ipAddr.toString(), sernum ) )
                            {
                                logTxt = logTxt.arg( "Info",
                                                     ipAddr.toString(),
                                                     data );
                                logMsg = true;
                            }
                            else
                            {
                                server->sendServerInfo( ipAddr, port );
                                bioHash->insert( ipAddr, udp.mid( 1 ) );
                            }
                            User::logBIO( sernum, ipAddr, dVar, wVar, data );
                        }
                        server->setUserPings( server->getUserPings() + 1 );
                    }
                break;
                case 'Q':   //Send Online User Information.
                    {
                        server->sendUserList( ipAddr, port, Q_Response );
                    }
                break;
                case 'R':   //Send Online User Information.
                    {
                        server->sendUserList( ipAddr, port, R_Response );
                    }
                break;
                default:    //Do nothing; Unknown command.
                    qDebug() << "Unknown Command!";
                break;
            }
        }
    }
    else
    {
        logTxt = logTxt.arg( "IP Address" )
                       .arg( ipAddr.toString() )
                       .arg( data );
        logMsg = true;
    }

    if ( logMsg )
    {
        Logger::getInstance()->insertLog( server->getName(), logTxt,
                                          LogTypes::IGNORE, true, true );
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
    if ( this->getIsBanned( plr->getSernumHex_s(), plr->getWVar(),
                            plr->getDVar(), plr->getPublicIP(), plrSerNum ) )
    {
        reason = reason.arg( "Banned Info" )
                       .arg( plr->getPublicIP() )
                       .arg( plr->getBioData() );

        Logger::getInstance()->insertLog( server->getName(), reason,
                                          LogTypes::DC, true, true );

        plrMessage = plrMessage.arg( "Disconnect" )
                               .arg( "Banned Info" );
        plr->sendMessage( plrMessage, false );

        plr->setDisconnected( true, DCTypes::IPDC );
        badInfo = true;
    }

    //Disconnect and ban duplicate IP's if required.
    if ( !Settings::getAllowDupedIP() )
    {
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            tmpPlr = server->getPlayer( i );
            if ( tmpPlr != nullptr
              && tmpPlr != plr )
            {
                if ( ( tmpPlr->getPublicIP() == plr->getPublicIP() )
                  && ( !tmpPlr->getIsDisconnected() || !plr->getIsDisconnected() ) )
                {
                    auto disconnect = [=]( Player* plr, const QString& logMsg,
                                           QString& plrMessage )
                    {
                        QString reason{ logMsg };
                        reason = reason.arg( "Duplicate IP" )
                                       .arg( plr->getPublicIP() )
                                       .arg( plr->getBioData() );

                        Logger::getInstance()->insertLog( server->getName(),
                                                          reason, LogTypes::DC,
                                                          true, true );

                        if ( Settings::getBanDupedIP() )
                        {
                            reason = "Auto-Banish; Duplicate IP "
                                     "Address: [ %1 ], %2";
                            reason = reason.arg( plr->getPublicIP() )
                                           .arg( plr->getBioData() );

                            User::addBan( nullptr, plr, reason, false,
                                          PunishDurations::THIRTY_DAYS );

                            Logger::getInstance()->insertLog(
                                        server->getName(), reason,
                                        LogTypes::BAN, true, true );

                            plrMessage = plrMessage.arg( "Banish" )
                                                   .arg( "Duplicate IP" );
                            plr->sendMessage( plrMessage, false );
                        }
                        else
                        {
                            plrMessage = plrMessage.arg( "Disconnect" )
                                                   .arg( "Duplicate IP" );
                            plr->sendMessage( plrMessage, false );
                        }
                        plr->setDisconnected( true, DCTypes::DupDC );
                    };

                    //Only disconnect the new Player.
                    //If [Settings::getBanDupedIP()] is true
                    //the second Player will be removed.
                    disconnect( plr, logMsg, plrMessage );

                    badInfo = true;
                }
            }
        }
    }

    //Disconnect new Players using the same SerNum as another Player.
    //This is an un-optional disconnect due to how Private chat is handled.
    //Perhaps once a better fix is found we can remove this.

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
                                   .arg( plr->getPublicIP() )
                                   .arg( plr->getBioData() );

                    Logger::getInstance()->insertLog( server->getName(),
                                                      reason, LogTypes::DC,
                                                      true, true );

                    plrMessage = plrMessage.arg( "Disconnect" )
                                           .arg( "Duplicate SerNum" );
                    plr->sendMessage( plrMessage, false );

                    plr->setDisconnected( true, DCTypes::DupDC );
                    badInfo = true;
                }
            }
        }
    }
    return badInfo;
}

bool PacketHandler::getIsBanned(const QString& serNum, const QString& wVar,
                                const QString& dVar, const QString& ipAddr,
                                const QString& plrSerNum) const
{
    bool banned{ false };
    banned = User::getIsBanned( serNum, BanTypes::SerNum, plrSerNum );
    if ( !banned )
        banned = User::getIsBanned( wVar, BanTypes::WV, plrSerNum );

    if ( !banned )
        banned = User::getIsBanned( dVar, BanTypes::DV, plrSerNum );

    if ( !banned )
        banned = User::getIsBanned( ipAddr, BanTypes::IP, plrSerNum );

    return banned;
}

bool PacketHandler::getIsMuted(const QString& serNum, const QString& wVar,
                               const QString& dVar, const QString& ipAddr,
                               const QString& plrSerNum) const
{
    bool muted{ false };
    muted = User::getIsMuted( serNum, BanTypes::SerNum, plrSerNum );
    if ( !muted )
        muted = User::getIsMuted( wVar, BanTypes::WV, plrSerNum );

    if ( !muted )
        muted = User::getIsMuted( dVar, BanTypes::DV, plrSerNum );

    if ( !muted )
        muted = User::getIsMuted( ipAddr, BanTypes::IP, plrSerNum );

    return muted;
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
                QString logMsg{ "Auto-Disconnect; Packet Flooding: [ %1 ] "
                                "sent %2 packets in %3 MS, they are "
                                "disconnected: [ %4 ]" };
                        logMsg = logMsg.arg( plr->getPublicIP()  )
                                       .arg( floodCount )
                                       .arg( time )
                                       .arg( plr->getBioData() );

                Logger::getInstance()->insertLog( server->getName(), logMsg,
                                                  LogTypes::DC, true, true );

                QString plrMessage{ "Auto-Disconnect; Packet Flooding" };
                plr->sendMessage( plrMessage, false );
                plr->setDisconnected( true, DCTypes::PktDC );
            }
        }
        else if ( time >= PACKET_FLOOD_TIME )
        {
            plr->restartFloodTimer();
            plr->setPacketFloodCount( 0 );
        }
    }
}

void PacketHandler::readMIX0(const QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );

    //Send the next Packet to the Scene's Host.
    plr->setTargetScene( sernum.toUInt( nullptr, 16 ) );
    plr->setTargetType( Player::SCENE );
}

void PacketHandler::readMIX1(const QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );
    plr->setSceneHost( sernum.toUInt( nullptr, 16 ) );
}

void PacketHandler::readMIX2(const QString&, Player* plr)
{
    plr->setSceneHost( 0 );
    plr->setTargetType( Player::ALL );
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
    plr->setTargetType( Player::PLAYER );
}

void PacketHandler::readMIX5(const QString& packet, Player* plr)
{
    cmdHandle->parseMix5Command( plr, packet );
}

void PacketHandler::readMIX6(const QString& packet, Player* plr)
{
    cmdHandle->parseMix6Command( plr, packet );
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
    QDir ssvDir( "mixVariableCache" );
    if ( ssvDir.exists() )
    {
        QString pkt = packet;
                pkt = pkt.mid( 10 );
                pkt = pkt.left( pkt.length() - 2 );

        QString sernum = pkt.mid( 2 ).left( 8 );
        QStringList vars = pkt.split( ',' );
        QString val{ ":SR@V%1%2,%3,%4,%5\r\n" };

        if ( Settings::getAllowSSV() )
        {
            QSettings ssv( "mixVariableCache/" % vars.value( 0 ) % ".ini",
                           QSettings::IniFormat );
            val = val.arg( sernum )
                     .arg( vars.value( 0 ) )
                     .arg( vars.value( 1 ) )
                     .arg( vars.value( 2 ) )
                     .arg( ssv.value( vars.value( 1 )
                                    % "/"
                                    % vars.value( 2 ), "" ).toString() );
        }
        else
            val = "";

        QTcpSocket* soc{ plr->getSocket() };
        if ( !val.isEmpty()
          && soc != nullptr )
        {
            soc->write( val.toLatin1(), val.length() );
        }
    }
}

void PacketHandler::readMIX9(const QString& packet, Player*)
{
    QDir ssvDir( "mixVariableCache" );
    if ( !ssvDir.exists() )
        ssvDir.mkpath( "." );

    QString pkt = packet;
            pkt = pkt.mid( 10 );
            pkt = pkt.left( pkt.length() - 2 );

    QStringList vars = pkt.split( ',' );
    if ( Settings::getAllowSSV() )
    {
        QSettings ssv( "mixVariableCache/" % vars.value( 0 ) % ".ini",
                       QSettings::IniFormat );

        ssv.setValue( vars.value( 1 ) % "/" % vars.value( 2 ),
                      vars.value( 3 ) );
    }
}
