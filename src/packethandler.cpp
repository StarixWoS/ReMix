
//Class includes.
#include "packethandler.hpp"

//ReMix includes.
#include "packetforge.hpp"
#include "cmdhandler.hpp"
#include "serverinfo.hpp"
#include "settings.hpp"
#include "chatview.hpp"
#include "helper.hpp"
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
    QObject::connect( cmdHandle, &CmdHandler::newUserCommentSignal,
                      this, &PacketHandler::newUserCommentSignal );
}

PacketHandler::~PacketHandler()
{
    cmdHandle->deleteLater();
}

void PacketHandler::parsePacket(const QString& packet, Player* plr)
{
    if ( plr == nullptr )
        return;

    this->detectFlooding( plr );
    if ( Helper::strStartsWithStr( packet, ":SR" ) )
    {
        if ( !!this->checkBannedInfo( plr ) )
        {
            //Prevent Users from Impersonating the Server Admin.
            if ( Helper::strStartsWithStr( packet, ":SR@" ) )
                return;

            //Prevent Users from changing the Server's rules.
            if ( Helper::strStartsWithStr( packet, ":SR$" ) )
                return;

            if ( !plr->getNetworkMuted() )
            {
                if ( !pktForge->validateSerNum( plr, packet ) )
                    return;

                this->parseSRPacket( packet, plr );

                if ( chatView->getGameID() != Games::Invalid )
                {
                    if ( chatView->getGameID() == Games::W97 )
                    {
                        //Handle Warpath Packets.
                        chatView->parsePacket( packet, plr->getAlias() );
                    }
                    else if ( chatView->getGameID() != Games::Invalid )
                    {
                        //Handle WoS and Arcadia Packets.
                        chatView->parsePacket( packet );
                    }
                }
            }
        }
    }

    if ( Helper::strStartsWithStr( packet, ":MIX" ) )
        this->parseMIXPacket( packet, plr );
}

