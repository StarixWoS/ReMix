
#include "includes.hpp"
#include "server.hpp"

Server::Server(QWidget* parent, ServerInfo* svr, User* usr,
               QStandardItemModel* plrView, QString svrID)
    : QTcpServer(parent)
{
    serverID = svrID;

    //Setup Objects.
    mother = parent;
    server = svr;
    plrViewModel = plrView;
    user = usr;

    //Setup Objects.
    serverComments = new Comments( parent );
    pktHandle = new PacketHandler( user, server, svrID );

    //Connect Objects.
    QObject::connect( pktHandle, &PacketHandler::newUserCommentSignal,
                      serverComments, &Comments::newUserCommentSlot );

    QObject::connect( this, &QTcpServer::newConnection,
                      this, &Server::newConnectionSlot );

    QObject::connect( server->getMasterSocket(), &QUdpSocket::readyRead,
                      this, &Server::readyReadUDPSlot );

    QObject::connect( dynamic_cast<ReMixWidget*>( mother ), &ReMixWidget::reValidateServerIP, [=]()
    {
        server->setIsSetUp( false );
        this->setupServerInfo();
    });

    //Ensure all possible User slots are fillable.
    this->setMaxPendingConnections( MAX_PLAYERS );
}

Server::~Server()
{
    pktHandle->disconnect();
    pktHandle->deleteLater();

    serverComments->close();
    serverComments->deleteLater();

    bioHash.clear();
}

void Server::updatePlayerTable(Player* plr, QHostAddress peerAddr, quint16 port)
{
    QString ip{ peerAddr.toString() };

    plr->setPublicIP( ip );
    plr->setPublicPort( port );

    ip = ip.append( ":%1" ).arg( port );

    QByteArray data = bioHash.value( peerAddr );
    if ( data.isEmpty() )
    {
        bioHash.insert( peerAddr, QByteArray( "No BIO Data Detected. "
                                              "Be wary of this User!" ) );
        data = bioHash.value( peerAddr );
    }

    plr->setBioData( data );
    if ( plrTableItems.contains( ip ) )
    {
        plr->setTableRow( this->updatePlayerTableImpl( ip, data,
                                                       plr, false ) );
    }
    else
    {
        plrTableItems[ ip ] = this->updatePlayerTableImpl( ip, data,
                                                           plr, true );

        plr->setTableRow( plrTableItems.value( ip ) );
        server->sendMasterInfo();
    }
    pktHandle->checkBannedInfo( plr );
}

QStandardItem* Server::updatePlayerTableImpl(QString& peerIP, QByteArray& data,
                                             Player* plr, bool insert)
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

    plrViewModel->setData( plrViewModel->index( row, 0 ),
                           peerIP,
                           Qt::DisplayRole );
    if ( !bio.isEmpty() )
    {
        QString sernum = Helper::getStrStr( bio, "sernum", "=", "," );
        plr->setSernum_i( Helper::serNumToHexStr( sernum )
                                     .toUInt( 0, 16 ) );

        qint32 index{ bio.indexOf( "d=", Qt::CaseInsensitive ) };
        QString dVar{ "" };
        QString wVar{ "" };
        if ( index >= 0 )
        {
            dVar = bio.mid( index + 2 ).left( 8 );
            plr->setDVar( dVar );
        }

        index = bio.indexOf( "w=", Qt::CaseInsensitive );
        if ( index >= 0 )
        {
            wVar = bio.mid( index + 2 ).left( 8 );
            plr->setWVar( wVar );
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

        plrViewModel->setData( plrViewModel->index( row, 7 ),
                               bio,
                               Qt::DisplayRole );
    }
    return plrViewModel->item( row, 0 );
}

Comments* Server::getServerComments() const
{
    return serverComments;
}

void Server::setupServerInfo()
{
    if ( !server->getIsSetUp() )
    {
        server->setPrivateIP( QHostAddress( QHostAddress::LocalHost )
                                   .toString() );

        QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
        for ( int i = 0; i < ipList.size(); ++i )
        {
            QString tmp = ipList.at( i ).toString();
            if ( ipList.at( i ) != QHostAddress::LocalHost
              && ipList.at( i ).toIPv4Address()
              && !Settings::getIsInvalidIPAddress( tmp )
              //Remove Windows generated APIPA addresses.
              && !tmp.startsWith( "169", Qt::CaseInsensitive ) )
            {
                //Use first non-local IP address.
                server->setPrivateIP( ipList.at(i).toString() );
                upnp = UPNP::getUpnp( QHostAddress( server->getPrivateIP() ) );
                break;
            }
        }

        QHostAddress addr{ server->getPrivateIP() };
        server->initMasterSocket( addr, server->getPrivatePort() );

        if ( this->isListening() )
            this->close();

        this->listen( addr, server->getPrivatePort() );

        server->setIsSetUp( true );
        if ( server->getIsPublic() && this->isListening() )
            server->sendMasterInfo();
    }
}

void Server::setupUPNPForward()
{
    if ( upnp == nullptr )
    {
        upnp = UPNP::getUpnp( QHostAddress( server->getPrivateIP() ) );
    }

    bool tunneled = UPNP::getTunneled();
    if ( tunneled != true )
    {
        QObject::connect( upnp, &UPNP::success, [=]()
        {
            upnp->checkPortForward( "TCP", server->getPrivatePort() );
            upnp->checkPortForward( "UDP", server->getPrivatePort() );
            upnp->disconnect();
        });
        upnp->makeTunnel( server->getPrivatePort(), server->getPublicPort() );
    }
    else
    {
        upnp->checkPortForward( "TCP", server->getPrivatePort() );
        upnp->checkPortForward( "UDP", server->getPrivatePort() );
    }
}

