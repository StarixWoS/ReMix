
#include "server.hpp"

#include "helper.hpp"
#include "serverinfo.hpp"
#include "usermessage.hpp"
#include "bandialog.hpp"
#include "admin.hpp"

Server::Server(QWidget* parent, ServerInfo* svr, Admin* adminDlg, QStandardItemModel* plrView)
{
    //Setup Objects.
    server = svr;
    plrViewModel = plrView;
    admin = adminDlg;

    masterCheckIn.setInterval( 300000 );    //Every 5 Minutes.

    //Setup Objects.
    serverComments = new UserMessage( parent );
    masterSocket = new QUdpSocket( this );

    //Connect Objects.
    QObject::connect( this, &QTcpServer::newConnection,
                      this, &Server::newConnectionSlot );

    QObject::connect( this, &Server::newUserCommentSignal,
                      serverComments, &UserMessage::newUserCommentSlot );

    QObject::connect( masterSocket, &QUdpSocket::readyRead,
                      this, &Server::readyReadUDPSlot );

    QObject::connect( &masterCheckIn, &QTimer::timeout,
                      this, &Server::masterCheckInTimeOutSlot );

    QDir( "mixVariableCache" ).mkpath( "." );

    //Ensure all possible User slots are fillable.
    this->setMaxPendingConnections( MAX_PLAYERS );

//#ifdef DECRYPT_PACKET_PLUGIN
//    this->loadPlugin();
//#endif
}

Server::~Server()
{
    masterSocket->close();
    masterSocket->deleteLater();

    serverComments->close();
    serverComments->deleteLater();

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        server->deletePlayer( i );
    }
    udpDatas.clear();
}

void Server::checkBannedInfo(Player* plr)
{
    //TODO: Check for banned D and V variables. --Low Priority.
    if ( plr == nullptr
      || plr->getSocket() == nullptr )
    {
        return;
    }

    BanDialog* bandlg = admin->getBanDialog();
    Player* tmpPlr{ nullptr };

    //Prevent Banned IP's or SerNums from remaining connected.
    if ( plr != nullptr
      && plr->getSocket() != nullptr )
    {
        if ( bandlg->getIsIPBanned( plr->getPublicIP() )
          || bandlg->getIsSernumBanned( plr->getSernum_s() ) )
        {
            plr->setForcedDisconnect( true );
        }
    }

    //Disconnect and ban all duplicate IP's if required.
    if ( !Helper::getAllowDupedIP() )
    {
        QString reason{ "Auto-ban: Duplicate IP Address." };
        QString peerAddr{ plr->getPublicIP() };
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            tmpPlr = server->getPlayer( i );
            if ( tmpPlr != nullptr
              && tmpPlr != plr )
            {
                if ( tmpPlr->getPublicIP() == plr->getPublicIP() )
                {
                    if ( Helper::getBanDupedIP() )
                        bandlg->addIPBan( peerAddr, reason );

                    plr->setForcedDisconnect( true );
                    server->setIpDc( server->getIpDc() + 1 );

                    if ( tmpPlr->getSocket() != nullptr )
                    {
                        tmpPlr->setForcedDisconnect( true );
                        server->setIpDc( server->getIpDc() + 1 );
                    }
                }
            }
        }
    }

    //Disconnect new Players using the same SerNum. This is an un-optional disconnect
    //due to how Private chat is handled. --Perhaps once a better fix is found we can remove this.
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr
          && tmpPlr->getSernum() == plr->getSernum() )
        {
            if ( tmpPlr != plr )
                plr->setForcedDisconnect( true );
        }
    }
}

