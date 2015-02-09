
#include "includes.hpp"
#include "remix.hpp"
#include "ui_remix.h"

//Initialize our accepted Commandline Argument List.
const QStringList ReMix::cmdlArgs =
{
    QStringList() << "game" << "master" << "public"
                  << "listen" << "name" << "fudge"
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
    #if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
        this->initSysTray();
    #endif

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

    //Proxy model to support sorting without actually
    //altering the underlying model
    plrProxy = new QSortFilterProxyModel();
    plrProxy->setDynamicSortFilter( true );
    plrProxy->setSourceModel( plrModel );
    plrProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->playerView->setModel( plrProxy );

    //Setup Objects.
    sysMessages = new Messages( this );
    admin = new Admin( this, server );
    settings = new Settings( this, admin );
    server = new ServerInfo( admin );

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
    this->getSynRealData();

    //Create and Connect Lamda Objects
    this->initUIUpdate();

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

    server->sendMasterInfo( true );
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

#if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
void ReMix::initSysTray()
{
    //While possible to create a system tray icon, some versions of linux
    //disallow applications to create their own.
    //Also disable the feature on OSX. --Unable to test.

    if ( QSystemTrayIcon::isSystemTrayAvailable()
      && !hasSysTray )
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

        QAction* restoreAction = new QAction( "Restore", this );
        QObject::connect( restoreAction, &QAction::triggered,
                          this, &QMainWindow::showNormal );

        QAction* quitAction = new QAction( "Quit", this );
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
                    this->setWindowState( this->windowState()
                                        & ~Qt::WindowMinimized );
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
        hasSysTray = true;
    }
}
#endif

void ReMix::initUIUpdate()
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
        ui->callCount->setText(
                    QString( "#Calls: %1" )
                        .arg( server->getUserCalls() ) );

        ui->packetDCCount->setText(
                    QString( "#Pkt-DC: %1" )
                        .arg( server->getPktDc() ) );

        ui->dupDCCount->setText(
                    QString( "#Dup-DC: %1" )
                        .arg( server->getDupDc() ) );

        ui->ipDCCount->setText(
                    QString( "#IP-DC: %1" )
                        .arg( server->getIpDc() ) );

        ui->packetINBD->setText(
                    QString( "#IN: %1 Bd" )
                        .arg( server->getBaudIn() ) );

        ui->packetOUTBD->setText(
                    QString( "#OUT: %1 Bd" )
                        .arg( server->getBaudOut() ) );

        if ( trayIcon != nullptr )
        {
            trayIcon->setToolTip(
                        QString( "ReMix Server: %1 (%2)\r\n" "#Calls: %3\r\n"
                                 "#Pkt-DC: %4\r\n" "#Dup-DC: %5\r\n"
                                 "#IP-DC: %6\r\n" )
                            .arg( server->getName() )
                            .arg( time_s )
                            .arg( server->getUserCalls() )
                            .arg( server->getPktDc() )
                            .arg( server->getDupDc() )
                            .arg( server->getIpDc() ) );
        }

        //Update other Info as well.
        QString msg{ "Select Port Number and press \"Accept Calls\" "
                     "when ready!" };
        if ( server->getIsSetUp() )
        {
            msg = QString( "Listening for incoming calls to %1:%2" )
                      .arg( server->getPrivateIP() )
                      .arg( server->getPrivatePort() );

            if ( server->getIsPublic() )
            {
                if ( server->getSentUDPCheckin() )
                {
                    if ( server->getMasterUDPResponse() )
                    {
                        QString msg2 = QString( " ( Need port forward from "
                                                "%1:%2 )" )
                                           .arg( server->getPublicIP() )
                                           .arg( server->getPublicPort() );
                        msg.append( msg2 );
                    }
                    else
                        msg = { "Sent UDP check-in to Master. "
                                "Waiting for response..." };
                }
            }
            ui->playerView->resizeColumnsToContents();
        }
        ui->networkStatus->setText( msg );
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
                        server->setMasterInfoHost( tmp );
                        this->getSynRealData();
                    }
                break;
                case CMDLArgs::PUBLIC:
                    tmp = Helper::getStrStr( arg, tmpArg, "=", "" );
                    if ( !tmp.isEmpty() )
                    {
                        server->setMasterIP( tmp.left( tmp.indexOf( ':' ) ) );
                        server->setMasterPort(
                                    static_cast<quint16>(
                                        tmp.mid( tmp.indexOf( ':' ) + 1 )
                                           .toInt() ) );
                    }
                    ui->isPublicServer->setChecked( true );
                break;
                case CMDLArgs::LISTEN:
                    tmp = Helper::getStrStr( arg, tmpArg, "=", "" );
                    if ( !tmp.isEmpty() )
                        server->setPrivatePort( tmp.toInt() );

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

    ui->serverPort->setText(
                Helper::intToStr(
                    server->getPrivatePort(), 10 ) );

    ui->isPublicServer->setChecked( server->getIsPublic() );
    ui->serverName->setText( server->getName() );
}

