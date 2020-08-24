
//Class includes.
#include "remixwidget.hpp"
#include "ui_remixwidget.h"

//Required ReMix Widget includes.
#include "widgets/plrlistwidget.hpp"
#include "widgets/ruleswidget.hpp"
#include "widgets/motdwidget.hpp"

//ReMix includes.
#include "remixtabwidget.hpp"
#include "packethandler.hpp"
#include "serverinfo.hpp"
#include "comments.hpp"
#include "chatview.hpp"
#include "comments.hpp"
#include "settings.hpp"
#include "randdev.hpp"
#include "player.hpp"
#include "server.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "user.hpp"

//Qt Includes.
#include <QStandardItemModel>
#include <QMenu>

ReMixWidget::ReMixWidget(QWidget* parent, ServerInfo* svrInfo) :
    QWidget(parent),
    ui(new Ui::ReMixWidget)
{
    ui->setupUi( this );
    server = svrInfo;

    //Initialize the ChatView Dialog.
    chatView = new ChatView( parent, server );
    chatView->setTitle( server->getServerName() );
    chatView->setGameID( server->getGameId() );

    //Initialize the PacketHandler Object..
    pktHandle = new PacketHandler( server, chatView );
    server->setPktHandle( pktHandle );

    //Initialize the Comments Dialog.
    serverComments = new Comments( parent, server );
    serverComments->setTitle( server->getServerName() );

    //Setup Objects.
    motdWidget = MOTDWidget::getWidget( server );

    //Initialize the RulesWidget
    rules = RulesWidget::getWidget( server );
    rules->setServerName( server->getServerName() );
    server->setGameInfo( rules->getGameInfo() );

    //Initialize the PlrListWidget.
    plrWidget = new PlrListWidget( this, server );
    plrViewModel = plrWidget->getPlrModel();

    //Fill the ReMix UI with the PlrListWidget.
    ui->tmpWidget->setLayout( plrWidget->layout() );
    ui->tmpWidget->layout()->addWidget( plrWidget );

    //Setup Networking Objects.
    tcpServer = new Server( this );
    server->setTcpServer( tcpServer );

    //Connect Object Signals to Slots.
    QObject::connect( tcpServer, &Server::plrConnectedSignal, this, &ReMixWidget::plrConnectedSlot, Qt::QueuedConnection );
    QObject::connect( pktHandle, &PacketHandler::newUserCommentSignal, serverComments, &Comments::newUserCommentSlot, Qt::QueuedConnection );
    QObject::connect( server, &ServerInfo::initializeServerSignal, this, &ReMixWidget::initializeServerSlot, Qt::QueuedConnection );

    QObject::connect( this, &ReMixWidget::reValidateServerIPSignal, this,
    [=]()
    {
        if ( server->getIsSetUp() )
            server->setIsSetUp( false );

        server->setIsPublic( true );
    }, Qt::QueuedConnection );

    QObject::connect( chatView, &ChatView::sendChatSignal, chatView,
    [=](QString msg)
    {
        if ( !msg.isEmpty() )
            server->sendMasterMessage( msg, nullptr, true );
    }, Qt::QueuedConnection );

    QObject::connect( rules, &RulesWidget::gameInfoChangedSignal, this,
    [=](const QString& gameInfo)
    {
        server->setGameInfo( gameInfo );
    }, Qt::QueuedConnection );

    //Initialize the TCP Server if we're starting as a public instance.
    if ( server->getIsPublic() )
        this->initializeServerSlot();

    ui->isPublicServer->setChecked( server->getIsPublic() );
    ui->useUPNP->setChecked( server->getUseUPNP() );

    //Create Timer Lambda to update our UI.
    this->initUIUpdate();
}

ReMixWidget::~ReMixWidget()
{
    server->sendMasterInfo( true );

    if ( ui->useUPNP->isChecked() )
        server->setupUPNP( false );

    //Disconnect and Delete Objects.
    serverComments->disconnect();
    serverComments->deleteLater();
    pktHandle->disconnect();
    pktHandle->deleteLater();
    tcpServer->disconnect();
    tcpServer->deleteLater();
    plrWidget->disconnect();
    plrWidget->deleteLater();
    pktHandle->disconnect();
    pktHandle->deleteLater();
    chatView->disconnect();
    chatView->deleteLater();

    RulesWidget::deleteWidget( server );
    MOTDWidget::deleteWidget( server );

    delete server;
    delete ui;
}

