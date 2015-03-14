
#include "includes.hpp"
#include "packethandler.hpp"

PacketHandler::PacketHandler(Admin* adm, ServerInfo* svr)
{
    server = svr;
    admin = adm;

    cmdHandle = new CmdHandler( this, server, admin );
    QObject::connect( cmdHandle, &CmdHandler::newUserCommentSignal,
                      this, &PacketHandler::newUserCommentSignal );

#ifdef DECRYPT_PACKET_PLUGIN
    this->loadPlugin();
#endif
}

PacketHandler::~PacketHandler()
{

#ifdef DECRYPT_PACKET_PLUGIN
    if ( pluginManager != nullptr )
        pluginManager->unload();
#endif

    cmdHandle->deleteLater();
}

void PacketHandler::parsePacket(QString& packet, Player* plr)
{
    if ( plr == nullptr )
        return;

    this->detectFlooding( plr );
    if ( packet.startsWith( ":SR", Qt::CaseInsensitive ) )
    {
        if ( !!this->checkBannedInfo( plr ) )
        {
            //Prevent Users from Impersonating the Server Admin.
            if ( packet.startsWith( ":SR@", Qt::CaseInsensitive ) )
                return;

            //Prevent Users from changing the Server's rules.
            if ( packet.startsWith( ":SR$", Qt::CaseInsensitive ) )
                return;

            //Prevent re-sending packets from Users with a muted Network.
            if ( !plr->getNetworkMuted() )
                this->parseSRPacket( packet, plr );
        }
    }

    if ( packet.startsWith( ":MIX", Qt::CaseInsensitive ) )
        this->parseMIXPacket( packet, plr );
}