void ReMix::getSynRealData()
{
    QFileInfo synRealFile( "synReal.ini" );

    bool downloadFile = true;
    if ( synRealFile.exists() )
    {
        qint64 curTime = static_cast<qint64>(
                             QDateTime::currentDateTime()
                                  .toMSecsSinceEpoch() / 1000 );
        qint64 modTime = static_cast<qint64>(
                             synRealFile.lastModified()
                                  .toMSecsSinceEpoch() / 1000 );

        //Check if the file is 48 hours old and set our bool.
        downloadFile = ( curTime - modTime >= 172800 );
    }

    //The file was older than 48 hours or did not exist. Request a fresh copy.
    if ( downloadFile )
    {
        QTcpSocket* socket = new QTcpSocket;
        QUrl url( server->getMasterInfoHost() );

        socket->connectToHost( url.host(), 80 );
        QObject::connect( socket, &QTcpSocket::connected, [=]()
        {
            socket->write( QString( "GET %1\r\n" )
                               .arg( server->getMasterInfoHost() )
                               .toLatin1() );
        });

        QObject::connect( socket, &QTcpSocket::readyRead, [=]()
        {
            QFile synreal( "synReal.ini" );
            if ( synreal.open( QIODevice::WriteOnly ) )
                synreal.write( socket->readAll() );

            synreal.close();

            QSettings settings( "synReal.ini", QSettings::IniFormat );
            QString str = settings.value( server->getGameName()
                                        % "/master" ).toString();
            int index = str.indexOf( ":" );
            if ( index > 0 )
            {
                server->setMasterIP( str.left( index ) );
                server->setMasterPort(
                            static_cast<quint16>(
                                str.mid( index + 1 ).toInt() ) );
            }
        });

        QObject::connect( socket, &QTcpSocket::disconnected,
                          socket, &QTcpSocket::deleteLater );
    }
    else
    {
        QSettings settings( "synReal.ini", QSettings::IniFormat );
        QString str = settings.value( server->getGameName()
                                    % "/master" ).toString();
        if ( !str.isEmpty() )
        {
            int index = str.indexOf( ":" );
            if ( index > 0 )
            {
                server->setMasterIP( str.left( index ) );
                server->setMasterPort(
                            static_cast<quint16>(
                                str.mid( index + 1 ).toInt() ) );
            }
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
    contextMenu->addAction( ui->actionMuteNetwork );
    contextMenu->addAction( ui->actionUnMuteNetwork );
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
        //Setup a connection with the Master Server.
        tcpServer->setupPublicServer( true );
    else   //Disconnect from the Master Server if applicable.
        tcpServer->setupPublicServer( false );
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
    qint32 val = arg1.toInt();
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
    QModelIndex menuIndex = plrProxy->mapToSource(
                                ui->playerView->indexAt( pos ) );

    this->initContextMenu();
    if ( menuIndex.row() >= 0 )
    {
        Player* plr = server->getPlayer(
                          server->getQItemSlot(
                              plrModel->item( menuIndex.row(), 0 ) ) );
        if ( plr != nullptr )
            menuTarget = plr;

        QString sernum = menuTarget->getSernum_s();
        if ( !admin->getIsRemoteAdmin( sernum ) )
            contextMenu->removeAction( ui->actionRevokeAdmin );
        else
            contextMenu->removeAction( ui->actionMakeAdmin );

        if ( plr != nullptr )
        {
            if( plr->getNetworkMuted() )
                contextMenu->removeAction( ui->actionMuteNetwork );
            else
                contextMenu->removeAction( ui->actionUnMuteNetwork );
        }
        contextMenu->popup( ui->playerView->viewport()->mapToGlobal( pos ) );
    }
    else
    {
        ui->actionSendMessage->setText( "Send Message to Everyone" );
        contextMenu->removeAction( ui->actionRevokeAdmin );
        contextMenu->removeAction( ui->actionMakeAdmin );
        contextMenu->removeAction( ui->actionMuteNetwork );
        contextMenu->removeAction( ui->actionUnMuteNetwork );
        contextMenu->removeAction( ui->actionDisconnectUser );
        contextMenu->removeAction( ui->actionBANISHSerNum );
        contextMenu->removeAction( ui->actionBANISHIPAddress );
    }
    contextMenu->popup( ui->playerView->viewport()->mapToGlobal( pos ) );
}

void ReMix::on_actionSendMessage_triggered()
{
    QString title{ "Admin Message:" };
    QString prompt{ "Message to User(s): " };
    quint64 bOut{ 0 };

    bool ok{ false };
    QString txt = Helper::getTextResponse( this, title, prompt, &ok, 1 );
    if ( !txt.isEmpty()
      && ok )
    {
        if ( menuTarget != nullptr )
            bOut = server->sendMasterMessage( txt, menuTarget, false );
        else
            bOut = server->sendMasterMessage( txt, nullptr, true );

        server->setBytesOut( server->getBytesOut() + bOut );
    }
    menuTarget = nullptr;
}

void ReMix::on_actionRevokeAdmin_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString sernum = menuTarget->getSernum_s();
    QString msg{ "Your Remote Administrator privileges have been REVOKED "
                 "by either the Server Host or an 'Owner'-ranked Admin. "
                 "Please contact the Server Host if you believe this was "
                 "in error." };

    if ( menuTarget->getSocket() != nullptr )
    {
        if ( admin->deleteRemoteAdmin( this, sernum ) )
        {
            //The User is no longer a registered Admin.
            //Revoke their current permissions.
            menuTarget->resetAdminAuth();

            quint64 bOut = server->sendMasterMessage( msg, menuTarget, false );
            server->setBytesOut( server->getBytesOut() + bOut );
            admin->loadServerAdmins();
        }
    }
    menuTarget = nullptr;
}

void ReMix::on_actionMakeAdmin_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString sernum = menuTarget->getSernum_s();
    QString msg{ "The server Admin is attempting to register you as an "
                 "Admin with the server. Please reply to this message with "
                 "(/register *YOURPASS). Note: The server Host and other Admins"
                 " will not have access to this information." };

    if ( !admin->getIsRemoteAdmin( sernum ) )
    {
        if ( admin->createRemoteAdmin( this, sernum ) )
        {
            server->sendMasterMessage( msg, menuTarget, false );
            menuTarget->setReqNewAuthPwd( true );
        }
    }
    menuTarget = nullptr;
}