ServerInfo* ReMixWidget::getServerInfo() const
{
    return server;
}

void ReMixWidget::renameServer(const QString& newName)
{
    if ( !newName.isEmpty() )
    {
        Settings::copyServerSettings( server, newName );
        motdWidget->setServerName( newName );
        rules->setServerName( newName );

        server->setServerName( newName );
    }
}

void ReMixWidget::sendServerMessage(const QString& msg)
{
    if ( server != nullptr )
        server->sendMasterMessage( msg, nullptr, true );
}

quint32 ReMixWidget::getPlayerCount() const
{
    if ( server != nullptr )
        return server->getPlayerCount();

    return 0;
}

QString ReMixWidget::getServerName() const
{
    return server->getServerName();
}

Server* ReMixWidget::getTcpServer() const
{
    return tcpServer;
}

quint16 ReMixWidget::getPrivatePort() const
{
    if ( server == nullptr )
        return 0;

    return server->getPrivatePort();
}

void ReMixWidget::initUIUpdate()
{
    //Create and Connect Lamda Objects
    QObject::connect( server->getUpTimer(), &QTimer::timeout, server->getUpTimer(),
    [=]()
    {
        ui->onlineTime->setText( Helper::getTimeFormat( server->getUpTime() ) );
        ReMixTabWidget::setToolTipString( this );

        //Update other Info as well.
        QString msg{ "Toggle \"Public Servers\" when ready!" };
        if ( server->getIsPublic() )
        {
            msg = QString( "Listening for incoming calls to: <a href=\"%1\"><span style=\" text-decoration: underline; color:#007af4;\">%1:%2</span></a>" )
                      .arg( server->getPrivateIP() )
                      .arg( server->getPrivatePort() );

            if ( server->getSentUDPCheckin() )
            {
                if ( server->getMasterUDPResponse() )
                {
                    QString msg2{ " ( Port forward from: %1:%2 ) ( Ping: %3 ms, Avg: %4 ms, Trend: %5 ms )" };
                            msg2 = msg2.arg( server->getPublicIP() )
                                       .arg( server->getPublicPort() )
                                       .arg( server->getMasterPing() )
                                       .arg( server->getMasterPingAvg() )
                                       .arg( server->getMasterPingTrend() );
                    msg.append( msg2 );

                    qint32 fails{ server->getMasterPingFailCount() };
                    if ( fails >= 1 )
                    {
                        QString msg3{ " ( Dropped: %1 ) " };
                                msg3 = msg3.arg( fails );

                        msg.append( msg3 );
                    }
                }
                else
                {
                    msg = "Sent UDP check-in to Master using: <a href=\"%1\"><span style=\" text-decoration: underline; color:#007af4;\">%1:%2</span></a>. "
                          "Waiting for response...";
                    msg = msg.arg( server->getPrivateIP() )
                             .arg( server->getPrivatePort() );

                    if ( server->getMasterTimedOut() )
                        msg = "No UDP response received from master server. Perhaps we are behind a firewall?";
                }
            }

            //Validate the server's IP Address is still valid.
            //If it is now invalid, restart the network sockets.
            if ( Settings::getSetting( SKeys::WrongIP, server->getPrivateIP() ).toBool() )
                emit this->reValidateServerIPSignal();

            plrWidget->resizeColumns();
        }
        ui->networkStatus->setText( msg );
    }, Qt::QueuedConnection );
}

void ReMixWidget::on_openUserInfo_clicked()
{
    User* user = User::getInstance();
    if ( user != nullptr )
    {
        if ( user->isVisible() )
            user->hide();
        else
            user->show();
    }
}

void ReMixWidget::on_openSettings_clicked()
{
    Settings* settings = Settings::getInstance();
    if ( settings->isVisible() )
        settings->hide();
    else
        settings->show();
}

void ReMixWidget::on_openUserComments_clicked()
{
    if ( serverComments != nullptr )
    {
        if ( serverComments->isVisible() )
            serverComments->hide();
        else
            serverComments->show();
    }
    else
    {
        QString title{ "Error:" };
        QString message{ "Unable to fetch the Server's Comment dialog!" };
        Helper::warningMessage( this, title, message );
    }
}