void PacketHandler::parseSRPacket(const QString& packet, Player* plr)
{
    if ( plr == nullptr )
        return;

    QString pkt = packet;
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
                        bOut = tmpSoc->write( pkt.toLatin1(),
                                              pkt.length() );

                        tmpPlr->setPacketsOut( tmpPlr->getPacketsOut() + 1 );
                        tmpPlr->setBytesOut( tmpPlr->getBytesOut() + bOut );
                        server->setBytesOut( server->getBytesOut() + bOut );
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
            return;
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
    if ( !User::getIsBanned( ipAddr.toString(), User::tIP ) )
    {
        QString sernum{ "" };
        QString dVar{ "" };
        QString wVar{ "" };

        qint32 index{ 0 };
        if ( !data.isEmpty() )
        {
            switch ( data.at( 0 ).toLatin1() )
            {
                case 'G':   //Set the Server's gameInfoString.
                    {
                        QString svrInfo{ server->getGameInfo() };
                        QString usrInfo{ data.mid( 1 ) };
                        if ( svrInfo.isEmpty() && !usrInfo.isEmpty() )
                        {
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
                            if ( User::getIsBanned( sernum, User::tSERNUM )
                              || User::getIsBanned( wVar, User::tWV )
                              || User::getIsBanned( dVar, User::tDV ) )
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
        logTxt = logTxt.arg( "IP Address",
                             ipAddr.toString(),
                             data );
        logMsg = true;
    }

    if ( logMsg )
        Helper::logToFile( Helper::IGNORE, logTxt, true, true );
}

bool PacketHandler::checkBannedInfo(Player* plr) const
{
    if ( plr == nullptr )
        return true;

    Player* tmpPlr{ nullptr };

    bool badInfo{ true };

    QString logMsg{ "Auto-Disconnect; %1: [ %2 ], [ %3 ]" };
    QString reason{ logMsg };

    //Prevent Banned IP's or SerNums from remaining connected.
    if ( User::getIsBanned( plr->getPublicIP(), User::tIP )
      || User::getIsBanned( plr->getSernumHex_s(), User::tSERNUM )
      || User::getIsBanned( plr->getWVar(), User::tWV )
      || User::getIsBanned( plr->getDVar(), User::tDV ) )
    {
        reason = reason.arg( "Banned Info",
                             plr->getPublicIP(),
                             plr->getBioData() );
        Helper::logToFile( Helper::DC, reason, true, true );

        plr->setDisconnected( true, DCTypes::IPDC );
        badInfo = true;
    }

    //Disconnect and ban all duplicate IP's if required.
    if ( !Settings::getAllowDupedIP() )
    {
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            tmpPlr = server->getPlayer( i );
            if ( tmpPlr != nullptr
              && tmpPlr != plr )
            {
                if ( tmpPlr->getPublicIP() == plr->getPublicIP() )
                {
                    auto disconnect = [=]( Player* plr, const QString& logMsg )
                    {
                        QString reason{ logMsg };
                        reason = reason.arg( "Duplicate IP",
                                             plr->getPublicIP(),
                                             plr->getBioData() );
                        Helper::logToFile( Helper::DC, reason,
                                           true, true );

                        if ( Settings::getBanDupedIP() )
                        {
                            reason = "Auto-Banish; Duplicate IP "
                                     "Address: [ %1 ], %2";
                            reason = reason.arg( plr->getPublicIP(),
                                                 plr->getBioData() );

                            User::addBan( nullptr, plr, reason );
                            Helper::logToFile( Helper::BAN, reason,
                                               true, true );
                        }
                        plr->setDisconnected( true, DCTypes::DUPDC );
                    };

                    disconnect( plr, logMsg );
                    disconnect( tmpPlr, logMsg );

                    badInfo = true;
                }
            }
        }
    }

    //Disconnect new Players using the same SerNum.
    //This is an un-optional disconnect
    //due to how Private chat is handled.
    //Perhaps once a better fix is found we can remove this.
    if ( !badInfo )
    {
        //The User has the same SerNum as another User
        //and is connecting from a different IP Address.
        //Disconnect the newly connected User.
        if ( plr != nullptr )
        {
            for ( int i = 0; i < MAX_PLAYERS; ++i )
            {
                tmpPlr = server->getPlayer( i );
                if ( tmpPlr != nullptr
                     && tmpPlr->getSernum_i() == plr->getSernum_i() )
                {
                    if ( tmpPlr != plr )
                    {
                        reason = logMsg;
                        reason = reason.arg( "Duplicate SerNum",
                                             tmpPlr->getPublicIP(),
                                             tmpPlr->getBioData() );
                        Helper::logToFile( Helper::DC, reason, true, true );

                        plr->setDisconnected( true, DCTypes::DUPDC );
                        badInfo = true;
                    }
                }
            }
        }
    }
    return badInfo;
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
              && !plr->getDisconnected() )
            {
                QString logMsg{ "Auto-Disconnect; Packet Flooding: [ %1 ] "
                                "sent %2 packets in %3 MS, they are "
                                "disconnected: [ %4 ]" };
                        logMsg = logMsg.arg( plr->getPublicIP(),
                                             QString::number(
                                                 floodCount ),
                                             QString::number(
                                                 time ),
                                             plr->getBioData() );
                Helper::logToFile( Helper::DC, logMsg, true, true );

                if ( Settings::getBanDeviants() )
                {
                    logMsg = "Auto-Banish; Suspicious data from: "
                             "[ %1 ], [ %2 ]";
                    logMsg = logMsg.arg( plr->getPublicIP(),
                                         plr->getBioData() );

                    User::addBan( nullptr, plr, logMsg );
                    Helper::logToFile( Helper::BAN, logMsg, true, true );
                }
                plr->setDisconnected( true, DCTypes::PKTDC );
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
    plr->setTargetScene( sernum.toUInt( 0, 16 ) );
    plr->setTargetType( Player::SCENE );
}

void PacketHandler::readMIX1(const QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );
    plr->setSceneHost( sernum.toUInt( 0, 16 ) );
}

void PacketHandler::readMIX2(const QString&, Player* plr)
{
    plr->setSceneHost( 0 );
    plr->setTargetType( Player::ALL );
}

void PacketHandler::readMIX3(const QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );

    plr->validateSerNum( server, sernum.toUInt( 0, 16 ) );
    this->checkBannedInfo( plr );
}

void PacketHandler::readMIX4(const QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );

    plr->setTargetSerNum( sernum.toUInt( 0, 16 ) );
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
    plr->validateSerNum( server, pkt.toUInt( 0, 16 ) );
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
        QString val{ "" };

        if ( Settings::getAllowSSV() )
        {
            QSettings ssv( "mixVariableCache/" % vars.value( 0 ) % ".ini",
                           QSettings::IniFormat );
            val = QString( ":SR@V%1%2,%3,%4,%5\r\n" )
                      .arg( sernum,
                            vars.value( 0 ),
                            vars.value( 1 ),
                            vars.value( 2 ),
                            ssv.value( vars.value( 1 ) % "/" %
                                       vars.value( 2 ), "" )
                               .toString() );
        }

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
