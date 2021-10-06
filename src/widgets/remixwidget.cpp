
//Class includes.
#include "remixwidget.hpp"
#include "ui_remixwidget.h"

//Required ReMix Widget includes.
#include "thread/mastermixthread.hpp"
#include "widgets/plrlistwidget.hpp"
#include "widgets/ruleswidget.hpp"
#include "widgets/motdwidget.hpp"

//ReMix includes.
#include "remixtabwidget.hpp"
#include "packethandler.hpp"
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

ReMixWidget::ReMixWidget(QWidget* parent, Server* svrInfo) :
    QWidget(parent),
    ui(new Ui::ReMixWidget)
{
    ui->setupUi( this );
    server = svrInfo;

    //Initialize the MasterMixThread Object.
    masterMixThread = new QThread();
    masterMixThread->moveToThread( MasterMixThread::getInstance() );

    //Initialize the ChatView Dialog.
    ChatView::getInstance( server )->setTitle( server->getServerName() );
    ChatView::getInstance( server )->setGameID( server->getGameId() );

    //Initialize the Comments Dialog.
    serverComments = new Comments( nullptr, server );
    serverComments->setTitle( server->getServerName() );

    //Setup Objects.
    motdWidget = MOTDWidget::getInstance( server );

    //Initialize the RulesWidget
    rules = RulesWidget::getInstance( server );
    rules->setServerName( server->getServerName() );
    server->setGameInfo( rules->getGameInfo() );

    //Initialize the PlrListWidget.
    PlrListWidget* plrList{ PlrListWidget::getInstance( this, server ) };
    QObject::connect( this, &ReMixWidget::fwdUpdatePlrViewSignal, plrList, &PlrListWidget::updatePlrViewSlot );
    QObject::connect( this, &ReMixWidget::plrViewInsertRowSignal, plrList, &PlrListWidget::plrViewInsertRowSlot );
    QObject::connect( this, &ReMixWidget::plrViewRemoveRowSignal, plrList, &PlrListWidget::plrViewRemoveRowSlot );
    QObject::connect( plrList, &PlrListWidget::insertedRowItemSignal, this, &ReMixWidget::insertedRowItemSlot );

    //Fill the ReMix UI with the PlrListWidget.
    ui->plrListFill->setLayout( plrList->layout() );
    ui->plrListFill->layout()->addWidget( plrList );
    ui->openPlayerView->setText( "Hide Player List" );

    ui->chatViewFill->setLayout( ChatView::getInstance( server )->layout() );
    ui->chatViewFill->layout()->addWidget( ChatView::getInstance( server ) );
    ui->openChatView->setText( "Hide Chat View" );

    //Connect Object Signals to Slots.
    QObject::connect( server, &Server::plrConnectedSignal, this, &ReMixWidget::plrConnectedSlot );
    QObject::connect( server->getPktHandle(), &PacketHandler::newUserCommentSignal, serverComments, &Comments::newUserCommentSlot );
    QObject::connect( serverComments, &Comments::newUserCommentSignal, this,
    [=, this](const QString& comment)
    {
        emit this->crossServerCommentSignal( server, comment );
    });

    QObject::connect( ReMixTabWidget::getTabInstance(), &ReMixTabWidget::crossServerCommentSignal, this,
    [=, this](Server* serverObj, const QString& comment)
    {
        if ( server != serverObj
          && Settings::getSetting( SKeys::Setting, SSubKeys::EchoComments ).toBool() )
        {
            QString message{ "Comment from Server [ %1 ]: %2" };
                    message = message.arg( serverObj->getServerName() )
                                     .arg( comment );
            server->sendMasterMessageToAdmins( message, nullptr );
        }
    });

    QObject::connect( server, &Server::initializeServerSignal, this, &ReMixWidget::initializeServerSlot );

    QObject::connect( this, &ReMixWidget::reValidateServerIPSignal, this,
    [=, this]()
    {
        if ( server->getIsSetUp() )
            server->setIsSetUp( false );

        server->setIsPublic( true );
    } );

    QObject::connect( ChatView::getInstance( server ), &ChatView::sendChatSignal, this,
    [=, this](QString msg)
    {
        if ( !msg.isEmpty() )
            server->sendMasterMessage( msg, nullptr, true );
    } );

    QObject::connect( rules, &RulesWidget::gameInfoChangedSignal, this,
    [=, this](const QString& gameInfo)
    {
        server->setGameInfo( gameInfo );
    } );

    //Initialize the TCP Server if we're starting as a public instance.
    if ( server->getIsPublic() )
        this->initializeServerSlot();

    ui->isPublicServer->setChecked( server->getIsPublic() );
    ui->useUPNP->setChecked( server->getUseUPNP() );

    //Create Timer Lambda to update our UI.
    this->initUIUpdate();

    //Start the MasterMix Thread.
    masterMixThread->start();
}