void PacketHandler::parseSRPacket(QString& packet, Player* plr)
{
    if ( plr == nullptr )
        return;

    if ( !packet.isEmpty() )
        packet.append( "\r\n" );

    QTcpSocket* tmpSoc{ nullptr };
    Player* tmpPlr{ nullptr };

    quint64 bOut{ 0 };

    //Only parse packets from Users that have entered the correct password.
    if (( plr->getEnteredPwd() || !plr->getPwdRequested() )
      && ( plr->getGotAuthPwd() || !plr->getReqAuthPwd() ))
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
                    if (( tmpPlr->getEnteredPwd() || !tmpPlr->getPwdRequested() )
                      && ( tmpPlr->getGotAuthPwd() || !tmpPlr->getReqAuthPwd() ))
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
                        bOut = tmpSoc->write( packet.toLatin1(),
                                              packet.length() );

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

void PacketHandler::parseMIXPacket(QString& packet, Player* plr)
{
    if ( plr == nullptr )
        return;

    QChar opCode = packet.at( 4 );
    QString tmp = Helper::getStrStr( packet, "", ":MIX", "" ).mid( 1 );

    switch ( opCode.toLatin1() )
    {
        case '0':   //Send Next Packet to Scene.
            this->readMIX0( tmp, plr );
        break;
        case '1':   //Register Player within SerNum's Scene.
            this->readMIX1( tmp, plr );
        break;
        case '2':   //Unknown.
            this->readMIX2( tmp, plr );
        break;
        case '3':   //Attune a Player to thier SerNum for private messaging.
            this->readMIX3( tmp, plr );
        break;
        case '4':   //Send the next Packet from the User to SerNum's Socket.
            this->readMIX4( tmp, plr );
        break;
        case '5':   //Handle Server password login and User Comments.
            this->readMIX5( tmp, plr );
        break;
        case '6':   //Handle Remote Admin Commands.
            this->readMIX6( tmp, plr );
        break;
        case '7':   //Set the User's HB ID.
            this->readMIX7( tmp, plr );
        break;
        case '8':   //Set/Read SSV Variable.
            this->readMIX8( tmp, plr );
        break;
        case '9':   //Set/Read SSV Variable.
            this->readMIX9( tmp, plr );
        break;
        default:    //Do nothing. Unknown command.
            return;
        break;
    }
}

void PacketHandler::parseUDPPacket(QByteArray& udp, QHostAddress& ipAddr,
                                   quint16 port, QHash<QHostAddress,
                                                      QByteArray>* bioHash)
{
    QString logTxt{ "Ignoring UDP from banned %1: "
                    "[ %2:%3 ] sent command: [ %4 ]" };
    QString log{ "logs/Ignored.txt" };
    bool logMsg{ false };

    QString data{ udp };
    if ( !BanDialog::getIsBanned( ipAddr.toString() ) )
    {
        if ( !data.isEmpty() )
        {
            qint32 index{ data.indexOf( "d=", Qt::CaseInsensitive ) };
            QString dVar{ data.mid( index + 2 ).left( 8 ) };

            index = data.indexOf( "w=", Qt::CaseInsensitive );
            QString wVar{ data.mid( index + 2 ).left( 8 ) };

            QString sernum = Helper::getStrStr( data, "sernum", "=", "," );
            sernum = Helper::serNumToHexStr( sernum );

            switch ( data.at( 0 ).toLatin1() )
            {
                case 'G':   //Set the Server's gameInfoString.
                    {
                        server->setGameInfo( data.mid( 1 ) );
                    }
                break;
                case 'M':   //Parse the Master Server's response.
                    {
                        int opcode{ 0 };
                        int pubIP{ 0 };
                        int pubPort{ 0 };

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
                        //We've obtained a Master response.
                        server->setMasterUDPResponse( true );
                    }
                break;
                case 'P':   //Store the Player information into a struct.
                    if ( !sernum.isEmpty() && server->getLogFiles() )
                        Helper::logBIOData( sernum, ipAddr, port, data );

                    if (( Settings::getReqSernums()
                       && Helper::serNumtoInt( sernum ) )
                      || !Settings::getReqSernums() )
                    {
                        if ( BanDialog::getIsBanned( sernum )
                          || BanDialog::getIsBanned( wVar )
                          || BanDialog::getIsBanned( dVar ) )
                        {
                            logTxt = logTxt.arg( "Info" )
                                           .arg( ipAddr.toString() )
                                           .arg( port )
                                           .arg( data );
                            logMsg = true;
                        }
                        else
                        {
                            bioHash->insert( ipAddr, udp.mid( 1 ) );
                            server->sendServerInfo( ipAddr, port );
                        }
                    }
                break;
                case 'Q':   //Send Online User Information.
                    if (( Settings::getReqSernums()
                       && Helper::serNumtoInt( sernum ) )
                      || !Settings::getReqSernums() )
                    {
                        if ( BanDialog::getIsBanned( sernum )
                          || BanDialog::getIsBanned( wVar )
                          || BanDialog::getIsBanned( dVar ) )
                        {
                            logTxt = logTxt.arg( "Info" )
                                           .arg( ipAddr.toString() )
                                           .arg( port )
                                           .arg( data );
                            logMsg = true;
                        }
                        else
                            server->sendUserList( ipAddr, port );
                    }
                break;
                case 'R':   //TODO: Command "R" with unknown use.
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
                       .arg( port )
                       .arg( data );
        logMsg = true;
    }

    if ( logMsg )
        Helper::logToFile( log, logTxt, true, true );
}

bool PacketHandler::checkBannedInfo(Player* plr)
{
    //TODO: Check for banned D and V variables. --Low Priority.
    if ( plr == nullptr )
        return true;

    BanDialog* bandlg = admin->getBanDialog();
    Player* tmpPlr{ nullptr };

    bool badInfo{ true };

    QString log{ "logs/DCLog.txt" };

    QString logMsg{ "Auto-Disconnect; %1: [ %2:%3 ], [ %4 ]" };
    QString tmpMsg{ logMsg };

    //Prevent Banned IP's or SerNums from remaining connected.
    if ( BanDialog::getIsBanned( plr->getPublicIP() )
      || BanDialog::getIsBanned( plr->getSernumHex_s() )
      || BanDialog::getIsBanned( plr->getWVar() )
      || BanDialog::getIsBanned( plr->getDVar() ) )
    {
        plr->setDisconnected( true );
        server->setIpDc( server->getIpDc() + 1 );

        server->sendMasterMessage( Settings::getBanishMesage(), plr, false );

        tmpMsg = tmpMsg.arg( "Banned Info" )
                       .arg( plr->getPublicIP() )
                       .arg( plr->getPublicPort() )
                       .arg( plr->getBioData() );

        Helper::logToFile( log, tmpMsg, true, true );
        badInfo = true;
    }

    //Disconnect and ban all duplicate IP's if required.
    if ( !Settings::getAllowDupedIP() )
    {
        QString reason{ "Auto-Banish; Duplicate IP Address: [ %1:%2 ]: %3" };
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            tmpPlr = server->getPlayer( i );
            if ( tmpPlr != nullptr
              && tmpPlr != plr )
            {
                if ( tmpPlr->getPublicIP() == plr->getPublicIP() )
                {
                    reason = reason.arg( plr->getPublicIP() )
                                   .arg( plr->getPublicPort() )
                                   .arg( plr->getBioData() );

                    if ( Settings::getBanDupedIP() )
                        bandlg->addBan( plr, reason );

                    if ( plr != nullptr )
                    {
                        tmpMsg = logMsg;
                        tmpMsg = tmpMsg.arg( "Duplicate IP" )
                                       .arg( plr->getPublicIP() )
                                       .arg( plr->getPublicPort() )
                                       .arg( plr->getBioData() );
                        Helper::logToFile( log, tmpMsg, true, true );

                        plr->setDisconnected( true );
                        server->setIpDc( server->getIpDc() + 1 );
                        server->setDupDc( server->getDupDc() + 1 );
                    }

                    if ( tmpPlr != nullptr )
                    {
                        tmpMsg = logMsg;
                        tmpMsg = tmpMsg.arg( "Duplicate IP" )
                                       .arg( tmpPlr->getPublicIP() )
                                       .arg( tmpPlr->getPublicPort() )
                                       .arg( tmpPlr->getBioData() );
                        Helper::logToFile( log, tmpMsg, true, true );

                        tmpPlr->setDisconnected( true );
                        server->setIpDc( server->getIpDc() + 1 );
                        server->setDupDc( server->getDupDc() + 1 );
                    }
                    badInfo = true;
                }
            }
        }
    }

    //Disconnect new Players using the same SerNum.
    //This is an un-optional disconnect
    //due to how Private chat is handled.
    //Perhaps once a better fix is found we can remove this.
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
                    tmpMsg = logMsg;
                    tmpMsg = tmpMsg.arg( "Duplicate SerNum" )
                                   .arg( tmpPlr->getPublicIP() )
                                   .arg( tmpPlr->getPublicPort() )
                                   .arg( tmpPlr->getBioData() );
                    Helper::logToFile( log, tmpMsg, true, true );

                    plr->setDisconnected( true );
                    server->setDupDc( server->getDupDc() + 1 );

                    badInfo = true;
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
        quint64 time = plr->getFloodTime();
        if ( time <= PACKET_FLOOD_TIME )
        {
            if ( floodCount >= PACKET_FLOOD_LIMIT
              && !plr->getDisconnected() )
            {
                QString log{ "logs/DCLog.txt" };
                QString logMsg{ "Auto-Disconnect; Packet Flooding: [ %1:%2 ] "
                                "sent %3 packets in %4 MS, they are "
                                "disconnected: [ %5 ]" };
                        logMsg = logMsg.arg( plr->getPublicIP() )
                                       .arg( plr->getPublicPort() )
                                       .arg( floodCount )
                                       .arg( time )
                                       .arg( plr->getBioData() );
                Helper::logToFile( log, logMsg, true, true );

                if ( Settings::getBanHackers() )
                {
                    BanDialog* banDlg = admin->getBanDialog();
                    if ( banDlg != nullptr )
                    {
                        log = "logs/BanLog.txt";
                        logMsg = "Auto-Banish; Suspicious data from: "
                                 "[ %1:%2 ]: [ %3 ]";
                        logMsg = logMsg.arg( plr->getPublicIP() )
                                       .arg( plr->getPublicPort() )
                                       .arg( plr->getBioData() );
                        Helper::logToFile( log, logMsg, true, true );

                        banDlg->addBan( plr, logMsg );
                    }
                }
                plr->setDisconnected( true );
                server->setPktDc( server->getPktDc() + 1 );
            }
        }
        else if ( time >= PACKET_FLOOD_TIME )
        {
            plr->restartFloodTimer();
            plr->setPacketFloodCount( 0 );
        }
    }
}

void PacketHandler::readMIX0(QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );

    //Send the next Packet to the Scene's Host.
    plr->setTargetScene( sernum.toUInt( 0, 16 ) );
    plr->setTargetType( Player::SCENE );
}