void ReMix::on_actionMuteNetwork_triggered()
{
    //Mute the selected User's Network.
    //We will not inform the User of this event.
    //This tells the Server to not re-send incoming
    //packets from this User to other connected Users.

    if ( menuTarget != nullptr )
    {
        QString title{ "Mute User:" };
        QString prompt{ "Are you certain you want to MUTE ( " %
                        menuTarget->getSernum_s() % " )'s Network?" };

        if ( Helper::confirmAction( this, title, prompt ) )
            menuTarget->setNetworkMuted( true );
    }
    menuTarget = nullptr;
}

void ReMix::on_actionUnMuteNetwork_triggered()
{
    //Un-Mute the selected User's Network.
    //We do not inform the User of this event.
    //This tells the Server to re-send incoming
    //packets from this User to other connected Users.

    if ( menuTarget != nullptr )
    {
        QString title{ "Un-Mute User:" };
        QString prompt{ "Are you certain you want to UN-MUTE ( " %
                        menuTarget->getSernum_s() % " )'s Network?" };

        if ( Helper::confirmAction( this, title, prompt ) )
            menuTarget->setNetworkMuted( false );
    }
    menuTarget = nullptr;
}

void ReMix::on_actionDisconnectUser_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QTcpSocket* sock = menuTarget->getSocket();
    if ( sock != nullptr )
    {
        QString title{ "Disconnect User:" };
        QString prompt{ "Are you certain you want to DISCONNECT ( " %
                        menuTarget->getSernum_s() % " )?" };

        QString inform{ "The Server Host or a Remote-Admin has disconnected "
                        "you from the Server. Reason: %1" };

        if ( Helper::confirmAction( this, title, prompt ) )
        {
            inform = inform.arg( Helper::getDisconnectReason( this ) );
            quint64 bOut = server->sendMasterMessage( inform, menuTarget,
                                                      false );
            server->setBytesOut( server->getBytesOut() + bOut );

            if ( sock->waitForBytesWritten() )
            {
                menuTarget->setSoftDisconnect( true );
                server->setIpDc( server->getIpDc() + 1 );
            }
        }
    }
    menuTarget = nullptr;
}

