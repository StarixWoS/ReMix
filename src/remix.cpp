
#include "remix.hpp"
#include "ui_remix.h"

#include "helper.hpp"
#include "usermessage.hpp"
#include "messages.hpp"
#include "server.hpp"
#include "settings.hpp"
#include "admin.hpp"
#include "bandialog.hpp"

//Initialize our accepted Commandline Argument List.
const QStringList ReMix::cmdlArgs =
{
    QStringList() << "game" << "master" << "public" << "listen" << "name" << "fudge"
};

ReMix::ReMix(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReMix)
{
    ui->setupUi(this);

    //Setup our Random Device
    randDev = new RandDev();

    //Create our Context Menus
    contextMenu = new QMenu( this );

    //Initialize our Tray Icon if available.
    this->initTrayIcon();

    //Initialize our Context Menu Items.
    this->initContextMenu();

    //Setup the PlayerInfo TableView.
    plrModel = new QStandardItemModel( 0, 8, 0 );
    plrModel->setHeaderData( 0, Qt::Horizontal, "Player IP:Port" );
    plrModel->setHeaderData( 1, Qt::Horizontal, "SerNum" );
    plrModel->setHeaderData( 2, Qt::Horizontal, "Age" );
    plrModel->setHeaderData( 3, Qt::Horizontal, "Alias" );
    plrModel->setHeaderData( 4, Qt::Horizontal, "Time" );
    plrModel->setHeaderData( 5, Qt::Horizontal, "IN" );
    plrModel->setHeaderData( 6, Qt::Horizontal, "OUT" );
    plrModel->setHeaderData( 7, Qt::Horizontal, "BIO" );

    //Proxy model to support sorting without actually altering the underlying model
    plrProxy = new QSortFilterProxyModel();
    plrProxy->setDynamicSortFilter( true );
    plrProxy->setSourceModel( plrModel );
    plrProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->playerView->setModel( plrProxy );

    //Setup Dialog Objects.
    sysMessages = new Messages( this );
    settings = new Settings( this );

    //Setup Server/Player Info objects.
    server = new ServerInfo();

    //Setup Other Objects.
    admin = new Admin( this, server );

    server->setServerID( Helper::getServerID() );
    if ( server->getServerID() <= 0 )
    {
        QVariant value = this->genServerID();
        server->setServerID( value.toInt() );

        Helper::setServerID( value );
    }
    server->setHostInfo( QHostInfo() );
    server->setServerRules( Helper::getServerRules() );

    this->parseCMDLArgs();
    if ( server->getMasterIP().isEmpty() )
        this->getSynRealData();

    //Create and Connect Lamda Objects
    this->initUIUpdateLambda();

    //Setup Networking Objects.
    if ( tcpServer == nullptr )
        tcpServer = new Server( this, server, admin, plrModel );
}

ReMix::~ReMix()
{
    contextMenu->deleteLater();

    if ( trayIcon != nullptr )
        trayIcon->deleteLater();

    if ( trayMenu != nullptr )
        trayMenu->deleteLater();

    plrModel->deleteLater();
    plrProxy->deleteLater();

    sysMessages->close();
    sysMessages->deleteLater();

    admin->close();
    admin->deleteLater();

    settings->close();
    settings->deleteLater();

    tcpServer->disconnectFromMaster();
    tcpServer->close();
    tcpServer->deleteLater();

    delete randDev;
    delete server;
    delete ui;
}

int ReMix::genServerID()
{
    //Server ID may be between 0 and 0x7FFFFFFF - 0x1
    return randDev->genRandNum( 0, 0x7FFFFFFE );
}