void Server::detectPacketFlood(Player* plr)
{
    if ( plr == nullptr
      || plr->getSocket() == nullptr )
    {
        return;
    }

    int floodCount = plr->getPacketFloodCount();
    if ( floodCount >= 1 )
    {
        quint64 time = plr->getFloodTime();
        if ( time <= PACKET_FLOOD_TIME )
        {
            if ( floodCount >= PACKET_FLOOD_LIMIT )
            {
                QDir usage{ "banLog" };
                if ( !usage.exists( "banLog" ) )
                    usage.mkpath( "." );

                QString log{ QDate::currentDate().toString( "banLog/yyyy-MM-dd.txt" ) };
                QString logMsg = QString( "*** This hoser [ %1:%2 ] sent %3 packets in %4 MS, he is disconnected!" )
                                     .arg( plr->getPublicIP() )
                                     .arg( plr->getPublicPort() )
                                     .arg( floodCount )
                                     .arg( time );

                Helper::logToFile( log, logMsg, true, true );
                if ( Helper::getBanHackers() )
                {
                    BanDialog* banDlg = admin->getBanDialog();
                    if ( banDlg != nullptr )
                    {
                        logMsg = QString( "Auto-Banish; suspicious data from: [ %1:%2 ]: %3" )
                                     .arg( plr->getPublicIP() )
                                     .arg( plr->getPublicPort() )
                                     .arg( QString( plr->getBioData() ) );

                        QString ip{ plr->getPublicIP() };
                        banDlg->addIPBan( ip, logMsg );
                    }
                }
                plr->setForcedDisconnect( true );
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

    int index = 0;
    if ( !bio.isEmpty() )
    {
        QString sernum{ "" };
        index = bio.indexOf( "sernum=", Qt::CaseInsensitive );
        if ( index > 0 )
        {
            sernum = bio.mid( index + 7 );
            sernum = sernum.left( sernum.indexOf( ',' ) );

            plr->setSernum( Helper::serNumToHexStr( sernum ).toUInt( 0, 16 ) );
            plr->setSernum_s( sernum );

            this->sendRemoteAdminPwdReq( plr, sernum );
        }
        plrViewModel->setData( plrViewModel->index( row, 1 ), sernum, Qt::DisplayRole );

        QString playTime{ "" };
        index = bio.indexOf( "HHMM=", Qt::CaseInsensitive );
        if ( index > 0 )
        {
            playTime = bio.mid( index + 5 );
            playTime = playTime.left( playTime.indexOf( ',' ) );
        }
        plrViewModel->setData( plrViewModel->index( row, 2 ), playTime, Qt::DisplayRole );

        QString alias{ "" };
        index = bio.indexOf( "lias=", Qt::CaseInsensitive );
        if ( index > 0 )
        {
            alias = bio.mid( index + 5 );
            alias = alias.left( alias.indexOf( ',' ) );
        }
        plrViewModel->setData( plrViewModel->index( row, 3 ), alias, Qt::DisplayRole );

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
        masterSocket->bind( QHostAddress( server->getPrivateIP() ), server->getPrivatePort() );
        this->listen( QHostAddress( server->getPrivateIP() ), server->getPrivatePort() );

        server->setIsSetUp( true );
        if ( server->getIsPublic() && this->isListening() )
            this->masterCheckInTimeOutSlot();
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
    greeting.append( "\r\n" );

    quint64 bOut{ 0 };
    if ( peer != nullptr )
    {
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
    }

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
        }

        server->setPlayerCount( server->getPlayerCount() - 1 );
        if ( server->getPlayerCount() <= 0 )
        {
            server->setPlayerCount( 0 );
            server->setGameInfo( "" );
        }
        this->masterCheckInTimeOutSlot();

        server->deletePlayer( plr->getSlotPos() );
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

                this->parsePacket( packet, plr );
                this->detectPacketFlood( plr );
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
        this->masterCheckInTimeOutSlot();
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
            QString logTxt{ "Ignoring UDP from banned %1 %2:%3 sent command: %4" };
            QString log{ "ignored.txt" };
            logTxt = logTxt.arg( "IP Address" )
                           .arg( senderAddr.toString() )
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
                if ( data.at( 0 ) == 'P'
                  || data.at( 0 ) == 'Q' )
                {
                    int index = udpData.indexOf( "sernum=", Qt::CaseInsensitive );
                    if ( index > 0 )
                    {
                        sernum = udpData.mid( index + 7 );
                        sernum = sernum.left( sernum.indexOf( ',' ) );
                    }
                }

                switch ( data.at( 0 ).toLatin1() )
                {
                    case 'G':   //Set the Server's gameInfoString.
//                        {
//                        #if defined( DECRYPT_PACKET_PLUGIN ) && defined( USE_MULTIWORLD_FEATURE )
//                            //Allow Multi-World Servers.
//                            Player* tmpPlr{ nullptr };
//                                    tmpPlr = server->getPlayer( server->getIPAddrSlot( senderAddr.toString() ) );

//                            if ( tmpPlr != nullptr )
//                                tmpPlr->setGameInfo( data.mid( 1 ) );
//                        #else
                            server->setGameInfo( data.mid( 1 ) );
//                        #endif
//                        }
                    break;
                    case 'M':   //Read the response to the Master Server checkin.
                        this->parseMasterServerResponse( udpData );
                    break;
                    case 'P':   //Store the Player information into a struct.
                        //Check if the sernum is banned.
                        if ( !bandlg->getIsSernumBanned( sernum ) )
                        {
                            udpDatas.insert( senderAddr, udpData.mid( 1 ) );
                            this->sendServerInfo( sender, senderAddr, senderPort );
                        }
                        //TODO: Check for banned D and V variables. --Low priority.
                    break;
                    case 'Q':   //Send our online User information to the requestor.
                        //Check if the sernum is banned.
                        if ( !bandlg->getIsSernumBanned( sernum ) )
                        {
                            this->sendUserList( sender, senderAddr, senderPort );
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
            this->masterCheckInTimeOutSlot();
        }
        else
        {
            masterCheckIn.stop();
            this->disconnectFromMaster();
        }
        server->setIsPublic( value );
    }
}

void Server::disconnectFromMaster()
{
    if ( server->getIsSetUp() )
    {
        char ex = 'X';
        masterSocket->writeDatagram( &ex, 2,
                                     QHostAddress( server->getMasterIP() ), server->getMasterPort() );
    }
}

void Server::sendServerInfo(QUdpSocket* socket, QHostAddress& socAddr, quint16 socPort)
{
    //TODO: Format Server Usage variable.

    QString response = QString( "#name=%1 [%2] //Rules: %3 //ID:%4 //TM:%5 //US:%6" );
    if ( !server->getGameInfo().isEmpty() )
    {
        response = response.arg( server->getName() )
                           .arg( server->getGameInfo() )
                           .arg( server->getServerRules() )
                           .arg( QString::number( server->getServerID(), 16 ).toUpper(), 8, QChar( '0' ) )
                           .arg( QString::number( QDateTime::currentDateTime().toTime_t(), 16 ).toUpper(), 8, QChar( '0' ) )
                           .arg( "999.999.999" );
    }
    else
    {
        response = QString( "#name=%1 //Rules: %2 //ID:%3 //TM:%4 //US:%5" );
        response = response.arg( server->getName() )
                           .arg( server->getServerRules() )
                           .arg( QString::number( server->getServerID(), 16 ).toUpper(), 8, QChar( '0' ) )
                           .arg( QString::number( QDateTime::currentDateTime().toTime_t(), 16 ).toUpper(), 8, QChar( '0' ) )
                           .arg( "999.999.999" );
    }

    if ( socket != nullptr )
        socket->writeDatagram( response.toLatin1(), response.size() + 1, socAddr, socPort );
}

void Server::sendUserList(QUdpSocket* soc, QHostAddress& addr, quint16 port)
{
    Player* plr{ nullptr };
    QString response{ "Q" };
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        plr = server->getPlayer( i );
        if ( plr != nullptr && plr->getSernum() > 0 )
            response += Helper::intToStr( plr->getSernum(), 16 ) + ",";
    }

    if ( !response.isEmpty() && soc != nullptr )
        soc->writeDatagram( response.toLatin1(), response.size() + 1, addr, port );
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

void Server::masterCheckInTimeOutSlot()
{
    QString response = QString( "!version=%1,nump=%2,gameid=%3,game=%4,host=%5,id=%6,port=%7,info=%8,name=%9" );
    if ( server->getIsSetUp() )
    {
        response = response.arg( server->getVersionID_i() )
                           .arg( server->getPlayerCount() )
                           .arg( server->getGameId() )
                           .arg( server->getGameName() )
                           .arg( server->getHostInfo().localHostName() )
                           .arg( server->getServerID() )
                           .arg( server->getPrivatePort() )
                           .arg( server->getGameInfo() )
                           .arg( server->getName() );

        masterSocket->writeDatagram( response.toLatin1(), response.length() + 1,
                                     QHostAddress( server->getMasterIP() ), server->getMasterPort() );
    }
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
        server->setPublicPort( qFromBigEndian( pubPort ) );
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

    bool isAuth = false;
    bool send = false;

    quint64 bOut{ 0 };

    //Only parse packets from Users that have entered the correct password.
    if (( plr->getEnteredPwd() || !plr->getPwdRequested() )
      && ( plr->getAdminPwdEntered() || !plr->getAdminPwdRequested() ))
    {
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            isAuth = false;
            send = false;

            tmpPlr = server->getPlayer( i );
            if ( tmpPlr != nullptr
              && tmpPlr->getSocket() != nullptr )
            {
                tmpSoc = tmpPlr->getSocket();
                if ( plr->getSocket() != tmpSoc )
                {
                    tmpSoc = tmpPlr->getSocket();
                    if (( tmpPlr->getEnteredPwd() || !tmpPlr->getPwdRequested() )
                      && ( tmpPlr->getAdminPwdEntered() || !tmpPlr->getAdminPwdRequested() ))
                    {
                        isAuth = true;
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

                                    plr->setTargetType( Player::ALL );
                                    plr->setTargetSerNum( 0 );
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
                                    plr->setTargetType( Player::ALL );
                                    plr->setTargetScene( 0 );
                                }
                            break;
                            default:
                                send = false;
                            break;
                        }
                    }

                    if ( send && isAuth )
                    {
//                    #if defined( DECRYPT_PACKET_PLUGIN ) && defined( USE_MULTIWORLD_FEATURE )
//                        if ( packetInterface != nullptr )
//                        {
//                            if ( plr->getGameInfo() == tmpPlr->getGameInfo() )
//                            {
//                                bOut = tmpSoc->write( packet.toLatin1(), packet.length() );
//                            }
//                            else if ( packetInterface->canSendPacket( packet ) )
//                            {
//                                bOut = tmpSoc->write( packet.toLatin1(), packet.length() );
//                            }
//                        }
//                        else
//                            bOut = tmpSoc->write( packet.toLatin1(), packet.length() );
//                    #else
                        bOut = tmpSoc->write( packet.toLatin1(), packet.length() );
//                    #endif

                        tmpPlr->setPacketsOut( tmpPlr->getPacketsOut() + 1 );
                        tmpPlr->setBytesOut( tmpPlr->getBytesOut() + bOut );
                        server->setBytesOut( server->getBytesOut() + bOut );

                    }
                    else
                        qDebug() << QString( "Skipping packet to unauthenticated User (%1)." ).arg( tmpPlr->getPublicIP() );
                }
            }
        }
    }
    else
        qDebug() << QString( "Ignoring packet from unauthenticated User (%1)." ).arg( plr->getPublicIP() );
}

