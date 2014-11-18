
#include "server.hpp"

#include "helper.hpp"
#include "serverinfo.hpp"

Server::Server(ServerInfo* svr, QStandardItemModel* plrView)
{
    //Setup Objects.
    server = svr;
    plrViewModel = plrView;

    masterCheckIn.setInterval( 300000 );    //Every 5 Minutes.
    masterSocket = new QUdpSocket( this );

    //Connect Objects.
    QObject::connect( this, &QTcpServer::newConnection,
                      this, &Server::newConnectionSlot );

    QObject::connect( masterSocket, &QUdpSocket::readyRead,
                      this, &Server::readyReadUDPSlot );

    QObject::connect( &masterCheckIn, &QTimer::timeout,
                      this, &Server::masterCheckInTimeOutSlot );
}

Server::~Server()
{
    masterSocket->close();
    masterSocket->deleteLater();

    Player* plr{ nullptr };
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        plr = server->getPlayer( i );
        if ( plr != nullptr )
            delete plr;
    }
    udpDatas.clear();
}

QStandardItem* Server::updatePlrListRow(QString& peerIP, QByteArray& data, bool insert)
{
    QString bio = QString( data );
    int row = -1;

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
    if ( !data.isEmpty() )
    {
        QString sernum{ "" };
        index = bio.indexOf( "sernum=", Qt::CaseInsensitive );
        if ( index > 0 )
        {
            sernum = bio.mid( index + 7 );
            sernum = sernum.left( sernum.indexOf( ',' ) );
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
        index = bio.indexOf( "alias=", Qt::CaseInsensitive );
        if ( index > 0 )
        {
            alias = bio.mid( index + 6 );
            alias = alias.left( alias.indexOf( ',' ) );
        }
        plrViewModel->setData( plrViewModel->index( row, 3 ), alias, Qt::DisplayRole );

        plrViewModel->setData( plrViewModel->index( row, 7 ), bio.mid( 1 ), Qt::DisplayRole );
    }
    return plrViewModel->item( row, 0 );
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

        if ( server->getIsPublic() && server->getIsSetUp() && this->isListening() )
            this->masterCheckInTimeOutSlot();

        server->setIsSetUp( true );
    }
}

void Server::newConnectionSlot()
{
    QTcpSocket* peer = this->nextPendingConnection();
    Player* plr{ nullptr };
    if ( peer == nullptr )
        return;

    int slot = server->getSocketSlot( peer );
    if ( slot < 0 )
    {
        slot = server->getEmptySlot();
        server->createPlayer( slot );

        plr = server->getPlayer( slot );
    }
    else
        plr = server->getPlayer( slot );

    plr->setSocket( peer );

    QString ip = QString( "%1" ).arg( peer->peerAddress().toString() );
    plr->setPublicIP( ip );

    for ( int i = 0, count = 0; i < MAX_PLAYERS; ++i )
    {
        Player* tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr )
        {
            if ( tmpPlr->getPublicIP() == ip )
                ++count;

            //More than one User has this IP address.
            if ( count > 1 )
            {
                //Check if we're allowing users to use multiple Clients.
                if ( !Helper::getAllowDupedIP() )
                {
                    plr->getSocket()->close();  //Close the duplicated IP. They're not allowed.
                    if ( Helper::getBanDupedIP() )
                    {
                        ;   //TODO: Add code to Ban the user.
                    }
                    //TODO: Remove the Player from all maps/hashes --With exception of the UDPDatas hash.
                    return; //Exit the method. We don't listen to dupes.
                }
            }
        }
    }
    ip = ip.append( ":%1" ).arg( peer->peerPort() );

    QString greeting = QString( ":SR@M" + Helper::getMOTDMessage().toLatin1() );
    if ( Helper::getRequirePassword() )
    {
        greeting.append( "///PASSWORD REQUIRED NOW: " );
        plr->setPwdRequested( true );
    }

    if ( peer != nullptr )
        peer->write( greeting.toLatin1() +  "\r\n" );

    //Connect the pending Connection to a ReadyRead lambda.
    QObject::connect( peer, &QTcpSocket::readyRead, [peer, plr, this]()
    {
        if ( plr != nullptr )
        {
            QByteArray data = plr->getOutBuff();

            data.append( peer->readAll() );
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

                    this->parsePacket( packet, plr );
                    emit peer->readyRead();
                }
            }
        }
    });

    //Connect the pending Connection to a Disconnected lambda.
    QObject::connect( peer, &QTcpSocket::disconnected, [peer, plr, ip, this]()
    {
        if ( plr != nullptr )
        {
            QStandardItem* item = plrTableItems.take( ip );
            if ( item != nullptr )
                plrViewModel->removeRow( item->row() );

            server->setPlayerCount( server->getPlayerCount() - 1 );
            if ( server->getPlayerCount() <= 0 )
            {
                server->setPlayerCount( 0 );
                server->setGameInfo( "" );
            }

            peer->deleteLater();
            this->masterCheckInTimeOutSlot();
        }
    });

    //Delete the Player Object associated with the Peer Socket.
    QObject::connect( peer, &QTcpSocket::destroyed, [peer, plr]()
    {
//        plr->setSocket( nullptr );
//        delete plr;
    });

    //Update the User's Table row.
    QByteArray data = udpDatas.value( peer->peerAddress() );
    if ( !data.isEmpty() )
    {
        if ( plrTableItems.contains( ip ) )
            this->updatePlrListRow( ip, data, false );
        else
        {
            plrTableItems[ ip ] = this->updatePlrListRow( ip, data, true );
            server->setPlayerCount( server->getPlayerCount() + 1 );

            this->masterCheckInTimeOutSlot();
        }
    }
}