void ReMix::initTrayIcon()
{
    //While possible to create a system tray icon, some versions of linux
    //disallow applications to create their own.. ( I'm looking at you, Ubuntu >.> )
    //Also disable the feature on OSX. --Unable to test.
    #if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
        if ( QSystemTrayIcon::isSystemTrayAvailable() )
        {
            trayIcon = new QSystemTrayIcon( QIcon( ":/icon/ReMix.ico" ), this );
            trayIcon->show();

            QAction* showAction = new QAction( "Show", this );
            QObject::connect( showAction, &QAction::triggered,
                              this, &QMainWindow::show );

            QAction* hideAction = new QAction( "Hide", this );
            QObject::connect( hideAction, &QAction::triggered,
                              this, &QMainWindow::hide );

            QAction* minimizeAction = new QAction( "Minimize", this );
            QObject::connect( minimizeAction, &QAction::triggered,
                              this, &QMainWindow::hide );

            QAction* maximizeAction = new QAction( "Maximize", this );
            QObject::connect( maximizeAction, &QAction::triggered,
                              this, &QMainWindow::showMaximized );

            QAction* restoreAction = new QAction( "Restore", this);
            QObject::connect( restoreAction, &QAction::triggered,
                              this, &QMainWindow::showNormal );

            QAction* quitAction = new QAction( "Quit", this);
            QObject::connect( quitAction, &QAction::triggered, [=]()
            {
                //Allow Rejection of a Global CloseEvent.
                if ( !this->rejectCloseEvent() )
                    qApp->quit();
            });

            trayMenu = new QMenu( this );
            trayMenu->addAction( showAction );
            trayMenu->addAction( hideAction );
            trayMenu->addSeparator();
            trayMenu->addAction( minimizeAction );
            trayMenu->addAction( maximizeAction );
            trayMenu->addAction( restoreAction );
            trayMenu->addAction( quitAction );

            QObject::connect( trayIcon, &QSystemTrayIcon::activated,
                              [=]( QSystemTrayIcon::ActivationReason reason )
            {
                if ( reason == QSystemTrayIcon::Trigger )
                {
                    if ( this->isHidden() )
                    {
                        this->show();
                        this->setWindowState( this->windowState() & ~Qt::WindowMinimized );
                        this->activateWindow();
                    }
                    else
                    {
                        this->hide();
                        this->setWindowState( Qt::WindowMinimized );
                    }
                }
                else if ( reason == QSystemTrayIcon::Context )
                {
                    if ( trayMenu != nullptr )
                        trayMenu->popup( QCursor::pos() );
                }
            });
        }
#endif
}

void ReMix::initUIUpdateLambda()
{
    //Create and Connect Lamda Objects
    QObject::connect( server->getUpTimer(), &QTimer::timeout, [=]()
    {
        quint64 time = server->getUpTime();

        QString time_s = QString( "%1:%2:%3" )
                             .arg( time / 3600, 2, 10, QChar( '0' ) )
                             .arg(( time / 60 ) % 60, 2, 10, QChar( '0' ) )
                             .arg( time % 60, 2, 10, QChar( '0' ) );

        ui->onlineTime->setText( time_s );
        ui->callCount->setText( QString( "#Calls: %1" ).arg( server->getUserCalls() ) );
        ui->packetINBD->setText( QString( "#IN: %1Bd" ).arg( server->getBaudIn() ) );
        ui->packetOUTBD->setText( QString( "#OUT: %1Bd" ).arg( server->getBaudOut() ) );

        if ( trayIcon != nullptr )
        {
            QString tTip = QString( "ReMix Server: %1 (%2)\r\n" "#Calls: %3\r\n"
                                    "#IN: %4\r\n" "#OUT: %5\r\n" )
                               .arg( server->getName() )
                               .arg( time_s )
                               .arg( server->getUserCalls() )
                               .arg( server->getBaudIn() )
                               .arg( server->getBaudOut() );
            trayIcon->setToolTip( tTip );
        }

        //Update other Info as well.
        if ( server->getIsSetUp() )
        {
            QString tmp = QString( "Listening for incoming calls to %1:%2" )
                                .arg( server->getPrivateIP() )
                                .arg( server->getPrivatePort() );
            if ( server->getIsPublic() )
            {
                QString tmp2 = QString( " ( Need port forward from %1:%2 )" )
                                     .arg( server->getPublicIP() )
                                     .arg( server->getPublicPort() );
                tmp.append( tmp2 );
            }
            ui->networkStatus->setText( tmp );
        }
        ui->playerView->resizeColumnsToContents();
    });
}