void ReMix::on_actionBANISHIPAddress_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString sernum = menuTarget->getSernum_s();
    QString ipAddr = menuTarget->getPublicIP();

    QString title{ "Ban IP Address:" };
    QString prompt{ "Are you certain you want to BANISH [ " % sernum % " ]'s "
                    "IP Address [ " % ipAddr % " ]?" };

    QString inform{ "The Server Host or a Remote-Admin has banned your IP "
                    "Address [ %1 ]. Reason: %2" };
    QString reason{ "Manual Banish; %1" };

    QTcpSocket* sock = menuTarget->getSocket();
    if ( sock != nullptr )
    {
        QHostAddress ip = sock->peerAddress();
        if ( Helper::confirmAction( this, title, prompt ) )
        {
            reason = reason.arg( Helper::getBanishReason( this ) );
            inform = inform.arg( ip.toString() )
                           .arg( reason ).toLatin1();
            quint64 bOut = server->sendMasterMessage( inform, menuTarget,
                                                      false );
            server->setBytesOut( server->getBytesOut() + bOut );

            reason = QString( "%1 [ %2:%3 ]: %4" )
                         .arg( reason )
                         .arg( menuTarget->getPublicIP() )
                         .arg( menuTarget->getPublicPort() )
                         .arg( QString( menuTarget->getBioData() ) );
            admin->getBanDialog()->addIPBan( ip, reason );

            if ( sock->waitForBytesWritten() )
            {
                menuTarget->setSoftDisconnect( true );
                server->setIpDc( server->getIpDc() + 1 );
            }
        }
    }
    menuTarget = nullptr;
}

void ReMix::on_actionBANISHSerNum_triggered()
{
    if ( menuTarget == nullptr )
        return;

    QString sernum = menuTarget->getSernum_s();

    QString title{ "Ban SerNum:" };
    QString prompt{ "Are you certain you want to BANISH the SerNum [ "
                  % sernum % " ]?" };

    QString inform{ "The Server Host or a Remote-Admin has banned your "
                    "SerNum [ %1 ]. Reason: %2" };
    QString reason{ "Manual Banish; %1" };

    QTcpSocket* sock = menuTarget->getSocket();
    if ( sock != nullptr )
    {
        if ( Helper::confirmAction( this, title, prompt ) )
        {
            reason = reason.arg( Helper::getBanishReason( this ) );
            inform = inform.arg( sernum )
                           .arg( reason ).toLatin1();
            quint64 bOut = server->sendMasterMessage( inform, menuTarget,
                                                      false );
            server->setBytesOut( server->getBytesOut() + bOut );

            reason = QString( "%1 [ %2:%3 ]: %4" )
                         .arg( reason )
                         .arg( menuTarget->getPublicIP() )
                         .arg( menuTarget->getPublicPort() )
                         .arg( QString( menuTarget->getBioData() ) );
            admin->getBanDialog()->addSerNumBan( sernum, reason );

            if ( sock->waitForBytesWritten() )
            {
                menuTarget->setSoftDisconnect( true );
                server->setIpDc( server->getIpDc() + 1 );
            }
        }
    }
    menuTarget = nullptr;
}

#if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
void ReMix::changeEvent(QEvent* event)
{
    if ( hasSysTray )
    {
        if ( event->type() == QEvent::WindowStateChange )
        {
            if ( this->isMinimized() )
                this->hide();
        }
    }
    QMainWindow::changeEvent( event );
}
#endif

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

    QString prompt = QString( "You are about to shut down your ReMix game "
                              "server!\r\nThis will affect [ %1 ] User(s) "
                              "connected to it.\r\n\r\nAre you certain?" )
                         .arg( server->getPlayerCount() );

    server->sendMasterMessage( "The admin is taking this server down...",
                               nullptr, true );
    if ( !Helper::confirmAction( this, title, prompt ) )
    {
        server->sendMasterMessage( "The admin changed his or her mind! "
                                   "(yay!)...", nullptr, true );
        return true;
    }
    return false;
}