void Server::readyReadUDPSlot()
{
    QUdpSocket* sender = static_cast<QUdpSocket*>( QObject::sender() );
    if ( sender != nullptr )
    {
        QHostAddress senderAddr;
        quint16 senderPort = 0;
        udpData.resize( sender->pendingDatagramSize() );
        sender->readDatagram( udpData.data(), udpData.size(), &senderAddr, &senderPort );

        QString response;
        QString data( udpData );
        if ( !data.isEmpty() )
        {
            switch ( data.at( 0 ).toLatin1() )
            {
                case 'G':   //Set the Server's gameInfoString.
                    server->setGameInfo( data.mid( 1 ) );
                break;
                case 'M':   //Read the response to the Master Server checkin.
                    this->parseMasterServerResponse( udpData );
                break;
                case 'P':   //TODO: Store the Player information into a struct.
                    udpDatas.insert( senderAddr, udpData );

                    //TODO: Check for banned D, V, and W variables and disconnect on positive matches.
                    //      If the variables are banned, no response will be sent.
                    this->sendServerInfo( sender, senderAddr, senderPort );

                break;
                case 'Q':   //TODO: Send our online User information to the requestor.
                    this->sendUserList( sender, senderAddr, senderPort );
                    //TODO: Format a string containing the serNums of all active players.
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

void Server::sendUserList(QUdpSocket*, QHostAddress&, quint16)
{

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
    int opcode = 0;
    int pubIP = 0;
    int pubPort = 0;

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

        //Only parse packets from Users that have entered the correct password.
        if ( plr->getEnteredPwd() || !Helper::getRequirePassword() )
            this->parseSRPacket( packet, plr );
    }

    if ( packet.startsWith( ":MIX", Qt::CaseInsensitive ) )
        this->parseMIXPacket( packet, plr );
}

void Server::parseMIXPacket(QString& packet, Player* plr)
{
    if ( plr == nullptr )
        return;

    QTcpSocket* soc = plr->getSocket();
    if ( soc == nullptr )
        return;

    QString tmp = packet;

    QChar opCode = packet.at( 4 );
    switch ( opCode.toLatin1() )
    {
        case '0':   //TODO: Send Packet to Scene hosted by trgSernum.
        break;
        case '1':   //TODO: Register srcSernum as Player within trgSernum's Scene.
        break;
        case '2':   //TODO: Find usage and implement.
        break;
        case '3':   //TODO: Set a Socket's attuned sernum. --This is used by "MIX4" to send the response packet.
            //TODO: Prevent multiple sockets from using the same sernum.
        break;
        case '4':   //TODO: Send the next packet from SourceSlot to DestSlot as defined within the packet.
        break;
        case '5':   //TODO: Print remoteUser comment.

            //Check for a password response.
            if ( plr->getPwdRequested() && !plr->getEnteredPwd() )
            {
                int index = tmp.indexOf( ": " );
                if ( index > 0 )
                {
                    tmp = tmp.mid( index + 2 );
                    tmp = tmp.left( tmp.length() - 2 );

                    if ( Helper::cmpPassword( tmp ) )
                    {
                        soc->write( QByteArray( ":SR@MCorrect password, welcome.\r\n" ) );
                        plr->setEnteredPwd( true );
                        plr->setPwdRequested( false );  //No longer required. Set to false.
                    }
                    else
                    {
                        soc->write( QByteArray( ":SR@MIncorrect password, please go away.\r\n" ) );
                    }
                }
                return;
            }
        break;
        case '6':   //TODO: Respond to a remoteAdmin command.
        break;
        case '7':   //TODO: Set Slot SerNum.
        break;
        case '8':   //Set/Read SSV Variable.
        case '9':
            if ( opCode.toLatin1() == '8' )
            {
                ;   //TODO: Set the SSV.
            }
            else
            {
                ;   //TODO: Read the SSV.
            }
        break;
        default:    //Do nothing. Unknown command.
            return;
        break;
    }
}

void Server::parseSRPacket(QString& packet, Player* plr)
{
    //This code is all messed up....
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        if ( plr != nullptr
          && plr->getSocket() != nullptr )
        {
            if ( server->getPlayer( i ) != nullptr
              && server->getPlayer( i )->getSocket() != nullptr )
            {
                if ( plr->getSocket() != server->getPlayer( i )->getSocket() )
                {
                    qDebug() << server->getPlayer( i )->getSocket();
                    server->getPlayer( i )->getSocket()->write( packet.toLatin1() + "\r\n" );
                }
            }
        }
    }
}