void ReMix::parseCMDLArgs()
{
    QStringList args = qApp->arguments();
    QString tmpArg{ "" };
    QString arg{ "" };
    QString tmp{ "" };

    int argIndex{ -1 };
    for ( int i = 0; i < args.count(); ++i )
    {
        arg = args.at( i );
        tmpArg.clear();
        tmp.clear();

        argIndex = -1;
        for ( int j = 0; j < cmdlArgs.count(); ++j )
        {
            if ( arg.contains( cmdlArgs.at( j ), Qt::CaseInsensitive ) )
            {
                tmpArg = cmdlArgs.at( j );
                argIndex = j;
                break;
            }
        }

        if ( !tmpArg.isEmpty() )
        {
            switch ( argIndex )
            {
                case CMDLArgs::GAME:
                    tmp = Helper::getStrStr( arg, tmpArg, "=", "" );
                    if ( !tmp.isEmpty() )
                        server->setGameName( tmp );
                break;
                case CMDLArgs::MASTER:
                    tmp = Helper::getStrStr( arg, tmpArg, "=", "" );
                    if ( !tmp.isEmpty() )
                    {
                        server->setMasterIP( tmp.left( tmp.indexOf( ':' ) ) );
                        server->setMasterPort( tmp.mid( tmp.indexOf( ':' ) + 1 ).toInt() );
                    }
                break;
                case CMDLArgs::PUBLIC:
                    tmp = Helper::getStrStr( arg, tmpArg, "=", "" );
                    if ( !tmp.isEmpty() )
                    {
                        server->setMasterIP( tmp.left( tmp.indexOf( ':' ) ) );
                        server->setMasterPort( tmp.mid( tmp.indexOf( ':' ) + 1 ).toInt() );
                    }
                    ui->isPublicServer->setChecked( true );
                break;
                case CMDLArgs::LISTEN:
                    tmp = Helper::getStrStr( arg, tmpArg, "=", "" );
                    if ( !tmp.isEmpty() )
                    {
                        server->setPrivatePort( tmp.toInt() );
                        if ( server->getMasterIP().isEmpty() )
                            this->getSynRealData();
                    }
                    emit ui->enableNetworking->clicked();
                break;
                case CMDLArgs::NAME:
                    tmp = Helper::getStrStr( arg, tmpArg, "=", "" );
                    if ( !tmp.isEmpty() )
                        server->setName( tmp );
                break;
                case CMDLArgs::FUDGE:
                    server->setLogUsage( true );
                break;
                default:
                    qDebug() << "Unknown Command Line Argument: " << tmp;
                break;
            }
        }
    }

    ui->serverPort->setText( Helper::intToStr( server->getPrivatePort(), 10 ) );
    ui->isPublicServer->setChecked( server->getIsPublic() );
    ui->serverName->setText( server->getName() );
}

void ReMix::getSynRealData()
{
    QFileInfo synRealFile( "synReal.ini" );

    bool downloadFile = true;
    if ( synRealFile.exists() )
    {
        int curTime = static_cast<int>( QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000 );
        int modTime = static_cast<int>( synRealFile.lastModified().toMSecsSinceEpoch() / 1000 );

        downloadFile = ( curTime - modTime >= 172800 ); //Check if the file is 48 hours old and set our bool.
    }

    //The file was older than 48 hours or did not exist. Request a fresh copy.
    if ( downloadFile )
    {
        QTcpSocket *socket = new QTcpSocket;

        socket->connectToHost( "www.synthetic-reality.com", 80 );
        QObject::connect(socket, &QTcpSocket::connected, [socket]()
        {
            socket->write( QByteArray( "GET http://synthetic-reality.com/synreal.ini\r\n" ));
        });

        QObject::connect( socket, &QTcpSocket::readyRead, [socket, this]()
        {
            QFile synreal( "synReal.ini" );
            if ( synreal.open( QIODevice::WriteOnly ) )
                synreal.write( socket->readAll() );

            synreal.close();

            QSettings settings( "synReal.ini", QSettings::IniFormat );
            QString str = settings.value( server->getGameName() % "/master" ).toString();
            int index = str.indexOf( ":" );
            if ( index > 0 )
            {
                server->setMasterIP( str.left( index ) );
                server->setMasterPort( str.mid( index + 1 ).toInt() );
            }
        });

        QObject::connect( socket, &QTcpSocket::disconnected, [socket](){ socket->deleteLater(); } );
    }
    else
    {
        QSettings settings( "synReal.ini", QSettings::IniFormat );
        QString str = settings.value( "WoS/master" ).toString();
        int index = str.indexOf( ":" );
        if ( index > 0 )
        {
            server->setMasterIP( str.left( index ) );
            server->setMasterPort( str.mid( index + 1 ).toInt() );
        }
    }
}

