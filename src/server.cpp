
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

    QTcpSocket* socket;

    QHash<QString, QTcpSocket*>::iterator iter = tcpSockets.begin();
    while ( iter != tcpSockets.end() )
    {
        socket = tcpSockets.take( iter.key() );
        tcpDatas.remove( socket );  //Remove all pending packets.

        socket->deleteLater();

        ++iter;
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
    if ( !server->isSetUp )
    {
        server->privateIP = QHostAddress( QHostAddress::LocalHost ).toString();

        QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
        for ( int i = 0; i < ipList.size(); ++i )
        {
            QString tmp = ipList.at( i ).toString();
            if ( ipList.at( i ) != QHostAddress::LocalHost
                 && ipList.at( i ).toIPv4Address()
                 && !Helper::isInvalidIPAddress( tmp ) )
            {
                server->privateIP = ipList.at(i).toString();   //Use first non-local IP address.
                break;
            }
        }
        masterSocket->bind( QHostAddress( server->privateIP ), server->privatePort );
        this->listen( QHostAddress( server->privateIP ), server->privatePort );

        if ( server->isPublic && server->isSetUp && this->isListening() )
            this->masterCheckInTimeOutSlot();

        server->isSetUp = true;
    }
}

void Server::newConnectionSlot()
{
    QTcpSocket* peer = this->nextPendingConnection();
    if ( peer == nullptr )
        return;

    QString ip = QString( "%1:%2" )
                     .arg( peer->peerAddress().toString() )
                     .arg( peer->peerPort() );
    if ( tcpSockets.contains( ip ) )
    {
        //Check if we're allowing users to use multiple Clients.
        if ( !Helper::getAllowDupedIP() )
        {
            peer->close();  //Close the duplicated IP. They're not allowed.
            if ( Helper::getBanDupedIP() )
            {
                ;   //TODO: Add code to Ban the user.
            }
            //TODO: Remove the Player from all maps/hashes --With exception of the UDPDatas hash.
            return; //Exit the method. We don't listen to dupes.
        }
    }
    else
        tcpSockets.insert( ip, peer );

    peer->write( QByteArray( ":SR@M" + Helper::getMOTDMessage().toLatin1() + "\r\n" ) );

    //Connect the pending Connection to a ReadyRead lambda.
    QObject::connect( peer, &QTcpSocket::readyRead, [peer, this]()
    {
        QByteArray data = tcpDatas.value( peer );
        if ( peer != nullptr )
        {
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
                    tcpDatas.insert( peer, data );

                    this->parsePacket( packet, peer );
                    emit peer->readyRead();
                }
            }
        }
    });

    //Connect the pending Connection to a Disconnected lambda.
    QObject::connect( peer, &QTcpSocket::disconnected, [peer, ip, this]()
    {
        if ( peer != nullptr )
        {
            QStandardItem* item = plrTableItems.take( ip );
            if ( item != nullptr )
                plrViewModel->removeRow( item->row() );

            tcpSockets.remove( ip );
            tcpDatas.remove( peer );
            peer->deleteLater();
        }
    });

    //Update the User's Table row.
    QByteArray data = udpDatas.value( peer->peerAddress() );
    if ( !data.isEmpty() )
    {
        if ( plrTableItems.contains( ip ) )
            this->updatePlrListRow( ip, data, false );
        else
            plrTableItems[ ip ] = this->updatePlrListRow( ip, data, true );
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
                    server->gameInfo = data.mid( 1 );
                break;
                case 'M':   //Read the response to the Master Server checkin.
                    this->parseMasterServerResponse( udpData );
                break;
                case 'P':   //TODO: Store the Player information into a struct.
                    udpDatas.insert( senderAddr, udpData );

                    //TODO: Check for banned D, V, and W variables and disconnect on positive matches.
                    //      If the variables are banned, no response will be sent.

                    //TODO: Format Server Usage variable.
                    if ( !server->gameInfo.isEmpty() )
                    {
                        response = QString( "#name=%1 [%2] //Rules: %3 //ID:%4 //TM:%5 //US:%6" )
                                       .arg( server->name )
                                       .arg( server->gameInfo )
                                       .arg( server->serverRules )
                                       .arg( QString::number( server->serverID, 16 ).toUpper(), 8, QChar( '0' ) )
                                       .arg( QString::number( QDateTime::currentDateTime().toTime_t(), 16 ).toUpper(), 8, QChar( '0' ) )
                                       .arg( "999.999.999" );
                    }
                    else
                    {
                        response = QString( "#name=%1 //Rules: %2 //ID:%3 //TM:%4 //US:%5" )
                                        .arg( server->name )
                                        .arg( server->serverRules )
                                        .arg( QString::number( server->serverID, 16 ).toUpper(), 8, QChar( '0' ) )
                                        .arg( QString::number( QDateTime::currentDateTime().toTime_t(), 16 ).toUpper(), 8, QChar( '0' ) )
                                        .arg( "999.999.999" );
                    }
                    sender->writeDatagram( response.toLatin1(), response.size() + 1, senderAddr, senderPort );
                break;
                case 'Q':   //TODO: Send our online User information to the requestor.
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
    if ( value != server->isPublic )
    {
        if ( !server->isPublic )
        {
            masterCheckIn.start();
            this->masterCheckInTimeOutSlot();
        }
        else
        {
            masterCheckIn.stop();
            this->disconnectFromMaster();
        }
        server->isPublic = value;
    }
}

void Server::disconnectFromMaster()
{
    if ( server->isSetUp )
    {
        char ex = 'X';
        masterSocket->writeDatagram( &ex, 2,
                                     QHostAddress( server->masterIP ), server->masterPort );
    }
}

void Server::masterCheckInTimeOutSlot()
{
    if ( server->isSetUp )
    {
        QString response = QString( "!version=%1,nump=%2,gameid=%3,game=%4,host=%5,id=%6,port=%7,info=%8,name=%9" )
                               .arg( server->versionID_i )
                               .arg( server->playerCount )
                               .arg( server->gameId )
                               .arg( server->gameName )
                               .arg( server->hostInfo.localHostName() )
                               .arg( server->serverID )
                               .arg( server->privatePort )
                               .arg( server->gameInfo )
                               .arg( server->name );

        masterSocket->writeDatagram( response.toLatin1(), response.length() + 1,
                                     QHostAddress( server->masterIP ), server->masterPort );
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

        server->publicIP = QHostAddress( pubIP ).toString();
        server->publicPort = qFromBigEndian( pubPort );
    }
}

void Server::parsePacket(QString& packet, QTcpSocket* socket)
{
    if ( packet.startsWith( ":SR", Qt::CaseInsensitive ) )
    {
        //Prevent Users from Impersonating the Server Admin.
        if ( packet.startsWith( ":SR@", Qt::CaseInsensitive ) )
            return;

        //Prevent Users from changing the Server's rules. ( However temporary the effect may be. )
        if ( packet.startsWith( ":SR$", Qt::CaseInsensitive ) )
            return;

        this->parseSRPacket( packet, socket );
    }

    if ( packet.startsWith( ":MIX", Qt::CaseInsensitive ) )
        this->parseMIXPacket( packet );
}

void Server::parseMIXPacket(QString& packet)
{
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

void Server::parseSRPacket(QString& packet, QTcpSocket* socket)
{
    QHash<QString, QTcpSocket*>::iterator iter = tcpSockets.begin();
    while ( iter != tcpSockets.end() )
    {
        QTcpSocket* soc = iter.value();
        if ( socket != soc )
            soc->write( packet.toLatin1() + "\r\n" );

        ++iter;
    }
}
