
//Class includes.
#include "remixwidget.hpp"
#include "ui_remixwidget.h"

//ReMix Widget includes.
#include "widgets/settingswidget.hpp"
#include "widgets/plrlistwidget.hpp"
#include "widgets/ruleswidget.hpp"
#include "widgets/motdwidget.hpp"

//ReMix includes.
#include "remixtabwidget.hpp"
#include "packethandler.hpp"
#include "cmdhandler.hpp"
#include "chatview.hpp"
#include "settings.hpp"
#include "randdev.hpp"
#include "player.hpp"
#include "server.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "user.hpp"

//Qt Includes.
#include <QStandardItemModel>
#include <QNetworkInterface>
#include <QMenu>

ReMixWidget::ReMixWidget(QSharedPointer<Server> svrInfo, QWidget* parent) :
    QWidget(parent),
    server( svrInfo ),
    ui(new Ui::ReMixWidget)
{
    ui->setupUi( this );

    censorUIIPInfo = Settings::getSetting( SKeys::Setting, SSubKeys::CensorIPInfo ).toBool();
    QObject::connect( SettingsWidget::getInstance(), &SettingsWidget::censorUIIPInfoSignal, this, &ReMixWidget::censorUIIPInfoSlot );

    //Setup Objects.
    motdWidget = MOTDWidget::getInstance( server );

    //Initialize the RulesWidget
    rules = RulesWidget::getInstance( server );
    server->setGameWorld( rules->getGameInfo() );

    //Initialize the PlrListWidget.
    PlrListWidget* plrList{ PlrListWidget::getInstance( this, server ) };
    QObject::connect( this, &ReMixWidget::plrViewInsertRowSignal, plrList, &PlrListWidget::plrViewInsertRowSlot );
    QObject::connect( this, &ReMixWidget::plrViewRemoveRowSignal, plrList, &PlrListWidget::plrViewRemoveRowSlot );

    //Fill the ReMix UI with the PlrListWidget.
    ui->plrListFill->setLayout( plrList->layout() );
    ui->plrListFill->layout()->addWidget( plrList );
    ui->openPlayerView->setText( "Hide Player List" );

    ui->chatViewFill->setLayout( ChatView::getInstance( server )->layout() );
    ui->chatViewFill->layout()->addWidget( ChatView::getInstance( server ) );
    ui->openChatView->setText( "Hide Chat View" );

    //Connect Object Signals to Slots.
    QObject::connect( server.get(), &Server::plrConnectedSignal, this, &ReMixWidget::plrConnectedSlot );
    QObject::connect( PacketHandler::getInstance( server ), &PacketHandler::newUserCommentSignal,
                      ChatView::getInstance( server ), &ChatView::newUserCommentSlot );

    QObject::connect( ChatView::getInstance( server ), &ChatView::newUserCommentSignal, this,
    [=, this](const QString& comment)
    {
        emit this->crossServerCommentSignal( server, comment );
    });

    QObject::connect( ReMixTabWidget::getInstance(), &ReMixTabWidget::crossServerCommentSignal, this,
    [=, this](QSharedPointer<Server> serverObj, const QString& comment)
    {
        if ( server != serverObj
          && Settings::getSetting( SKeys::Setting, SSubKeys::EchoComments ).toBool() )
        {
            QString message{ "Comment from Server [ %1 ]: %2" };
                    message = message.arg( serverObj->getServerName() )
                                     .arg( comment );
            server->sendMasterMessageToAdmins( message );
        }
    });

    QObject::connect( server.get(), &Server::initializeServerSignal, this, &ReMixWidget::initializeServerSlot );

    QObject::connect( this, &ReMixWidget::reValidateServerIPSignal, this,
    [=, this](const QString& interfaceIP)
    {
        if ( !Helper::cmpStrings( interfaceIP, server->getPrivateIP() ) )
        {
            if ( server->getIsSetUp() )
            {
                server->setIsSetUp( false );
                server->setupInfo( interfaceIP );
            }
            server->setIsPublic( true );
        }
    } );

    QObject::connect( ChatView::getInstance( server ), &ChatView::sendChatSignal, this,
    [=, this](const QString& msg)
    {
        if ( !msg.isEmpty() )
            server->sendMasterMessage( msg, nullptr, true );
    } );

    //Initialize the TCP Server if we're starting as a public instance.
    if ( server->getIsPublic() )
        this->initializeServerSlot();

    ui->isPublicServer->setChecked( server->getIsPublic() );
    ui->useUPNP->setChecked( server->getUseUPNP() );

    //Hide PlayerView based on User Settings.
    bool plrListHidden{ false };
    if ( Settings::getSetting( SKeys::Setting, SSubKeys::HidePlayerView, server->getServerName() ).toBool() )
    {
        ui->openPlayerView->setText( "Show Player List" );
        ui->openChatView->setDisabled( true );
        ui->plrListFill->hide();
        plrListHidden = true;
    }

    //Hide ChatView based on User Settings.
    if ( Settings::getSetting( SKeys::Setting, SSubKeys::HideChatView, server->getServerName() ).toBool()
      && !plrListHidden )
    {
        ui->openChatView->setText( "Show Chat View" );
        ui->openPlayerView->setDisabled( true );
        ui->chatViewFill->hide();
    }

    QObject::connect( Theme::getInstance(), &Theme::themeChangedSignal, this,
    [=,this]()
    {
        auto pal{ Theme::getCurrentPal() };
        ui->openPlayerView->setPalette( pal );
        ui->isPublicServer->setPalette( pal );
        ui->networkStatus->setPalette( pal );
        ui->openSettings->setPalette( pal );
        ui->openUserInfo->setPalette( pal );
        ui->openChatView->setPalette( pal );
        ui->onlineTime->setPalette( pal );
        ui->logButton->setPalette( pal );
        ui->useUPNP->setPalette( pal );
    });

    //Create Timer Lambda to update our UI.
    this->initUIUpdate();
}