void ReMix::initContextMenu()
{
    contextMenu->clear();

    ui->actionSendMessage->setText( "Send Message" );
    contextMenu->addAction( ui->actionSendMessage );
    contextMenu->addAction( ui->actionRevokeAdmin );
    contextMenu->addAction( ui->actionMakeAdmin );
    contextMenu->addAction( ui->actionDisconnectUser );
    contextMenu->addAction( ui->actionBANISHIPAddress );
    contextMenu->addAction( ui->actionBANISHSerNum );

    contextMenu->insertSeparator( ui->actionDisconnectUser );
}

void ReMix::on_enableNetworking_clicked()
{
    //Setup Networking Objects.
    if ( tcpServer == nullptr )
        tcpServer = new Server( this, server, admin, plrModel );

    ui->enableNetworking->setEnabled( false );
    ui->serverPort->setEnabled( false );
    tcpServer->setupServerInfo();
}

void ReMix::on_openRemoteAdmins_clicked()
{
    if ( admin->isVisible() )
        admin->hide();
    else
        admin->show();
}

void ReMix::on_isPublicServer_stateChanged(int)
{
    //Setup Networking Objects.
    if ( tcpServer == nullptr )
        tcpServer = new Server( this, server, admin, plrModel );

    if ( ui->isPublicServer->isChecked() )
        tcpServer->setupPublicServer( true );   //Setup a connection with the Master Server.
    else
        tcpServer->setupPublicServer( false );   //Disconnect from the Master Server if applicable.
}

void ReMix::on_openSettings_clicked()
{
    if ( settings->isVisible() )
        settings->hide();
    else
        settings->show();
}

void ReMix::on_openSysMessages_clicked()
{
    if ( sysMessages->isVisible() )
        sysMessages->hide();
    else
        sysMessages->show();
}

void ReMix::on_openBanDialog_clicked()
{
    admin->showBanDialog();
}

void ReMix::on_openUserComments_clicked()
{
    tcpServer->showServerComments();
}

void ReMix::on_serverPort_textChanged(const QString &arg1)
{
    int val = arg1.toInt();
    if ( val < 0 || val > 65535 )
        val = 0;

    server->setPrivatePort( val );
}

void ReMix::on_serverName_textChanged(const QString &arg1)
{
    server->setName( arg1 );
}

void ReMix::on_playerView_customContextMenuRequested(const QPoint &pos)
{
    menuIndex = plrProxy->mapToSource( ui->playerView->indexAt( pos ) );

    this->initContextMenu();
    if ( menuIndex.row() >= 0 )
    {
        QString sernum = plrModel->data( plrModel->index( menuIndex.row(), 1 ) ).toString();
        if ( !AdminHelper::getIsRemoteAdmin( sernum ) )
            contextMenu->removeAction( ui->actionRevokeAdmin );
        else
            contextMenu->removeAction( ui->actionMakeAdmin );

        contextMenu->popup( ui->playerView->viewport()->mapToGlobal( pos ) );
    }
    else
    {
        ui->actionSendMessage->setText( "Send Message to Everyone" );
        contextMenu->removeAction( ui->actionRevokeAdmin );
        contextMenu->removeAction( ui->actionMakeAdmin );
        contextMenu->removeAction( ui->actionDisconnectUser );
        contextMenu->removeAction( ui->actionBANISHSerNum );
        contextMenu->removeAction( ui->actionBANISHIPAddress );
    }
    contextMenu->popup( ui->playerView->viewport()->mapToGlobal( pos ) );
}