void Server::parseMIXPacket(QString& packet, Player* plr)
{
    if ( plr == nullptr )
        return;

    QChar opCode = packet.at( 4 );
    QString tmp = packet.mid( packet.indexOf( ":MIX", Qt::CaseInsensitive ) + 5 );

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
    quint32 sernum_a = Helper::strToInt( sernum_s, 16 );

    //Send the next Packet to the Scene's Host.
    plr->setTargetScene( sernum_a );
    plr->setTargetType( Player::SCENE );
}

void Server::readMIX1(QString& packet, Player* plr)
{
    QString sernum_s = packet.mid( 2 ).left( 8 );
    quint32 sernum_a = Helper::strToInt( sernum_s, 16 );

    plr->setSceneHost( sernum_a );
}

void Server::readMIX2(QString&, Player* plr)
{
    plr->setSceneHost( 0 );
    plr->setTargetType( Player::ALL );
}

void Server::readMIX3(QString& packet, Player* plr)
{
    QString sernum_s = packet.mid( 2 ).left( 8 );
    quint32 sernum_i = Helper::strToInt( sernum_s, 16 );

    //Check if the User is banned or requires authentication.
    this->authRemoteAdmin( plr, sernum_i );
    this->checkBannedInfo( plr );
}

void Server::readMIX4(QString& packet, Player* plr)
{
    QString sernum_s = packet.mid( 2 ).left( 8 );
    quint32 sernum_a = Helper::strToInt( sernum_s, 16 );

    plr->setTargetSerNum( sernum_a );
    plr->setTargetType( Player::PLAYER );
}