ReMixWidget::~ReMixWidget()
{
    server->sendMasterInfo( true );

    if ( ui->useUPNP->isChecked() )
        server->setupUPNP( false );

    //Disconnect and Delete Objects.
    serverComments->disconnect();
    serverComments->deleteLater();
    masterMixThread->exit();
    masterMixThread->disconnect();
    masterMixThread->deleteLater();

    PlrListWidget::deleteInstance( server );
    PacketHandler::deleteInstance( server );
    RulesWidget::deleteInstance( server );
    MOTDWidget::deleteInstance( server );
    ChatView::deleteInstance( server );

    delete server;
    delete ui;
}

Server* ReMixWidget::getServer() const
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

void ReMixWidget::sendServerMessage(const QString& msg) const
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
    [=, this]()
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
                    QString waiting{ "Waiting for response..." };
                    msg = "Sent UDP check-in to Master using: <a href=\"%1\"><span style=\" text-decoration: underline; color:#007af4;\">%1:%2</span></a>. %3";
                    if ( server->getGameId() == Games::W97 )
                        waiting = "This is a Warpath Server; the Master will not respond.";

                    msg = msg.arg( server->getPrivateIP() )
                             .arg( server->getPrivatePort() )
                             .arg( waiting );

                    if ( server->getMasterTimedOut() )
                        msg = "No UDP response received from master server. Perhaps we are behind a firewall?";
                }
            }

            //Validate the server's IP Address is still valid.
            //If it is now invalid, restart the network sockets.
            if ( Settings::getSetting( SKeys::WrongIP, server->getPrivateIP() ).toBool() )
                emit this->reValidateServerIPSignal();

            PlrListWidget::getInstance( this, server )->resizeColumns();
        }
        ui->networkStatus->setText( msg );
    } );
}

void ReMixWidget::on_openUserInfo_clicked()
{
    User* user{ User::getInstance() };
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
    Settings* settings{ Settings::getInstance() };
    if ( settings->isVisible() )
        settings->hide();
    else
        settings->show();
}

void ReMixWidget::on_openPlayerView_clicked()
{
    if ( ui->chatViewFill->isVisible() )
    {
        if ( !ui->plrListFill->isVisible() )
        {
            ui->openPlayerView->setText( "Hide Player List" );
            ui->openChatView->setEnabled( true );
            ui->plrListFill->show();
        }
        else
        {
            ui->openPlayerView->setText( "Show Player List" );
            ui->openChatView->setEnabled( false );
            ui->plrListFill->hide();
        }
    }
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
    if ( ui->plrListFill->isVisible() )
    {
        if ( !ui->chatViewFill->isVisible() )
        {
            ui->openChatView->setText( "Hide Chat View" );
            ui->openPlayerView->setEnabled( true );
            ui->chatViewFill->show();
        }
        else
        {
            ui->openChatView->setText( "Show Chat View" );
            ui->openPlayerView->setEnabled( false );
            ui->chatViewFill->hide();
        }
    }
}

void ReMixWidget::on_isPublicServer_toggled(bool value)
{
    //Prevent the Server class from re-initializing the Server.
    if ( value != server->getIsPublic() )
        server->setIsPublic( ui->isPublicServer->isChecked() );
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
    Logger* logUi{ Logger::getInstance() };
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
    if ( server != nullptr )
    {
        if ( server->isListening() )
            server->close();

        server->listen( QHostAddress( server->getPrivateIP() ), server->getPrivatePort() );
    }
}

void ReMixWidget::plrConnectedSlot(qintptr socketDescriptor)
{
    server->setUserCalls( server->getUserCalls() + 1 );

    Player* plr{ server->getPlayer( socketDescriptor ) };
    if ( plr == nullptr )
        plr = server->createPlayer( socketDescriptor );

    //Set the Player's reference to the Server class.
    plr->setServer( server );
    plr->setPlrConnectedTime( QDateTime::currentDateTime().toSecsSinceEpoch() );

    QObject::connect( plr, &Player::updatePlrViewSignal, this, &ReMixWidget::fwdUpdatePlrViewSlot, Qt::UniqueConnection );

    //Connect the pending Connection to a Disconnected lambda.
    //Using a lambda to safely access the Plr Object within the Slot.
    QObject::connect( plr, &Player::disconnected, plr,
    [=, this]()
    {
        this->plrDisconnectedSlot( plr, false );
    }, Qt::UniqueConnection );

    QObject::connect( plr, &Player::errorOccurred, this, [=, this](QAbstractSocket::SocketError socketError)
    {
        if ( QAbstractSocket::RemoteHostClosedError == socketError )
            this->plrDisconnectedSlot( plr, false );
        else if ( QAbstractSocket::SocketTimeoutError == socketError )
            this->plrDisconnectedSlot( plr, true );

    }, Qt::UniqueConnection );

    QObject::connect( plr, &Player::parsePacketSignal, server->getPktHandle(), &PacketHandler::parsePacketSlot, Qt::UniqueConnection );

    server->sendServerGreeting( plr );
    plr->setPlrConnectedTime( QDateTime::currentDateTime().toSecsSinceEpoch() );
    this->updatePlayerTable( plr );
}