ReMixWidget::~ReMixWidget()
{
    QObject::disconnect( this );
    this->disconnect( this );

    server->sendMasterInfo( true );

    if ( ui->useUPNP->isChecked() )
        server->setupUPNP( false );

    PlrListWidget::deleteInstance( server );
    PacketHandler::deleteInstance( server );
    RulesWidget::deleteInstance( server );
    CmdHandler::deleteInstance( server );
    MOTDWidget::deleteInstance( server );
    ChatView::deleteInstance( server );

    server.get()->disconnect();
//    server->deleteLater();
    server = nullptr;

    delete ui;
}

QSharedPointer<Server> ReMixWidget::getServer() const
{
    return server;
}

void ReMixWidget::renameServer(const QString& newName)
{
    if ( !newName.isEmpty() )
    {
        Settings::copyServerSettings( server, newName );
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

void ReMixWidget::initUIUpdate()
{
    //Create and Connect Lamda Objects
    QObject::connect( server.get(), &Server::connectionTimeUpdateSignal, this,
    [=, this]()
    {
        ui->onlineTime->setText( Helper::getTimeFormat( server->getUpTime() ) );
        ReMixTabWidget::setToolTipString( this );

        //Update other Info as well.
        QString msg{ "Toggle \"Public Servers\" when ready!" };
        QString listeningStr{ "Listening for incoming calls to: <a href=\"%1\"><span style=\" text-decoration: underline; color:#007af4;\">%1:%2</span></a>" };
        if ( server->getIsPublic() )
        {
            if ( this->getCensorUIIPInfo() )
            {
                msg = listeningStr.arg( "***.***.***.***" )
                                  .arg( server->getPrivatePort() );
            }
            else
            {
                msg = listeningStr.arg( server->getPrivateIP() )
                                  .arg( server->getPrivatePort() );
            }

            if ( server->getSentUDPCheckin() )
            {
                if ( server->getMasterUDPResponse() )
                {
                    QString msg2{ " ( Port forward from: %1:%2 ) ( Ping: %3 ms, Avg: %4 ms, Trend: %5 ms )" };
                    if ( this->getCensorUIIPInfo() )
                    {
                        msg2 = msg2.arg( "***.***.***.***" )
                                   .arg( server->getPublicPort() );
                    }
                    else
                    {
                        msg2 = msg2.arg( server->getPublicIP() )
                                   .arg( server->getPublicPort() );
                    }
                    msg2 = msg2.arg( server->getMasterPing() )
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

            PlrListWidget::getInstance( this, server )->resizeColumns();
        }
        ui->networkStatus->setText( msg );
    } );
}

QString ReMixWidget::getInterface(QWidget* parent)
{
    QList<QHostAddress> ipList{ QNetworkInterface::allAddresses() };
    QStringList items;

    //Default to our localhost address if nothing valid is found.
    QHostAddress ipAddress{ QHostAddress::Null };
    for ( const QHostAddress& ip : ipList )
    {
        items.append( ip.toString() );
    }

    bool ok;
    QString item{ QInputDialog::getItem( parent, "ReMix", "Select Network Interface:", items, 0, false, &ok) };
    if ( ok && !item.isEmpty() )
        return item;

    return Helper::getPrivateIP().toString();
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
    bool hide{ false };
    if ( ui->chatViewFill->isVisible() )
    {
        if ( !ui->plrListFill->isVisible() )
        {
            ui->openPlayerView->setText( "Hide Player List" );
            ui->openChatView->setEnabled( true );
            ui->plrListFill->show();
            hide = false;
        }
        else
        {
            ui->openPlayerView->setText( "Show Player List" );
            ui->openChatView->setEnabled( false );
            ui->plrListFill->hide();
            hide = true;
        }
    }

    Settings::setSetting( hide, SKeys::Setting, SSubKeys::HidePlayerView, server->getServerName() );
    if ( hide )
        Settings::setSetting( false, SKeys::Setting, SSubKeys::HideChatView, server->getServerName() );
}

void ReMixWidget::on_openChatView_clicked()
{
    bool hide{ false };
    if ( ui->plrListFill->isVisible() )
    {
        if ( !ui->chatViewFill->isVisible() )
        {
            ui->openChatView->setText( "Hide Chat View" );
            ui->openPlayerView->setEnabled( true );
            ui->chatViewFill->show();
            hide = false;
        }
        else
        {
            ui->openChatView->setText( "Show Chat View" );
            ui->openPlayerView->setEnabled( false );
            ui->chatViewFill->hide();
            hide = true;
        }
    }

    Settings::setSetting( hide, SKeys::Setting, SSubKeys::HideChatView, server->getServerName() );
    if ( hide )
        Settings::setSetting( false, SKeys::Setting, SSubKeys::HidePlayerView, server->getServerName() );
}

void ReMixWidget::on_isPublicServer_toggled(bool value)
{
    //Prevent the Server class from re-initializing the Server.
    if ( value != server->getIsPublic() )
        server->setIsPublic( ui->isPublicServer->isChecked() );
}

void ReMixWidget::on_networkStatus_linkActivated(const QString&)
{
    QString interface{ this->getInterface( this ) };
    if ( interface != server->getPrivateIP() )
        emit this->reValidateServerIPSignal( interface );
}

void ReMixWidget::on_useUPNP_toggled(bool value)
{
    if ( value != server->getUseUPNP() )
        server->setUseUPNP( ui->useUPNP->isChecked() );
}

void ReMixWidget::on_useUPNP_clicked()
{
    if ( ui->useUPNP->checkState() == Qt::Checked )
    {
        static const QString title{ "UPNP Feature:" };
        static const QString prompt{ "Enabling the UPNP Feature may result in ReMix being unable to ping the Master Mix! "
                                     "If you are unable to ping the Master Mix try disabling the UPNP Feature and manusally forward your desired ports.." };
        Helper::warningMessage( this, title, prompt );
    }
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

    QSharedPointer<Player> plr{ server->getPlayer( socketDescriptor ) };
    if ( plr == nullptr )
    {
        plr = server->createPlayer( socketDescriptor, server );
        QObject::connect( PacketHandler::getInstance( server ), &PacketHandler::sendPacketToPlayerSignal, plr.get(), &Player::sendPacketToPlayerSlot );
    }

    if ( plr == nullptr )
        return;

    plr->setPlrConnectedTime( QDateTime::currentDateTime().toSecsSinceEpoch() );

    QObject::connect( plr.get(), &Player::updatePlrViewSignal, PlrListWidget::getInstance( this, server ), &PlrListWidget::updatePlrViewSlot );

    //Connect the pending Connection to a Disconnected lambda.
    //Using a lambda to safely access the Plr Object within the Slot.
    QObject::connect( plr.get(), &Player::disconnected, plr.get(),
    [=, this]()
    {
        this->plrDisconnectedSlot( plr, false );
    }, Qt::UniqueConnection );

    QObject::connect( plr.get(), &Player::errorOccurred, this,
    [=, this](QAbstractSocket::SocketError socketError)
    {
        if ( QAbstractSocket::SocketTimeoutError == socketError )
            this->plrDisconnectedSlot( plr, true );

    }, Qt::UniqueConnection );

    QObject::connect( plr.get(), &Player::parsePacketSignal, PacketHandler::getInstance( server ),&PacketHandler::parsePacketSlot, Qt::UniqueConnection );

    server->sendServerGreeting( plr );
    plr->setPlrConnectedTime( QDateTime::currentDateTime().toSecsSinceEpoch() );
    this->updatePlayerTable( plr );
}

void ReMixWidget::plrDisconnectedSlot(QSharedPointer<Player> plr, const bool& timedOut)
{
    if ( plr == nullptr )
        return;

    emit this->plrViewRemoveRowSignal( plr );

    plr->setDisconnected( true );   //Ensure ReMix knows that the player object is in a disconnected state.
    server->deletePlayer( plr, false, timedOut );
}

void ReMixWidget::updatePlayerTable(QSharedPointer<Player> plr)
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
    emit this->plrViewInsertRowSignal( plr, plr->getIPPortAddress(), data );

    PacketHandler::getInstance( server )->checkBannedInfo( plr );

    QString logMsg{ "Client: [ %1: ] connected with BIO [ %2 ]" };
            logMsg = logMsg.arg( plr->getIPAddress() )
                           .arg( plr->getBioData() );

    Logger::getInstance()->insertLog( this->getServerName(), logMsg, LogTypes::CLIENT, true, true );
}

bool ReMixWidget::getCensorUIIPInfo() const
{
    return censorUIIPInfo;
}

void ReMixWidget::censorUIIPInfoSlot(const bool& state)
{
    censorUIIPInfo = state;
}