void Server::readMIX5(QString& packet, Player* plr)
{
    QString sernum = plr->getSernum_s();
    QString alias{ "" };
    QString msg{ "" };

    int index = packet.indexOf( ": " );
    if ( index > 0 )
    {
        alias = packet.left( index );
        alias = alias.mid( 10 );

        msg = packet.mid( index + 2 );
        msg = msg.left( msg.length() - 2 );
    }

    if ( !alias.isEmpty()
      && !msg.isEmpty() )
    {
        QString response{ "" };
        quint64 bOut{ 0 };

        QString invalid{ "Correct password, welcome." };
        QString valid{ "Incorrect password, please go away." };

        bool disconnect{ false };
        if ( plr->getPwdRequested()
          && !plr->getEnteredPwd() )
        {
            QVariant pwd( msg );
            if ( Helper::cmpServerPassword( pwd ) )
            {
                response = valid;

                plr->setPwdRequested( false );
                plr->setEnteredPwd( true );
            }
            else
            {
                response = invalid;
                disconnect = true;
            }
        }
        else if ( plr->getAdminPwdRequested()
               && !plr->getAdminPwdEntered() )
        {
            QVariant pwd( msg );
            QString sernum = plr->getSernum_s();
            if ( AdminHelper::cmpRemoteAdminPwd( sernum, pwd ) )
            {
                response = valid;

                plr->setAdminPwdRequested( false );
                plr->setAdminPwdEntered( true );
            }
            else
            {
                response = invalid;
                disconnect = true;
            }
        }
        else
            emit newUserCommentSignal( sernum, alias, msg );

        if ( !response.isEmpty() )
        {
            bOut = server->sendMasterMessage( response, plr, false );
            if ( disconnect )
            {
                plr->setForcedDisconnect( true );
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

void Server::readMIX6(QString&, Player*)
{
    //TODO: Handle incoming Admin commands.
}

void Server::readMIX7(QString& packet, Player* plr)
{
    if ( plr == nullptr )
        return;

    packet = packet.mid( 2 );
    packet = packet.left( packet.length() - 2 );

    //Check if the User is banned or requires authentication.
    this->authRemoteAdmin( plr, packet.toUInt( 0, 16 ) );
    this->checkBannedInfo( plr );
}

void Server::readMIX8(QString& packet, Player* plr)
{
    QString sernum = packet.mid( 2 ).left( 8 );

    packet = packet.mid( 10 );
    packet = packet.left( packet.length() - 2 );

    QStringList vars = packet.split( ',' );
    QString val{ "" };

    if ( Helper::getAllowSSV()
      && !vars.contains( "Admin", Qt::CaseInsensitive ))
    {
        QSettings ssv( "mixVariableCache/" + vars.value( 0 ) + ".ini", QSettings::IniFormat );
        val = QString( ":SR@V%1%2,%3,%4,%5\r\n" )
                  .arg( sernum )
                  .arg( vars.value( 0 ) )
                  .arg( vars.value( 1 ) )
                  .arg( vars.value( 2 ) )
                  .arg( ssv.value( vars.value( 1 ) + "/" + vars.value( 2 ), "" ).toString() );
    }

    if ( !val.isEmpty() )
        plr->getSocket()->write( val.toLatin1(), val.length() );
}

void Server::readMIX9(QString& packet, Player*)
{
    packet = packet.mid( 10 );
    packet = packet.left( packet.length() - 2 );

    QStringList vars = packet.split( ',' );
    if ( Helper::getAllowSSV()
      && !vars.contains( "Admin", Qt::CaseInsensitive ))
    {
        QSettings ssv( "mixVariableCache/" + vars.value( 0 ) + ".ini", QSettings::IniFormat );
                  ssv.setValue( vars.value( 1 ) + "/" + vars.value( 2 ), vars.value( 3 ) );
    }
}

void Server::sendRemoteAdminPwdReq(Player* plr, QString& serNum)
{
    if ( plr == nullptr )
        return;

    QString msg{ "The server Admin requires all Remote Administrators to authenticate themselves with their password. "
                 "Please enter your password or be denied access to the server. Thank you!"
                 "///PASSWORD REQUIRED NOW:" };
    if ( AdminHelper::getReqAdminAuth()
      && AdminHelper::getIsRemoteAdmin( serNum ) )
    {
        plr->setAdminPwdRequested( true );
        if ( plr->getSocket() != nullptr )
        {
            quint64 bOut = server->sendMasterMessage( msg, plr, false );
            server->setBytesOut( server->getBytesOut() + bOut );
        }
    }
}

void Server::authRemoteAdmin(Player* plr, quint32 id)
{
    if ( plr == nullptr )
        return;

    if (( plr->getSernum() != id
       && id != 0 )
      || plr->getSernum() == 0 )
    {
        QString serNum_s = Helper::serNumToIntStr( Helper::intToStr( id, 16, 8 ) );
        if ( plr->getSernum() <= 0 )
        {
            if ( plr->getTableRow() != nullptr )
            {
                plrViewModel->setData( plrViewModel->index( plr->getTableRow()->row(), 1 ),
                                       serNum_s,
                                       Qt::DisplayRole );
            }

            plr->setSernum( id );
            plr->setSernum_s( serNum_s );

            this->sendRemoteAdminPwdReq( plr, serNum_s );
        }
        else if ( id > 0 && plr->getSernum() != id )
            plr->setForcedDisconnect( true );   //User's sernum has somehow changed. Disconnect them. --This is a possible Ban event.
    }
}

//#ifdef DECRYPT_PACKET_PLUGIN
//bool Server::loadPlugin()
//{
//    QDir pluginsDir( qApp->applicationDirPath() );

//#if defined(Q_OS_WIN)
//    if ( pluginsDir.dirName().toLower() == "debug"
//      || pluginsDir.dirName().toLower() == "release")
//    {
//        pluginsDir.cdUp();
//    }

//#elif defined(Q_OS_MAC)
//    if (pluginsDir.dirName() == "MacOS") {
//        pluginsDir.cdUp();
//        pluginsDir.cdUp();
//        pluginsDir.cdUp();
//    }
//#endif

//    pluginsDir.cd( "plugins" );
//    foreach ( QString fileName, pluginsDir.entryList( QDir::Files ))
//    {
//        QPluginLoader pluginLoader( pluginsDir.absoluteFilePath( fileName ));
//        QObject *plugin = pluginLoader.instance();
//        if ( plugin )
//        {
//            packetInterface = qobject_cast<PacketDecryptInterface*>( plugin );
//            if ( packetInterface )
//                return true;
//        }
//    }
//    return false;
//}
//#endif