void ReMixWidget::plrDisconnectedSlot(Player* plr, const bool& timedOut)
{
    if ( plr == nullptr )
        return;

    QStandardItem* item{ plrTableItems.take( plr->socketDescriptor() ) };
    if ( item != nullptr )
        emit this->plrViewRemoveRowSignal( item );

    plr->setDisconnected( true );   //Ensure ReMix knows that the player object is in a disconnected state.
    server->deletePlayer( plr, timedOut );
    server->sendMasterInfo();
}

void ReMixWidget::updatePlayerTable(Player* plr)
{
    if ( plr == nullptr )
        return;

    QHostAddress ipAddr{ plr->peerAddress() };

    QByteArray data{ Settings::getBioHashValue( ipAddr ) };
    if ( data.isEmpty() )
    {
        data = QByteArray( "No BIO Data Detected. Be wary of this User!" );
        Settings::insertBioHash( ipAddr, data );
    }

    plr->setBioData( data );
    qintptr peerSoc{ plr->socketDescriptor() };
    if ( !plrTableItems.contains( peerSoc ) )
        emit this->plrViewInsertRowSignal( peerSoc, data );
    else
        this->insertedRowItemSlot( plrTableItems.value( peerSoc, nullptr ), peerSoc, data );

    server->getPktHandle()->checkBannedInfo( plr );

    QString logMsg{ "Client: [ %1: ] connected with BIO [ %2 ]" };
            logMsg = logMsg.arg( plr->peerAddress().toString() )
                           .arg( plr->getBioData() );

    Logger::getInstance()->insertLog( this->getServerName(), logMsg, LogTypes::CLIENT, true, true );
}

qintptr ReMixWidget::getPeerFromQItem(QStandardItem* item) const
{
    return plrTableItems.key( item );
}

void ReMixWidget::fwdUpdatePlrViewSlot(Player* plr, const qint32& column, const QVariant& data, const qint32& role, const bool& isColor)
{
    if ( plr != nullptr )
    {
        QStandardItem* object{ plrTableItems.value( plr->socketDescriptor(), nullptr ) };
        if ( object != nullptr )
            emit this->fwdUpdatePlrViewSignal( object, column, data, role, isColor );
    }
}

void ReMixWidget::insertedRowItemSlot(QStandardItem* item, const qintptr& peer, const QByteArray& data)
{
    Player* plr{ server->getPlayer( peer ) };
    if ( item != nullptr )
    {
        if ( plrTableItems.value( plr->socketDescriptor(), nullptr ) == nullptr )
            plrTableItems[ plr->socketDescriptor() ] = item;
    }

    if ( plr != nullptr )
    {
        QString bio{ QString( data ) };
        QString ip{ plr->peerAddress().toString() % ":%1" };
                ip = ip.arg( plr->peerPort() );

        this->fwdUpdatePlrViewSlot( plr, 0, ip, Qt::DisplayRole, false );

        if ( !bio.isEmpty() )
        {
            QString sernum{ Helper::getStrStr( bio, "sernum", "=", "," ) };
            QString alias{ Helper::getStrStr( bio, "alias", "=", "," ) };
            QString age{ Helper::getStrStr( bio, "HHMM", "=", "," ) };

            User::updateCallCount( Helper::serNumToHexStr( sernum ) );

            this->fwdUpdatePlrViewSlot( plr, 1, sernum, Qt::DisplayRole, false );
            this->fwdUpdatePlrViewSlot( plr, 2, age, Qt::DisplayRole, false );
            this->fwdUpdatePlrViewSlot( plr, 3, alias, Qt::DisplayRole, false );
            this->fwdUpdatePlrViewSlot( plr, 7, bio, Qt::DisplayRole, false );
        }
    }
    server->sendMasterInfo();
}
