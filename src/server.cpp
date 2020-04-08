
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

QHash<QHostAddress, QByteArray> Server::bioHash;

Server::Server(QWidget* parent, ServerInfo* svr, QStandardItemModel* plrView)
    : QTcpServer(parent)
{
    //Setup Objects.
    mother = parent;
    server = svr;
    plrViewModel = plrView;

    //Setup Objects.
    serverComments = new Comments( parent, svr );
    serverComments->setTitle( svr->getServerName() );

    chatView = new ChatView( parent, server );
    chatView->setTitle( svr->getServerName() );

    chatView->setGameID( server->getGameId() );

    pktHandle = new PacketHandler( server, chatView );
    server->setPktHandle( pktHandle );

    //Connect Objects.
    QObject::connect( pktHandle, &PacketHandler::newUserCommentSignal, serverComments, &Comments::newUserCommentSlot, Qt::QueuedConnection );

    auto* widget = dynamic_cast<ReMixWidget*>( mother );
    QObject::connect( widget, &ReMixWidget::reValidateServerIPSignal, widget,
    [=]()
    {
        if ( server->getIsSetUp() )
            server->setIsSetUp( false );

        server->setIsPublic( true );
    }, Qt::QueuedConnection );

    QObject::connect( chatView, &ChatView::sendChatSignal, chatView,
    [=](QString msg)
    {
        qint64 bOut{ 0 };
        if ( !msg.isEmpty() )
        {
            if ( !Helper::strStartsWithStr( msg, "Owner" ) )
            {
                for ( int i = 0; i < MAX_PLAYERS; ++i )
                {
                    Player* plr = server->getPlayer( i );
                    if ( plr != nullptr )
                    {
                        bOut = plr->write( msg.toLatin1(), msg.length() );
                        server->updateBytesOut( plr, bOut );
                    }
                }
            }
            else
                server->sendMasterMessage( msg, nullptr, true );
        }
    }, Qt::QueuedConnection );

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

void Server::updatePlayerTable(Player* plr, const QHostAddress& peerAddr, const quint16& port)
{
    QString ip{ peerAddr.toString() % ":%1" };
            ip = ip.arg( port );

    QByteArray data{ this->getBioHashValue( peerAddr ) };
    if ( data.isEmpty() )
    {
        this->insertBioHash( peerAddr, QByteArray( "No BIO Data Detected. Be wary of this User!" ) );
        data = this->getBioHashValue( peerAddr );
    }

    plr->setBioData( data );
    if ( plrTableItems.contains( ip ) )
    {
        plr->setTableRow( this->updatePlayerTableImpl( ip, data, plr, false ) );
    }
    else
    {
        plrTableItems[ ip ] = this->updatePlayerTableImpl( ip, data, plr, true );

        plr->setTableRow( plrTableItems.value( ip ) );
        server->sendMasterInfo();
    }
    pktHandle->checkBannedInfo( plr );
}

QStandardItem* Server::updatePlayerTableImpl(const QString& peerIP, const QByteArray& data, Player* plr, const bool& insert)
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
        QString alias = Helper::getStrStr( bio, "alias", "=", "," );
        QString age = Helper::getStrStr( bio, "HHMM", "=", "," );

        User::updateCallCount( Helper::serNumToHexStr( sernum ) );
        plr->setPlayTime( age );
        plr->setAlias( alias );

        plrViewModel->setData( plrViewModel->index( row, 1 ), sernum, Qt::DisplayRole );
        plrViewModel->setData( plrViewModel->index( row, 2 ), age, Qt::DisplayRole );
        plrViewModel->setData( plrViewModel->index( row, 3 ), alias, Qt::DisplayRole );
        plrViewModel->setData( plrViewModel->index( row, 7 ), bio, Qt::DisplayRole );
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

    this->listen( QHostAddress( server->getPrivateIP() ), server->getPrivatePort() );
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    Player* plr{ nullptr };

    server->setUserCalls( server->getUserCalls() + 1 );
    int slot = server->getSocketSlot( socketDescriptor );
    if ( slot < 0 )
        plr = server->createPlayer( server->getEmptySlot(), socketDescriptor );
    else
        plr = server->getPlayer( slot );

    //Set the Player's reference to the ServerInfo class.
    plr->setServerInfo( server );

    //Connect the pending Connection to a Disconnected lambda.
    QObject::connect( plr, &Player::disconnected, plr,
    [=]()
    {
        this->userDisconnected( plr );
    });

    //Connect the pending Connection to a ReadyRead lambda.
    QObject::connect( plr, &Player::readyRead, plr,
    [=]()
    {
        this->userReadyRead( plr );
    }, Qt::QueuedConnection );

    server->sendServerGreeting( plr );
    this->updatePlayerTable( plr, plr->peerAddress(), plr->peerPort() );
}

void Server::userReadyRead(Player* plr)
{
    if ( plr == nullptr )
        return;

    QByteArray data = plr->getOutBuff();
    qint64 bIn = data.length();

    data.append( plr->readAll() );
    server->setBytesIn( server->getBytesIn() + static_cast<quint64>(data.length() - bIn ) );

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
            plr->setBytesIn( plr->getBytesIn() + static_cast<quint64>( packet.length() ) );

            pktHandle->parsePacket( packet, plr );
            if ( plr->bytesAvailable() > 0
              || plr->getOutBuff().size() > 0 )
            {
                emit plr->readyRead();
            }
        }
    }
}

void Server::userDisconnected(Player* plr)
{
    if ( plr == nullptr )
        return;

    QString ip{ plr->peerAddress().toString() % ":%1" };
            ip = ip.arg( plr->peerPort() );

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

    server->deletePlayer( plr );
    server->sendMasterInfo();
}

QHash<QHostAddress, QByteArray> Server::getBioHash()
{
    return bioHash;
}

void Server::insertBioHash(const QHostAddress& addr, const QByteArray& value)
{
    bioHash.insert( addr, value );
}

QByteArray Server::getBioHashValue(const QHostAddress& addr)
{
    return bioHash.value( addr );
}

QHostAddress Server::getBioHashKey(const QByteArray& bio)
{
    return bioHash.key( bio );
}