void PacketHandler::readMIX1(QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );
    plr->setSceneHost( sernum.toUInt( 0, 16 ) );
}

void PacketHandler::readMIX2(QString&, Player* plr)
{
    plr->setSceneHost( 0 );
    plr->setTargetType( Player::ALL );
}

void PacketHandler::readMIX3(QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );

    plr->validateSerNum( server, sernum.toUInt( 0, 16 ) );
    this->checkBannedInfo( plr );
}

void PacketHandler::readMIX4(QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );

    plr->setTargetSerNum( sernum.toUInt( 0, 16 ) );
    plr->setTargetType( Player::PLAYER );
}

void PacketHandler::readMIX5(QString& packet, Player* plr)
{
    cmdHandle->parseMix5Command( plr, packet );
}

void PacketHandler::readMIX6(QString& packet, Player* plr)
{
    cmdHandle->parseMix6Command( plr, packet );
}

void PacketHandler::readMIX7(QString& packet, Player* plr)
{
    if ( plr == nullptr )
        return;

    packet = packet.mid( 2 );
    packet = packet.left( packet.length() - 2 );

    //Check if the User is banned or requires authentication.
    plr->validateSerNum( server, packet.toUInt( 0, 16 ) );
    this->checkBannedInfo( plr );
}