void ReMixWidget::on_openChatView_clicked()
{
    if ( chatView != nullptr )
    {
        if ( chatView->isVisible() )
            chatView->hide();
        else
            chatView->show();
    }
    else
    {
        QString title{ "Error:" };
        QString message{ "Unable to fetch the Server's Chat View dialog!" };
        Helper::warningMessage( this, title, message );
    }
}

void ReMixWidget::on_isPublicServer_toggled(bool value)
{
    //Prevent the Server class from re-initializing the ServerInfo.
    if ( value != server->getIsPublic() )
    {
        server->setIsPublic( ui->isPublicServer->isChecked() );
    }
}

void ReMixWidget::on_networkStatus_linkActivated(const QString& link)
{
    QString title{ "Invalid IP:" };
    QString prompt{ "Do you wish to mark the IP Address [ %1 ] as invalid and refresh the network interface?" };

    prompt = prompt.arg( link );
    if ( Helper::confirmAction( this, title, prompt ) )
    {
        Settings::setSetting( true, SKeys::WrongIP, link );

        emit this->reValidateServerIPSignal();

        title = "Note:";
        prompt = "Please refresh your server list in-game!";
        Helper::confirmAction( this, title, prompt );
    }
}

void ReMixWidget::on_useUPNP_toggled(bool value)
{
    if ( value != server->getUseUPNP() )
        server->setUseUPNP( ui->useUPNP->isChecked() );
}

void ReMixWidget::on_networkStatus_customContextMenuRequested(const QPoint&)
{
    if ( contextMenu == nullptr )
        contextMenu = new QMenu( this );

    if ( contextMenu != nullptr )
    {
        //Populate ContextMenu with IP Addresses which were manually
        //blacklisted and removed from selection for use as the Private IP
        //For a more user-friendly method of removing them from the preferences.
    }
    //contextMenu->popup( this->mapToGlobal( pos ) );
}

void ReMixWidget::on_logButton_clicked()
{
    //Show the Logger Dialog.
    Logger* logUi = Logger::getInstance();
    if (logUi != nullptr )
    {
        if ( logUi->isVisible() )
            logUi->hide();
        else
            logUi->show();
    }
}

void ReMixWidget::initializeServerSlot()
{
    if ( tcpServer != nullptr )
    {
        if ( tcpServer->isListening() )
            tcpServer->close();

        tcpServer->listen( QHostAddress( server->getPrivateIP() ), server->getPrivatePort() );
    }
}

void ReMixWidget::plrConnectedSlot(qintptr socketDescriptor)
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
    plr->setPlrConnectedTime( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    //Connect the pending Connection to a Disconnected lambda.
    //Using a lambda to safely access the Plr Object within the Slot.
    QObject::connect( plr, &Player::disconnected, plr,
    [=]()
    {
        this->plrDisconnectedSlot( plr );
    });

    QObject::connect( plr, &Player::parsePacketSignal, pktHandle, &PacketHandler::parsePacketSlot, Qt::QueuedConnection );

    server->sendServerGreeting( plr );
    plr->setPlrConnectedTime( QDateTime::currentDateTime().toMSecsSinceEpoch() );
    this->updatePlayerTable( plr );
}

void ReMixWidget::plrDisconnectedSlot(Player* plr)
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

    plr->setDisconnected( true );   //Ensure ReMix knows that the player object is in a disconnected state.
    server->deletePlayer( plr );
    server->sendMasterInfo();
}

void ReMixWidget::updatePlayerTable(Player* plr)
{
    if ( plr == nullptr )
        return;

    QHostAddress ipAddr{ plr->peerAddress() };
    QString ip{ ipAddr.toString() % ":%1" };
            ip = ip.arg( plr->peerPort() );

    QByteArray data{ Settings::getBioHashValue( ipAddr ) };
    if ( data.isEmpty() )
    {
        data = QByteArray( "No BIO Data Detected. Be wary of this User!" );
        Settings::insertBioHash( ipAddr, data );
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

QStandardItem* ReMixWidget::updatePlayerTableImpl(const QString& peerIP, const QByteArray& data, Player* plr, const bool& insert)
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