void ReMix::on_actionSendMessage_triggered()
{
    Player* plr{ nullptr };
    if ( menuIndex.row() >= 0 )
        plr = server->getPlayer( server->getQItemSlot( plrModel->item( menuIndex.row(), 0 ) ) );

    QString title{ "Admin Message:" };
    QString prompt{ "Message to User(s): " };

    bool ok;
    QString txt = Helper::getTextResponse( this, title, prompt, &ok, 1 );
    if ( ok && !txt.isEmpty() )
    {
        quint64 bOut{ 0 };
        if ( plr != nullptr )
            bOut = server->sendMasterMessage( txt, plr, false );
        else
            bOut = server->sendMasterMessage( txt, nullptr, true );

        server->setBytesOut( server->getBytesOut() + bOut );
    }
    menuIndex = QModelIndex();
}

void ReMix::on_actionRevokeAdmin_triggered()
{
    QString msg{ "Your Remote Administrator privileges have been REVOKED by either the "
                 "Server Host or an 'Owner'-ranked Admin. Please contact the Server Host "
                 "if you believe this was in error." };

    QString sernum = plrModel->data( plrModel->index( menuIndex.row(), 1 ) ).toString();
    Player* plr = server->getPlayer( server->getQItemSlot( plrModel->item( menuIndex.row(), 0 ) ) );

    if ( plr != nullptr && plr->getSocket() != nullptr )
    {
        if ( AdminHelper::deleteRemoteAdmin( this, sernum ) )
        {
            //The User is no longer a registered Admin. Revoke their current permissions.
            plr->resetAdminAuth();

            quint64 bOut = server->sendMasterMessage( msg, plr, false );
            server->setBytesOut( server->getBytesOut() + bOut );
            admin->loadServerAdmins();
        }
    }
    menuIndex = QModelIndex();
}

void ReMix::on_actionMakeAdmin_triggered()
{
    QString msg{ "The server Admin is attempting to register you as an Admin with the server. "
                 "Please reply to this message using the (/admin) pop-up dialog with a password of your liking. "
                 "Note: The server Host and other Admins will not have access to this information as it will be hashed+salted. "
                 "///PASSWORD REQUIRED NOW:" };

    QString sernum = plrModel->data( plrModel->index( menuIndex.row(), 1 ) ).toString();;
    Player* plr = server->getPlayer( server->getQItemSlot( plrModel->item( menuIndex.row(), 0 ) ) );

    if ( plr != nullptr
      && plr->getSocket() != nullptr )
    {
        if ( !AdminHelper::getIsRemoteAdmin( sernum )
          && AdminHelper::createRemoteAdmin( this, sernum ) )
        {
            server->sendMasterMessage( msg, plr, false );
            plr->setReqNewAuthPwd( true );
        }
    }
    menuIndex = QModelIndex();
}

void ReMix::on_actionDisconnectUser_triggered()
{
    Player* plr = server->getPlayer( server->getQItemSlot( plrModel->item( menuIndex.row(), 0 ) ) );
    if ( plr != nullptr
      && plr->getSocket() != nullptr )
    {
        QString title{ "Disconnect User:" };
        QString prompt{ "Are you certain you want to DISCONNECT ( " % plr->getSernum_s() % " )?" };
        QString inform{ "The Server Host or a Remote-Admin has disconnected you from the Server. Reason: %1" };

        if ( Helper::confirmAction( this, title, prompt ) )
        {
            inform = inform.arg( Helper::getDisconnectReason( this ) );
            quint64 bOut = server->sendMasterMessage( inform, plr, false );
            server->setBytesOut( server->getBytesOut() + bOut );

            if ( plr->getSocket()->waitForBytesWritten() )
                plr->setSoftDisconnect( true );
        }
    }
    menuIndex = QModelIndex();
}