void PacketHandler::readMIX8(QString& packet, Player* plr)
{
    QDir ssvDir( "mixVariableCache" );
    if ( ssvDir.exists() )
    {
        QString sernum = packet.mid( 2 ).left( 8 );

        packet = packet.mid( 10 );
        packet = packet.left( packet.length() - 2 );

        QStringList vars = packet.split( ',' );
        QString val{ "" };

        if ( Settings::getAllowSSV()
          && !vars.contains( "Admin", Qt::CaseInsensitive ))
        {
            QSettings ssv( "mixVariableCache/" % vars.value( 0 ) % ".ini",
                           QSettings::IniFormat );
            val = QString( ":SR@V%1%2,%3,%4,%5\r\n" )
                      .arg( sernum )
                      .arg( vars.value( 0 ) )
                      .arg( vars.value( 1 ) )
                      .arg( vars.value( 2 ) )
                      .arg( ssv.value( vars.value( 1 ) % "/" %
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

void PacketHandler::readMIX9(QString& packet, Player*)
{
    QDir ssvDir( "mixVariableCache" );
    if ( !ssvDir.exists() )
        ssvDir.mkpath( "." );

    packet = packet.mid( 10 );
    packet = packet.left( packet.length() - 2 );

    QStringList vars = packet.split( ',' );
    if ( Settings::getAllowSSV()
      && !vars.contains( "Admin", Qt::CaseInsensitive ))
    {
        QSettings ssv( "mixVariableCache/" % vars.value( 0 ) % ".ini",
                       QSettings::IniFormat );

        ssv.setValue( vars.value( 1 ) % "/" % vars.value( 2 ),
                      vars.value( 3 ) );
    }
}


#ifdef DECRYPT_PACKET_PLUGIN
bool Server::loadPlugin()
{
    QDir pluginsDir( qApp->applicationDirPath() );

    #if defined( Q_OS_WIN )
        if ( pluginsDir.dirName().toLower() == QLatin1String( "debug" )
          || pluginsDir.dirName().toLower() == QLatin1String( "release" ) )
        {
            pluginsDir.cdUp();
        }

    #elif defined(Q_OS_MAC)
        if ( pluginsDir.dirName() == QLatin1String( "MacOS" ) )
        {
            pluginsDir.cdUp();
            pluginsDir.cdUp();
            pluginsDir.cdUp();
        }
    #endif

    pluginsDir.cd( "plugins" );
    foreach ( QString fileName, pluginsDir.entryList( QDir::Files ) )
    {
        QPluginLoader pluginLoader( pluginsDir.absoluteFilePath( fileName ) );
        QObject *plugin = pluginLoader.instance();
        if ( plugin )
        {
            packetInterface = qobject_cast<PacketDecryptInterface*>( plugin );
            if ( packetInterface )
                return true;
        }
    }
    return false;
}
#endif
