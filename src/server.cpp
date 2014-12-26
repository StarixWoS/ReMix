
#include "includes.hpp"

Server::Server(QWidget* parent, ServerInfo* svr, Admin* adminDlg, QStandardItemModel* plrView)
{
    //Setup Objects.
    mother = parent;
    server = svr;
    plrViewModel = plrView;
    admin = adminDlg;

    //Every 2 Seconds we will attempt to Obtain Master Info.
    //This will be set to 300000 (5-Minutes) once Master info is obtained.
    masterCheckIn.setInterval( 2000 );

    //Setup Objects.
    serverComments = new UserMessage( parent );

    //Connect Objects.
    QObject::connect( this, &QTcpServer::newConnection,
                      this, &Server::newConnectionSlot );

    QObject::connect( this, &Server::newUserCommentSignal,
                      serverComments, &UserMessage::newUserCommentSlot );

    QObject::connect( server->getMasterSocket(), &QUdpSocket::readyRead,
                      this, &Server::readyReadUDPSlot );

    QObject::connect( &masterCheckIn, &QTimer::timeout, [=]()
    {
        server->sendMasterInfo();
    });

    //Ensure all possible User slots are fillable.
    this->setMaxPendingConnections( MAX_PLAYERS );

#ifdef DECRYPT_PACKET_PLUGIN
    this->loadPlugin();
#endif
}

Server::~Server()
{
    serverComments->close();
    serverComments->deleteLater();

#ifdef DECRYPT_PACKET_PLUGIN
    if ( pluginManager != nullptr )
        pluginManager->unload();
#endif

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        server->deletePlayer( i );
    }
    udpDatas.clear();
}

void Server::checkBannedInfo(Player* plr)
{
    //TODO: Check for banned D and V variables. --Low Priority.
    if ( plr == nullptr )
        return;

    BanDialog* bandlg = admin->getBanDialog();
    Player* tmpPlr{ nullptr };

    //Prevent Banned IP's or SerNums from remaining connected.
    if ( bandlg->getIsIPBanned( plr->getPublicIP() )
      || bandlg->getIsSernumBanned( plr->getSernum_s() ) )
    {
        plr->setSoftDisconnect( true );
        server->setIpDc( server->getIpDc() + 1 );

        server->sendMasterMessage( Helper::getBanishMesage(), plr, false );
    }

    //Disconnect and ban all duplicate IP's if required.
    if ( !Helper::getAllowDupedIP() )
    {
        QString reason{ "Auto-Banish; Duplicate IP Address: [ %1:%2 }: %3" };
        QString peerAddr{ plr->getPublicIP() };

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
                                   .arg( QString( plr->getBioData() ) );

                    if ( Helper::getBanDupedIP() )
                        bandlg->addIPBan( peerAddr, reason );

                    if ( plr != nullptr )
                    {
                        plr->setHardDisconnect( true );
                        server->setIpDc( server->getIpDc() + 1 );
                        server->setDupDc( server->getDupDc() + 1 );
                    }

                    if ( tmpPlr != nullptr )
                    {
                        tmpPlr->setHardDisconnect( true );
                        server->setIpDc( server->getIpDc() + 1 );
                        server->setDupDc( server->getDupDc() + 1 );
                    }
                }
            }
        }
    }

    //Disconnect new Players using the same SerNum. This is an un-optional disconnect
    //due to how Private chat is handled. --Perhaps once a better fix is found we can remove this.
    if ( plr != nullptr )
    {
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            tmpPlr = server->getPlayer( i );
            if ( tmpPlr != nullptr
              && tmpPlr->getSernum() == plr->getSernum() )
            {
                if ( tmpPlr != plr )
                {
                    plr->setHardDisconnect( true );
                    server->setDupDc( server->getDupDc() + 1 );
                }
            }
        }
    }
}