void Server::removeUPNPForward()
{
    if ( upnp != nullptr )
    {
        //Add a delay of one second after each removal command.
        //This is to ensure the command is sent.
        upnp->removePortForward( "TCP", server->getPrivatePort() );
        Helper::delay( 1 );

        upnp->removePortForward( "UDP", server->getPrivatePort() );
        Helper::delay( 1 );
    }
}

void Server::newConnectionSlot()
{
    QTcpSocket* peer = this->nextPendingConnection();
    Player* plr{ nullptr };

    if ( peer == nullptr )
        return;

    server->setUserCalls( server->getUserCalls() + 1 );
    int slot = server->getSocketSlot( peer );
    if ( slot < 0 )
        plr = server->createPlayer( server->getEmptySlot() );
    else
        plr = server->getPlayer( slot );

    //We've created a new Player, assign it's Socket.
    plr->setSocket( peer );

    //Connect to our Password Request Signal. This signal is
    //turned on or off by enabling or disabling Admin Auth requirements.
    QObject::connect( plr, &Player::sendRemoteAdminPwdReqSignal,
                      this, &Server::sendRemoteAdminPwdReqSlot );
    QObject::connect( plr, &Player::sendRemoteAdminRegisterSignal,
                      this, &Server::sendRemoteAdminRegisterSlot );

    //Connect the pending Connection to a Disconnected lambda.
    QObject::connect( peer, &QTcpSocket::disconnected,
                      this, &Server::userDisconnectedSlot );

    //Connect the pending Connection to a ReadyRead lambda.
    QObject::connect( peer, &QTcpSocket::readyRead,
                      this, &Server::userReadyReadSlot );

    server->sendServerGreeting( plr );
    this->updatePlayerTable( plr, peer->peerAddress(), peer->peerPort() );
}

void Server::userReadyReadSlot()
{
    QTcpSocket* socket{ dynamic_cast<QTcpSocket*>( QObject::sender() ) };
    if ( socket == nullptr )
        return;

    qint32 slot{ server->getSocketSlot( socket ) };
    Player* plr{ server->getPlayer( slot ) };

    if ( plr == nullptr )
        return;

    QByteArray data = plr->getOutBuff();
    quint64 bIn = data.length();

    data.append( socket->readAll() );
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

            pktHandle->parsePacket( packet, plr );
            if ( socket->bytesAvailable() > 0
              || plr->getOutBuff().size() > 0 )
            {
                emit socket->readyRead();
            }
        }
    }
}

void Server::userDisconnectedSlot()
{
    QTcpSocket* socket{ dynamic_cast<QTcpSocket*>( QObject::sender() ) };
    if ( socket == nullptr )
        return;

    qint32 slot{ server->getSocketSlot( socket ) };
    Player* plr{ server->getPlayer( slot ) };

    if ( plr == nullptr )
        return;

    QString ip{ socket->peerAddress().toString() % ":%1" };
            ip = ip.arg( socket->peerPort() );

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

    server->deletePlayer( plr->getSlotPos() );
    server->sendMasterInfo();
}

void Server::readyReadUDPSlot()
{
    QUdpSocket* socket{ server->getMasterSocket() };
    if ( socket == nullptr )
        return;

    QByteArray data;
    if ( socket != nullptr )
    {
        QHostAddress senderAddr{};
        quint16 senderPort{ 0 };

        data.resize( socket->pendingDatagramSize() );
        socket->readDatagram( data.data(), data.size(),
                              &senderAddr, &senderPort );

        pktHandle->parseUDPPacket( data, senderAddr, senderPort, &bioHash );
        if ( socket->hasPendingDatagrams() )
            emit socket->readyRead();
    }
}

void Server::setupPublicServer(bool value)
{
    if ( value != server->getIsPublic() )
    {
        if ( !server->getIsPublic() )
        {
            server->startMasterCheckIn();
            server->sendMasterInfo( false );
            server->setMasterUDPResponse( false );
        }
        else
        {
            server->stopMasterCheckIn();
            server->sendMasterInfo( true );
        }
        server->setIsPublic( value );
    }
}

void Server::sendRemoteAdminPwdReqSlot(Player* plr)
{
    if ( plr == nullptr )
        return;

    QString msg{ "The server Admin requires all Remote Administrators to "
                 "authenticate themselves with their password. "
                 "Please enter your password with the command (/login *PASS) "
                 "or be denied access to the server. Thank you!" };

    if ( Settings::getReqAdminAuth()
      && plr->getIsAdmin() )
    {
        plr->setReqAuthPwd( true );
        server->sendMasterMessage( msg, plr, false );
    }
}

void Server::sendRemoteAdminRegisterSlot(Player* plr)
{
    if ( plr == nullptr )
        return;

    QString msg{ "The Server Host is attempting to register you as an "
                 "Admin with the server. Please reply to this message with "
                 "(/register *YOURPASS). Note: The server Host and other "
                 "Admins will not have access to this information." };

    if ( plr->getIsAdmin() )
        server->sendMasterMessage( msg, plr, false );
}
