
//Class includes.
#include "server.hpp"

//ReMix Widget includes.
#include "widgets/remixwidget.hpp"

//ReMix includes.
#include "packethandler.hpp"
#include "packetforge.hpp"
#include "serverinfo.hpp"
#include "settings.hpp"
#include "comments.hpp"
#include "chatview.hpp"
#include "helper.hpp"
#include "player.hpp"
#include "user.hpp"

//Qt Includes.
#include <QStandardItem>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QtCore>

Server::Server(QWidget* parent, ServerInfo* svr,
               QStandardItemModel* plrView)
    : QTcpServer(parent)
{
    //Setup Objects.
    mother = parent;
    server = svr;
    plrViewModel = plrView;

    //Setup Objects.
    serverComments = new Comments( parent, svr );
    serverComments->setTitle( svr->getName() );

    chatView = new ChatView( parent, server );
    chatView->setTitle( svr->getName() );

    chatView->setGameID( server->getGameId() );

    pktHandle = new PacketHandler( server, chatView );

    //Connect Objects.
    QObject::connect( pktHandle, &PacketHandler::newUserCommentSignal,
                      serverComments, &Comments::newUserCommentSlot );

    QObject::connect( this, &QTcpServer::newConnection,
                      this, &Server::newConnectionSlot );

    QObject::connect( server->getMasterSocket(), &QUdpSocket::readyRead,
                      this, &Server::readyReadUDPSlot );

    ReMixWidget* widget = dynamic_cast<ReMixWidget*>( mother );
    QObject::connect( widget, &ReMixWidget::reValidateServerIP, widget,
    [=]()
    {
        if ( server->getIsSetUp() )
            server->setIsSetUp( false );

        server->setIsPublic( true );
    });

    QObject::connect( chatView, &ChatView::sendChat, chatView,
    [=](QString msg)
    {
        if ( !msg.isEmpty() )
        {
            if ( !Helper::strStartsWithStr( msg, "Owner" ) )
            {
                for ( int i = 0; i < MAX_PLAYERS; ++i )
                {
                    Player* plr = server->getPlayer( i );
                    if ( plr != nullptr )
                    {
                        QTcpSocket* tmpSoc{ plr->getSocket() };
                        if ( tmpSoc != nullptr  )
                        {
                            qint64 bOut = tmpSoc->write( msg.toLatin1(),
                                                         msg.length() );

                            plr->setBytesOut( plr->getBytesOut()
                                            + static_cast<quint64>( bOut ) );
                            server->setBytesOut( server->getBytesOut()
                                               + static_cast<quint64>( bOut ) );
                        }
                    }
                }
            }
            else
                server->sendMasterMessage( msg, nullptr, true );
        }
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

    chatView->close();
    chatView->deleteLater();

    bioHash.clear();
}

void Server::updatePlayerTable(Player* plr, const QHostAddress& peerAddr,
                               const quint16& port)
{
    QString ip{ peerAddr.toString() % ":%1" };
            ip = ip.arg( port );

    plr->setPublicIP( peerAddr.toString() );
    plr->setPublicPort( port );

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

QStandardItem* Server::updatePlayerTableImpl(const QString& peerIP,
                                             const QByteArray& data,
                                             Player* plr, const bool& insert)
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
        plr->validateSerNum( plr->getServerInfo(),
                             Helper::serNumToHexStr( sernum )
                                        .toUInt( nullptr, 16 ) );

        User::updateCallCount( Helper::serNumToHexStr( sernum ) );

        QString alias = Helper::getStrStr( bio, "alias", "=", "," );
        plr->setAlias( alias );

        QString age = Helper::getStrStr( bio, "HHMM", "=", "," );
        plr->setPlayTime( age );

        qint32 index{ Helper::getStrIndex( bio, "d=" ) };
        QString dVar{ "" };
        QString wVar{ "" };
        if ( index >= 0 )
        {
            dVar = bio.mid( index + 2 ).left( 8 );
            plr->setDVar( dVar );
        }

        index = Helper::getStrIndex( bio, "w=" );
        if ( index >= 0 )
        {
            wVar = bio.mid( index + 2 ).left( 8 );
            plr->setWVar( wVar );
        }

        plrViewModel->setData( plrViewModel->index( row, 1 ),
                               sernum,
                               Qt::DisplayRole );

        plrViewModel->setData( plrViewModel->index( row, 2 ),
                               age,
                               Qt::DisplayRole );

        plrViewModel->setData( plrViewModel->index( row, 3 ),
                               alias,
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

ChatView* Server::getChatView() const
{
    return chatView;
}

void Server::setupServerInfo()
{
    if ( this->isListening() )
        this->close();

    this->listen( QHostAddress( server->getPrivateIP() ),
                                server->getPrivatePort() );
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

    //Set the Player's reference to the ServerInfo class.
    plr->setServerInfo( server );

    //Connect the pending Connection to a Disconnected lambda.
    QObject::connect( peer, &QTcpSocket::disconnected, peer,
    [=]()
    {
        this->userDisconnected( peer );
    });

    //Connect the pending Connection to a ReadyRead lambda.
    QObject::connect( peer, &QTcpSocket::readyRead, peer,
    [=]()
    {
        this->userReadyRead( peer );
    });

    server->sendServerGreeting( plr );
    this->updatePlayerTable( plr, peer->peerAddress(), peer->peerPort() );
}

void Server::userReadyRead(QTcpSocket* socket)
{
    if ( socket == nullptr )
        return;

    qint32 slot{ server->getSocketSlot( socket ) };
    Player* plr{ server->getPlayer( slot ) };

    if ( plr == nullptr )
        return;

    QByteArray data = plr->getOutBuff();
    qint64 bIn = data.length();

    data.append( socket->readAll() );
    server->setBytesIn( server->getBytesIn()
                      + static_cast<quint64>(data.length() - bIn) );

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
            QByteArray packet = data.left( bytes + 1 );
                       packet = packet.left( packet.length() - 1 );

            data = data.mid( bytes + 1 ).data();

            plr->setOutBuff( data );

            plr->setPacketsIn( plr->getPacketsIn(), 1 );
            plr->setBytesIn( plr->getBytesIn()
                           + static_cast<quint64>( packet.length() ) );

            pktHandle->parsePacket( packet, plr );
            if ( socket->bytesAvailable() > 0
              || plr->getOutBuff().size() > 0 )
            {
                emit socket->readyRead();
            }
        }
    }
}

void Server::userDisconnected(QTcpSocket* socket)
{
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

        data.resize( static_cast<int>( socket->pendingDatagramSize() ) );
        socket->readDatagram( data.data(), data.size(),
                              &senderAddr, &senderPort );

        pktHandle->parseUDPPacket( data, senderAddr, senderPort, &bioHash );
        if ( socket->hasPendingDatagrams() )
            emit socket->readyRead();
    }
}