void Server::detectPacketFlood(Player* plr)
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
              && !plr->getHardDisconnect() )
            {
                QString log{ QDate::currentDate().toString( "banLog/yyyy-MM-dd.txt" ) };
                QString logMsg{ "Auto-Disconnect; Packet Flooding: [ %1:%2 ] sent %3 packets in %4 MS, he is disconnected!" };
                        logMsg = logMsg.arg( plr->getPublicIP() )
                                       .arg( plr->getPublicPort() )
                                       .arg( floodCount )
                                       .arg( time );

                Helper::logToFile( log, logMsg, true, true );
                if ( Helper::getBanHackers() )
                {
                    BanDialog* banDlg = admin->getBanDialog();
                    if ( banDlg != nullptr )
                    {
                        logMsg = QString( "Auto-Banish; Suspicious data from: [ %1:%2 ]: %3" );
                        logMsg = logMsg.arg( plr->getPublicIP() )
                                       .arg( plr->getPublicPort() )
                                       .arg( QString( plr->getBioData() ) );

                        QString ip{ plr->getPublicIP() };
                        banDlg->addIPBan( ip, logMsg );
                    }
                }
                plr->setHardDisconnect( true );
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

QStandardItem* Server::updatePlrListRow(QString& peerIP, QByteArray& data, Player* plr, bool insert)
{
    QString bio = QString( data );
    int row{ -1 };

    QStandardItem* item;
    if ( !insert )
    {
        item = plrTableItems.value( peerIP );
        row = item->row();
    }
    else
    {
        row = plrViewModel->rowCount();
        plrViewModel->insertRow( row );
    }

    plrViewModel->setData( plrViewModel->index( row, 0 ), peerIP, Qt::DisplayRole );
    if ( !bio.isEmpty() )
    {
        QString sernum = Helper::getStrStr( bio, "sernum", "=", "," );
        if ( !sernum.isEmpty() )
        {
            plr->setSernum( Helper::serNumToHexStr( sernum ).toUInt( 0, 16 ) );
            plr->setSernum_s( sernum );
        }
        plrViewModel->setData( plrViewModel->index( row, 1 ),
                               sernum,
                               Qt::DisplayRole );

        plrViewModel->setData( plrViewModel->index( row, 2 ),
                               Helper::getStrStr( bio, "HHMM", "=", "," ),
                               Qt::DisplayRole );

        plrViewModel->setData( plrViewModel->index( row, 3 ),
                               Helper::getStrStr( bio, "alias", "=", "," ),
                               Qt::DisplayRole );

        plrViewModel->setData( plrViewModel->index( row, 7 ), bio, Qt::DisplayRole );
    }
    return plrViewModel->item( row, 0 );
}

void Server::showServerComments()
{
    if ( serverComments->isVisible() )
        serverComments->hide();
    else
        serverComments->show();
}

void Server::setupServerInfo()
{
    if ( !server->getIsSetUp() )
    {
        server->setPrivateIP( QHostAddress( QHostAddress::LocalHost ).toString() );

        QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
        for ( int i = 0; i < ipList.size(); ++i )
        {
            QString tmp = ipList.at( i ).toString();
            if ( ipList.at( i ) != QHostAddress::LocalHost
              && ipList.at( i ).toIPv4Address()
              && !Helper::isInvalidIPAddress( tmp ) )
            {
                server->setPrivateIP( ipList.at(i).toString() );   //Use first non-local IP address.
                break;
            }
        }

        QHostAddress addr{ server->getPrivateIP() };
        bool validUDP = server->initMasterSocket( addr, server->getPrivatePort() );
        bool validTCP = this->listen( addr, server->getPrivatePort() );

        if ( !validUDP
          || !validTCP )
        {
            QString title{ "Invalid Port [ %1 ]" };
                    title = title.arg( server->getPrivatePort() );

            QString prompt{ "The selected UDP/TCP Port [ %1 ]\r\n"
                            "is either invalid or already in use." };
                    prompt = prompt.arg( server->getPrivatePort() );

            Helper::warningMessage( mother, title, prompt );
        }

        server->setIsSetUp( true );
        if ( server->getIsPublic() && this->isListening() )
            server->sendMasterInfo();
    }
}

void Server::newConnectionSlot()
{
    QTcpSocket* peer = this->nextPendingConnection();
    Player* plr{ nullptr };

    if ( peer == nullptr )
        return;

    QHostAddress peerAddr = peer->peerAddress();

    server->setUserCalls( server->getUserCalls() + 1 );
    int slot = server->getSocketSlot( peer );
    if ( slot < 0 )
        plr = server->createPlayer( server->getEmptySlot() );
    else
        plr = server->getPlayer( slot );

    QObject::connect( plr, &Player::sendRemoteAdminPwdReqSignal,
                      this, &Server::sendRemoteAdminPwdReqSlot );
    plr->setSocket( peer );

    QString ip{ peerAddr.toString() };
    plr->setPublicIP( ip );
    plr->setPublicPort( peer->peerPort() );
    ip = ip.append( ":%1" ).arg( peer->peerPort() );

    QString greeting = Helper::getMOTDMessage();
    if ( Helper::getRequirePassword() )
    {
        greeting.append( "///PASSWORD REQUIRED NOW: " );
        plr->setPwdRequested( true );
    }

    quint64 bOut{ 0 };
    if ( !greeting.isEmpty() )
    {
        bOut += server->sendMasterMessage( greeting, plr, false );
        plr->setPacketsOut( plr->getPacketsOut() + 1 );
    }

    if ( !server->getServerRules().isEmpty() )
    {
        bOut += this->sendServerRules( plr );
        plr->setPacketsOut( plr->getPacketsOut() + 1 );
    }
    plr->setBytesOut( plr->getBytesIn() + bOut );
    server->setBytesOut( server->getBytesOut() + bOut );

    //Connect the pending Connection to a Disconnected lambda.
    QObject::connect( peer, &QTcpSocket::disconnected, [=]()
    {
        QStandardItem* item = plrTableItems.take( ip );
        if ( item != nullptr )
        {
            if ( item == plr->getTableRow() )
            {
                plrViewModel->removeRow( item->row() );
                plr->setTableRow( nullptr );
            }
            else    //The item is unrelated to our User, re-insert it.
                plrTableItems.insert( ip, item );
        }

        server->setPlayerCount( server->getPlayerCount() - 1 );
        if ( server->getPlayerCount() <= 0 )
        {
            server->setPlayerCount( 0 );
            server->setGameInfo( "" );
        }

        server->deletePlayer( plr->getSlotPos() );
        server->sendMasterInfo();
    });

    //Connect the pending Connection to a ReadyRead lambda.
    QObject::connect( peer, &QTcpSocket::readyRead, [=]()
    {
        QByteArray data = plr->getOutBuff();
        quint64 bIn = data.length();

        data.append( peer->readAll() );
        server->setBytesIn( server->getBytesIn() + (data.length() - bIn) );

        if ( data.contains( "\r" )
          || data.contains( "\n" ) )
        {
            int bytes = data.indexOf( "\r\n" );
            if ( bytes <= 0 )
                bytes = data.indexOf( "\n" );
            if ( bytes <= 0 )
                bytes = data.indexOf( "\r" );

            if ( bytes > 0 )
            {
                QString packet = data.left( bytes + 1 ).trimmed();
                data = data.mid( bytes + 1 ).data();

                plr->setOutBuff( data );

                plr->setPacketsIn( plr->getPacketsIn(), 1 );
                plr->setBytesIn( plr->getBytesIn() + packet.length() );

                this->detectPacketFlood( plr );
                this->parsePacket( packet, plr );
                emit peer->readyRead();
            }
        }
    });

    //Update the User's Table row.
    QByteArray data = udpDatas.value( peerAddr );
    if ( data.isEmpty() )
    {
        udpDatas.insert( peerAddr, QByteArray( "No BIO Data Detected. Be wary of this User!" ) );
        data = udpDatas.value( peerAddr );
    }

    plr->setBioData( data );
    if ( plrTableItems.contains( ip ) )
    {
        plr->setTableRow( this->updatePlrListRow( ip, data, plr, false ) );
    }
    else
    {
        plrTableItems[ ip ] = this->updatePlrListRow( ip, data, plr, true );
        plr->setTableRow( plrTableItems.value( ip ) );

        server->setPlayerCount( server->getPlayerCount() + 1 );
        server->sendMasterInfo();
    }
    this->checkBannedInfo( plr );
}

void Server::readyReadUDPSlot()
{
    QUdpSocket* sender = static_cast<QUdpSocket*>( QObject::sender() );
    if ( sender != nullptr )
    {
        BanDialog* bandlg = admin->getBanDialog();
        QHostAddress senderAddr{};
        quint16 senderPort{ 0 };

        udpData.resize( sender->pendingDatagramSize() );
        sender->readDatagram( udpData.data(), udpData.size(), &senderAddr, &senderPort );

        //Check for a banned IP-Address. --Log the rejection to file.
        if ( bandlg->getIsIPBanned( senderAddr ) )
        {
            QString logTxt{ "Ignoring UDP from banned IP Address: [ %2:%3 ] sent command: %4" };
            QString log{ "ignored.txt" };
            logTxt = logTxt.arg( senderAddr.toString() )
                           .arg( senderPort )
                           .arg( QString( udpData ) );

            Helper::logToFile( log, logTxt, true, true );
        }
        else    //Read the incoming command.
        {
            QString data( udpData );
            if ( !data.isEmpty() )
            {
                QString sernum{ "" };
                switch ( data.at( 0 ).toLatin1() )
                {
                    case 'G':   //Set the Server's gameInfoString.
                        #if defined( DECRYPT_PACKET_PLUGIN ) && defined( USE_MULTIWORLD_FEATURE )
                            //This will not work. The incoming port from UDP isn't usable due to TCP using a random port.
                            //What we may be able to do is unconditionally allow Users with the same IP to communicate
                            //in an unbiased manner. (e.g. assume both IP addresses are on the same world.)

                            //The above is our last option available if we would like a multi-world implementation.
                            {
                            qDebug() << senderPort;
                                Player* tmpPlr = server->getPlayer( server->getIPAddrSlot( senderAddr.toString() ) );
                                if ( tmpPlr != nullptr )
                                    tmpPlr->setGameInfo( data.mid( 1 ) );
                            }
                        #else
                            server->setGameInfo( data.mid( 1 ) );
                        #endif
                    break;
                    case 'M':   //Read the response to the Master Server checkin.
                        this->parseMasterServerResponse( udpData );
                    break;
                    case 'P':   //Store the Player information into a struct.
                        sernum = Helper::getStrStr( data, "sernum", "=", "," );
                        if (( Helper::getReqSernums()
                           && Helper::serNumtoInt( sernum ) )
                          || !Helper::getReqSernums() )
                        {
                            if ( !bandlg->getIsSernumBanned( sernum ) )
                            {
                                udpDatas.insert( senderAddr, udpData.mid( 1 ) );
                                server->sendServerInfo( senderAddr, senderPort );
                            }
                        }
                        //TODO: Check for banned D and V variables. --Low priority.
                    break;
                    case 'Q':   //Send our online User information to the requestor.
                        sernum = Helper::getStrStr( data, "sernum", "=", "," );
                        if (( Helper::getReqSernums()
                           && Helper::serNumtoInt( sernum ) )
                          || !Helper::getReqSernums() )
                        {
                            if ( !bandlg->getIsSernumBanned( sernum ) )
                                server->sendUserList( senderAddr, senderPort );
                        }
                        //TODO: Check for banned D and V variables. --Low priority.
                    break;
                    case 'R':   //TODO: Command "R" with unknown use.
                    break;
                    default:    //Do nothing; Unknown command.
                    return;
                    break;
                }
            }
        }
    }
}

void Server::setupPublicServer(bool value)
{
    if ( value != server->getIsPublic() )
    {
        if ( !server->getIsPublic() )
        {
            masterCheckIn.start();
            server->sendMasterInfo();
        }
        else
        {
            masterCheckIn.stop();
            server->sendMasterInfo( true );
        }
        server->setIsPublic( value );
    }
}

quint64 Server::sendServerRules(Player* plr)
{
    if ( plr == nullptr
      || plr->getSocket() == nullptr )
    {
        return 0;
    }

    QString rules{ ":SR$%1\r\n" };
            rules = rules.arg( server->getServerRules() );

    return plr->getSocket()->write( rules.toLatin1(), rules.length() );
}

void Server::parseMasterServerResponse(QByteArray& mData)
{
    int opcode{ 0 };
    int pubIP{ 0 };
    int pubPort{ 0 };

    if ( !mData.isEmpty() )
    {
        QDataStream mDataStream( mData );
                    mDataStream >> opcode;
                    mDataStream >> pubIP;
                    mDataStream >> pubPort;

        server->setPublicIP( QHostAddress( pubIP ).toString() );
        server->setPublicPort( static_cast<quint16>( qFromBigEndian( pubPort ) ) );

        //We've obtained valid Master information.
        //Reset check-in to every 5 minutes.
        masterCheckIn.setInterval( 300000 );
    }
}

void Server::parsePacket(QString& packet, Player* plr)
{
    if ( plr == nullptr )
        return;

    if ( packet.startsWith( ":SR", Qt::CaseInsensitive ) )
    {
        //Prevent Users from Impersonating the Server Admin.
        if ( packet.startsWith( ":SR@", Qt::CaseInsensitive ) )
            return;

        //Prevent Users from changing the Server's rules. ( However temporary the effect may be. )
        if ( packet.startsWith( ":SR$", Qt::CaseInsensitive ) )
            return;

        //Prevent re-sending packets from Users with a muted Network.
        if ( !plr->getNetworkMuted() )
            this->parseSRPacket( packet, plr );
    }

    if ( packet.startsWith( ":MIX", Qt::CaseInsensitive ) )
        this->parseMIXPacket( packet, plr );
}

void Server::parseSRPacket(QString& packet, Player* plr)
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
                        switch ( plr->getTargetType() )
                        {
                            case Player::ALL:
                                {
                                    send = true;
                                }
                            break;
                            case Player::PLAYER:
                                {
                                    if ( plr->getTargetSerNum() == tmpPlr->getSernum() )
                                        send = true;
                                }
                            break;
                            case Player::SCENE:
                                {
                                    if ((( plr->getTargetScene() == tmpPlr->getSernum() )
                                      || ( plr->getTargetScene() == tmpPlr->getSceneHost() ))
                                      && ( plr != tmpPlr ) )
                                    {
                                        send = true;
                                    }
                                }
                            break;
                            default:
                                send = false;
                            break;
                        }
                    }

                    if ( send && isAuth )
                    {
                    #if defined( DECRYPT_PACKET_PLUGIN ) && defined( USE_MULTIWORLD_FEATURE )
                        qDebug() << tmpPlr << tmpPlr->getHasGameInfo() << tmpPlr->getGameInfo()
                                 << plr << plr->getHasGameInfo() << plr->getGameInfo();
                        if ( tmpPlr->getHasGameInfo()
                          && tmpPlr->getGameInfo().compare( plr->getGameInfo(), Qt::CaseInsensitive ) )
                        {
                            bOut = tmpSoc->write( packet.toLatin1(), packet.length() );
                        }
                        else if ( packetInterface != nullptr
                               && packetInterface->canSendPacket( packet, server->getGameName() ) )
                        {
                            bOut = tmpSoc->write( packet.toLatin1(), packet.length() );
                        }
                    #else
                        bOut = tmpSoc->write( packet.toLatin1(), packet.length() );
                    #endif

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

void Server::parseMIXPacket(QString& packet, Player* plr)
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
        case '7':   //Set the User's HB ID. --Unknown usage outside of disconnecting users.
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

void Server::readMIX0(QString& packet, Player* plr)
{
    QString sernum_s = packet.mid( 2 ).left( 8 );
    qint32 sernum_i = Helper::serNumtoInt( sernum_s );

    //Send the next Packet to the Scene's Host.
    plr->setTargetScene( sernum_i );
    plr->setTargetType( Player::SCENE );
}

void Server::readMIX1(QString& packet, Player* plr)
{
    QString sernum_s = packet.mid( 2 ).left( 8 );
    qint32 sernum_i = Helper::serNumtoInt( sernum_s );

    plr->setSceneHost( sernum_i );
}

void Server::readMIX2(QString&, Player* plr)
{
    plr->setSceneHost( 0 );
    plr->setTargetType( Player::ALL );
}

void Server::readMIX3(QString& packet, Player* plr)
{
    QString sernum_s = packet.mid( 2 ).left( 8 );
    qint32 sernum_i = Helper::serNumtoInt( sernum_s );

    //Check if the User is banned or requires authentication.
    this->authRemoteAdmin( plr, sernum_i );
    this->checkBannedInfo( plr );
}

void Server::readMIX4(QString& packet, Player* plr)
{
    QString sernum_s = packet.mid( 2 ).left( 8 );
    qint32 sernum_i = Helper::serNumtoInt( sernum_s );

    plr->setTargetSerNum( sernum_i );
    plr->setTargetType( Player::PLAYER );
}

void Server::readMIX5(QString& packet, Player* plr)
{
    QString sernum = plr->getSernum_s();
    QString alias = Helper::getStrStr( packet, "", "", ": " ).mid( 10 );
    QString msg = Helper::getStrStr( packet, ": ", ": ", "" );
            msg = msg.left( msg.length() - 2 );

    if ( !alias.isEmpty()
      && !msg.isEmpty() )
    {
        QString response{ "" };
        quint64 bOut{ 0 };

        bool disconnect{ false };
        if ( plr->getPwdRequested()
          && !plr->getEnteredPwd() )
        {
            QVariant pwd( msg );
            if ( Helper::cmpServerPassword( pwd ) )
            {
                response = "Correct password, welcome.";

                plr->setPwdRequested( false );
                plr->setEnteredPwd( true );
            }
            else
            {
                response = "Incorrect password, please go away.";
                disconnect = true;
            }
        }
        else if (( msg.startsWith( "/password ", Qt::CaseInsensitive )
               || msg.startsWith( "/pwd ", Qt::CaseInsensitive ) ) )
        {
            QVariant pwd = Helper::getStrStr( msg, "/password", " ", "" );
            if ( pwd.toString().isEmpty() )
                pwd = Helper::getStrStr( msg, "/pwd", " ", "" );

            //Check if the User is creating a Password.
            if ( plr->getReqNewAuthPwd()
              && !plr->getGotNewAuthPwd() )
            {
                QString pass( pwd.toString() );
                if ( admin->makeAdmin( sernum, pass ) )
                {
                    response = "You are now registered as an Admin with the Server. "
                               "You are currently Rank-0 (Game Master) with limited commands.";

                    plr->setReqNewAuthPwd( false );
                    plr->setGotNewAuthPwd( true );

                    //Prevent the server from attempting to authenticate the new Admin.
                    plr->setReqAuthPwd( false );
                    plr->setGotAuthPwd( true );
                }
                else
                {
                    response = "You are not registered as an Admin with the Server. "
                               "It seems something has gone wrong or you were already registered as an Admin.";
                    plr->setReqNewAuthPwd( false );
                    plr->setGotNewAuthPwd( false );
                }
            }   //Check if the User needs to have their password confirmed.
            else if ( !plr->getGotAuthPwd()
                   || plr->getReqAuthPwd() )
            {
                //The User is attempting to Manually Authenticate.
                if ( !pwd.toString().isEmpty()
                  && AdminHelper::cmpRemoteAdminPwd( sernum, pwd ) )
                {
                    response = "Correct password, welcome.";

                    plr->setReqAuthPwd( false );
                    plr->setGotAuthPwd( true );
                }
                else
                {
                    response = "Incorrect password, please go away.";
                    disconnect = true;
                }
            }
        }
        else
            emit newUserCommentSignal( sernum, alias, msg );

        if ( !response.isEmpty() )
        {
            bOut = server->sendMasterMessage( response, plr, false );
            if ( disconnect )
            {
                plr->setSoftDisconnect( true );
                server->setIpDc( server->getIpDc() + 1 );
            }
            else if ( bOut >= 1 )
            {
                plr->setPacketsOut( plr->getPacketsOut() + 1 );
                plr->setBytesOut( plr->getBytesOut() + bOut );
                server->setBytesOut( server->getBytesOut() + bOut );
            }
        }
    }
}

void Server::readMIX6(QString&, Player* plr)
{
    if ( plr == nullptr )
        return;

    QString unauth{ "While your SerNum is registered as a Remote Admin, you are not Authenticated and "
                    "are unable to use these commands. Please reply to this message with (/password *PASS) "
                    "and the server will authenticate you." };

    QString invalid{ "Your SerNum is not registered as a Remote Admin. Please refrain from attempting to use "
                     "Remote Admin commands as you will be banned after ( %1 ) more tries." };
    QString valid{ "It has been done. I hope you entered the right command!" };

    QString sernum = plr->getSernum_s();
    if ( AdminHelper::getIsRemoteAdmin( sernum ) )
    {
        if ( plr->getGotAuthPwd() )
        {
            //admin->parseCommand( packet );
            server->sendMasterMessage( valid, plr, false);
        }
        else
            server->sendMasterMessage( unauth, plr, false);
    }
    else
    {
        plr->setCmdAttempts( plr->getCmdAttempts() + 1 );
        invalid = invalid.arg( MAX_CMD_ATTEMPTS - plr->getCmdAttempts() );

        if ( plr->getCmdAttempts() >= MAX_CMD_ATTEMPTS )
        {
            QString reason = QString( "Auto-Banish; <Unregistered Remote Admin: ( %1 ) command attempts>" )
                                 .arg( plr->getCmdAttempts() );

            server->sendMasterMessage( reason, plr, false );

            //Append BIO data to the reason for the Ban log.
            reason.append( " [ %1:%2 ]: %3" );
            reason = reason.arg( plr->getPublicIP() )
                           .arg( plr->getPublicPort() )
                           .arg( QString( plr->getBioData() ) );
            admin->getBanDialog()->addIPBan( plr->getPublicIP(), reason );

            plr->setSoftDisconnect( true );
            server->setIpDc( server->getIpDc() + 1 );
        }
        else
            server->sendMasterMessage( invalid, plr, false );
    }

}

void Server::readMIX7(QString& packet, Player* plr)
{
    if ( plr == nullptr )
        return;

    packet = packet.mid( 2 );
    packet = packet.left( packet.length() - 2 );

    //Check if the User is banned or requires authentication.
    this->authRemoteAdmin( plr, packet.toInt( 0, 16 ) );
    this->checkBannedInfo( plr );
}

void Server::readMIX8(QString& packet, Player* plr)
{
    QDir ssvDir( "mixVariableCache" );
    if ( ssvDir.exists() )
    {
        QString sernum = packet.mid( 2 ).left( 8 );

        packet = packet.mid( 10 );
        packet = packet.left( packet.length() - 2 );

        QStringList vars = packet.split( ',' );
        QString val{ "" };

        if ( Helper::getAllowSSV()
          && !vars.contains( "Admin", Qt::CaseInsensitive ))
        {
            QSettings ssv( "mixVariableCache/" % vars.value( 0 ) % ".ini", QSettings::IniFormat );
            val = QString( ":SR@V%1%2,%3,%4,%5\r\n" )
                      .arg( sernum )
                      .arg( vars.value( 0 ) )
                      .arg( vars.value( 1 ) )
                      .arg( vars.value( 2 ) )
                      .arg( ssv.value( vars.value( 1 ) % "/" % vars.value( 2 ), "" ).toString() );
        }

        if ( !val.isEmpty() )
            plr->getSocket()->write( val.toLatin1(), val.length() );
    }
}

void Server::readMIX9(QString& packet, Player*)
{
    QDir ssvDir( "mixVariableCache" );
    if ( !ssvDir.exists() )
        ssvDir.mkpath( "." );

    packet = packet.mid( 10 );
    packet = packet.left( packet.length() - 2 );

    QStringList vars = packet.split( ',' );
    if ( Helper::getAllowSSV()
      && !vars.contains( "Admin", Qt::CaseInsensitive ))
    {
        QSettings ssv( "mixVariableCache/" % vars.value( 0 ) % ".ini", QSettings::IniFormat );
                  ssv.setValue( vars.value( 1 ) % "/" % vars.value( 2 ), vars.value( 3 ) );
    }
}

void Server::sendRemoteAdminPwdReqSlot(Player* plr, QString& serNum)
{
    if ( plr == nullptr )
        return;

    QString msg{ "The server Admin requires all Remote Administrators to authenticate themselves with their password. "
                 "Please enter your password with the command (/password *PASS) or be denied access to the server. Thank you!" };

    if ( AdminHelper::getReqAdminAuth()
      && AdminHelper::getIsRemoteAdmin( serNum ) )
    {
        plr->setReqAuthPwd( true );
        server->setBytesOut( server->getBytesOut()
                           + server->sendMasterMessage( msg, plr, false ) );
    }
}

void Server::authRemoteAdmin(Player* plr, qint32 id)
{
    if ( plr == nullptr )
        return;

    if (( plr->getSernum() != id
       && id != 0 )
      || plr->getSernum() == 0 )
    {
        QString serNum_s = Helper::serNumToIntStr( Helper::intToStr( id, 16, 8 ) );
        if ( plr->getSernum() == 0 )
        {
            if ( plr->getTableRow() != nullptr )
            {
                plrViewModel->setData( plrViewModel->index( plr->getTableRow()->row(), 1 ),
                                       serNum_s,
                                       Qt::DisplayRole );
            }

            //Disconnect the User if they have no SerNum, as we require SerNums.
            if ( Helper::getReqSernums()
              && id == 0 )
            {
                plr->setHardDisconnect( true );
                server->setIpDc( server->getIpDc() + 1 );
            }
            plr->setSernum( id );
            plr->setSernum_s( serNum_s );
        }
        else if (( id != 0 && plr->getSernum() != id )
               && plr->getSernum() != 0 )
        {
            plr->setHardDisconnect( true );   //User's sernum has somehow changed. Disconnect them. --This is a possible Ban event.
            server->setIpDc( server->getIpDc() + 1 );
        }
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