void ReMix::on_actionBANISHIPAddress_triggered()
{
    QString title{ "Ban IP Address:" };
    QString prompt{ "This command will BANISH the IP Address, which will prevent the User from "
                    "making any connections in the future.\r\nAre you certain?" };
    QString reason{ "Manual Banish; %1" };
    QString inform{ "The Server Host or a Remote-Admin has banned your IP Address ( %1 ). Reason: %2" };

    Player* plr = server->getPlayer( server->getQItemSlot( plrModel->item( menuIndex.row(), 0 ) ) );
    if ( plr != nullptr
      && plr->getSocket() != nullptr )
    {
        QHostAddress ip = plr->getSocket()->peerAddress();
        if ( Helper::confirmAction( this, title, prompt ) )
        {
            reason = reason.arg( Helper::getBanishReason( this ) );
            inform = inform.arg( ip.toString() )
                           .arg( reason ).toLatin1();
            quint64 bOut = server->sendMasterMessage( inform, plr, false );
            server->setBytesOut( server->getBytesOut() + bOut );

            reason = QString( "%1 [ %2:%3 ]: %4" )
                         .arg( reason )
                         .arg( plr->getPublicIP() )
                         .arg( plr->getPublicPort() )
                         .arg( QString( plr->getBioData() ) );
            admin->getBanDialog()->addIPBan( ip, reason );

            if ( plr->getSocket()->waitForBytesWritten() )
                plr->setSoftDisconnect( true );
        }
    }
    menuIndex = QModelIndex();
}

void ReMix::on_actionBANISHSerNum_triggered()
{
    QString sernum = plrModel->data( plrModel->index( menuIndex.row(), 1 ) ).toString();

    QString title{ "Ban SerNum:" };
    QString prompt{ "This command will BANISH the SerNum, which will prevent the User from "
                    "making any connections in the future.\r\nAre you certain?" };
    QString reason{ "Manual Banish; %1" };
    QString inform{ "The Server Host or a Remote-Admin has banned your SerNum ( %1 ). Reason: %2" };

    Player* plr = server->getPlayer( server->getQItemSlot( plrModel->item( menuIndex.row(), 0 ) ) );
    if ( plr != nullptr
      && plr->getSocket() != nullptr )
    {
        if ( Helper::confirmAction( this, title, prompt ) )
        {
            reason = reason.arg( Helper::getBanishReason( this ) );
            inform = inform.arg( sernum )
                           .arg( reason ).toLatin1();
            quint64 bOut = server->sendMasterMessage( inform, plr, false );
            server->setBytesOut( server->getBytesOut() + bOut );

            reason = QString( "%1 [ %2:%3 ]: %4" )
                         .arg( reason )
                         .arg( plr->getPublicIP() )
                         .arg( plr->getPublicPort() )
                         .arg( QString( plr->getBioData() ) );
            admin->getBanDialog()->addSerNumBan( sernum, reason );

            if ( plr->getSocket()->waitForBytesWritten() )
                plr->setSoftDisconnect( true );
        }
    }
    menuIndex = QModelIndex();
}

void ReMix::changeEvent(QEvent* event)
{
#ifndef Q_OS_LINUX
    if ( event->type() == QEvent::WindowStateChange )
    {
        if ( this->isMinimized() )
            this->hide();
    }
    QMainWindow::changeEvent( event );
#endif
}

void ReMix::closeEvent(QCloseEvent* event)
{
    if ( event == nullptr )
        return;

    if ( event->type() == QEvent::Close )
    {
        //Allow rejection of a CloseEvent.
        if ( !this->rejectCloseEvent() )
        {
            event->accept();

            //Explicitly tell the program to close as merely accepting the event
            //may not be enough.
            qApp->quit();
        }
        else
            event->ignore();
    }
    else
        QMainWindow::closeEvent( event );
}

bool ReMix::rejectCloseEvent()
{
    QString title = QString( "Close: [ %1 ]" )
                        .arg( server->getName() );

    QString prompt = QString( "You are about to shut down your ReMix game server!\r\n"
                              "This will affect ( %1 ) User(s) connected to it.\r\n\r\n"
                              "Are you certain?" )
                          .arg( server->getPlayerCount() );

    server->sendMasterMessage( "The admin is taking this server down...", nullptr, true );
    if ( !Helper::confirmAction( this, title, prompt ) )
    {
        server->sendMasterMessage( "The admin changed his or her mind! (yay!)...", nullptr, true );
        return true;
    }
    return false;
}
