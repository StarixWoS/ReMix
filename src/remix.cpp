
#include "remix.hpp"
#include "ui_remix.h"

#include "helper.hpp"
#include "usermessage.hpp"
#include "messages.hpp"
#include "server.hpp"
#include "settings.hpp"
#include "admin.hpp"
#include "bandialog.hpp"

ReMix::ReMix(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReMix)
{
    ui->setupUi(this);

    //Setup our Random Device
    randDev = new RandDev();

    //Create our Context Menus
    contextMenu = new QMenu( this );

    //Create our System-Tray Icon if Possible.
    if ( QSystemTrayIcon::isSystemTrayAvailable() )
    {
        trayIcon = new QSystemTrayIcon( QIcon( ":/icon/ReMix.ico" ), this );
        trayIcon->show();

        QObject::connect( trayIcon, &QSystemTrayIcon::activated,
                          [=]( QSystemTrayIcon::ActivationReason reason )
        {
            if ( reason == QSystemTrayIcon::Trigger )
            {
                if ( this->isHidden() )
                    this->show();
                else
                    this->hide();
            }
        });
    }

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
    admin = new Admin( this );
    settings = new Settings( this );

    //Setup Server/Player Info objects.
    server = new ServerInfo();
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

    //Setup Networking Objects.
    if ( tcpServer == nullptr )
        tcpServer = new Server( this, server, admin, plrModel );
}

ReMix::~ReMix()
{
    sysMessages->close();
    sysMessages->deleteLater();

    tcpServer->disconnectFromMaster();
    tcpServer->close();
    tcpServer->deleteLater();

    delete ui;
}

int ReMix::genServerID()
{
    int id = randDev->genRandNum( 0, 32767 );
        id = id << 4;
        id = id ^ randDev->genRandNum( 0, 32767 );
        id = id << 4;
        id = id ^ ( randDev->genRandNum( 0, 32767 ) << 10 );
        id = id ^ randDev->genRandNum( 0, 32767 );
        id = id << 4;
        id = id ^ ( randDev->genRandNum( 0, 32767 ) << 10 );
        id = id ^ randDev->genRandNum( 0, 32767 );
    return id;
}

void ReMix::parseCMDLArgs()
{
    QStringList args = qApp->arguments();

    QString tmpArg;
    for ( int i = 0; i < args.count(); ++i )
    {
        int index = 0;

        QString arg = args.at( i );
        if ( arg.startsWith( "/game", Qt::CaseInsensitive ) )
        {
            index = arg.indexOf( '=' );
            if ( index > 0 )
                server->setGameName( arg.mid( index + 1 ) );
        }
        else if ( arg.startsWith( "/master", Qt::CaseInsensitive ) )
        {
            index = arg.indexOf( '=' );
            if ( index > 0 )
            {
               tmpArg = arg.mid( index + 1 );
               if ( !tmpArg.isEmpty() )
               {
                   server->setMasterIP( tmpArg.left( tmpArg.indexOf( ':' ) ) );
                   server->setMasterPort( tmpArg.mid( tmpArg.indexOf( ':' ) + 1 ).toInt() );
               }
            }
        }
        else if ( arg.startsWith( "/public", Qt::CaseInsensitive ) )
        {
            index = arg.indexOf( '=' );
            if ( index > 0 )
            {
               tmpArg = arg.mid( index + 1 );
               if ( !tmpArg.isEmpty() )
               {
                   server->setMasterIP( tmpArg.left( tmpArg.indexOf( ':' ) ) );
                   server->setMasterPort( tmpArg.mid( tmpArg.indexOf( ':' ) + 1 ).toInt() );
               }
            }
            ui->isPublicServer->setChecked( true );
        }
        else if ( arg.startsWith( "/listen", Qt::CaseInsensitive ) )
        {
            index = arg.indexOf( '=' );
            if ( index > 0 )
                server->setPrivatePort( arg.mid( index + 1 ).toInt() );

            if ( server->getMasterIP().isEmpty() )
                this->getSynRealData();

            emit ui->enableNetworking->clicked();
        }
        else if ( arg.startsWith( "/name", Qt::CaseInsensitive ) )
        {
            tmpArg = arg.mid( arg.indexOf( '=' ) + 1 );
            if ( !tmpArg.isEmpty() )
                server->setName( tmpArg );
        }
    }

    ui->serverPort->setText( QString::number( server->getPrivatePort() ) );
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
            QString str = settings.value( server->getGameName() + "/master" ).toString();
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
        contextMenu->removeAction( ui->actionBANISHSerNum );
        contextMenu->removeAction( ui->actionBANISHIPAddress );
    }
    contextMenu->popup( ui->playerView->viewport()->mapToGlobal( pos ) );
}

void ReMix::on_actionSendMessage_triggered()
{
    Player* plr{ nullptr };
    QString msg{ "" };
    if ( menuIndex.row() >= 0 )
        plr = server->getPlayer( server->getQItemSlot( plrModel->item( menuIndex.row(), 0 ) ) );

    QString title{ "Admin Message:" };
    QString prompt{ "Message to User(s): " };

    bool ok;
    QString txt = Helper::getTextResponse( this, title, prompt, &ok, 1 );
    if ( ok && !txt.isEmpty() )
    {
        msg = QString( ":SR@M%1\r\n" )
                  .arg( txt );

        if ( plr != nullptr && plr->getSocket() != nullptr )
            plr->getSocket()->write( msg.toLatin1() );
        else
        {
            for ( int i = 0; i < MAX_PLAYERS; ++i )
            {
                plr = server->getPlayer( i );
                if ( plr != nullptr && plr->getSocket() != nullptr )
                    plr->getSocket()->write( msg.toLatin1() );
            }
        }
    }
    menuIndex = QModelIndex();
}

void ReMix::on_actionRevokeAdmin_triggered()
{
    QString sernum = plrModel->data( plrModel->index( menuIndex.row(), 1 ) ).toString();
    if ( !sernum.isEmpty()
      && AdminHelper::deleteRemoteAdmin( this, sernum ) )
    {
        Player* plr = server->getPlayer( server->getQItemSlot( plrModel->item( menuIndex.row(), 0 ) ) );
        if ( plr != nullptr && plr->getSocket() != nullptr )
        {
            plr->getSocket()->write( ":SR@MYour Remote Administrator privileges have been REVOKED by either the "
                                     "Server Host or an 'Owner'-ranked Admin. Please contact the Server Host "
                                     "if this was in error.\r\n" );
            admin->loadServerAdmins();
        }
    }
    menuIndex = QModelIndex();
}

void ReMix::on_actionMakeAdmin_triggered()
{
    QString sernum{ "" };
    if ( menuIndex.isValid() )
    {
        sernum = plrModel->data( plrModel->index( menuIndex.row(), 1 ) ).toString();
        if ( !sernum.isEmpty() && !AdminHelper::getIsRemoteAdmin( sernum ) )
        {
            ; //TODO: Request a Password from the selected User.
        }
    }
}

void ReMix::on_actionDisconnectUser_triggered()
{
    QString sernum = plrModel->data( plrModel->index( menuIndex.row(), 1 ) ).toString();
    if ( !sernum.isEmpty() )
    {
        Player* plr = server->getPlayer( server->getQItemSlot( plrModel->item( menuIndex.row(), 0 ) ) );
        if ( plr != nullptr && plr->getSocket() != nullptr )
        {
            QString title{ "Disconnect User:" };
            QString prompt{ "Are you certain you want to DISCONNECT ( " + plr->getSernum_s() + " )?" };
            if ( Helper::confirmAction( this, title, prompt ) )
            {
                plr->getSocket()->write( ":SR@MThe server Host or a Remote-Admin has disconnected you from the Server. "
                                         "Please contact the Server Host if this was in error.\r\n" );
                plr->getSocket()->waitForBytesWritten( 100 );
                plr->getSocket()->abort();
            }
        }
    }
    menuIndex = QModelIndex();
}

void ReMix::on_actionBANISHIPAddress_triggered()
{
    QString sernum = plrModel->data( plrModel->index( menuIndex.row(), 1 ) ).toString();
    if ( !sernum.isEmpty() )
    {
        QString title{ "Ban IP Address:" };
        QString prompt{ "This command will BANISH the IP Address, which will prevent the User from "
                        "making any connections in the future.\r\nAre you certain?" };

        Player* plr = server->getPlayer( server->getQItemSlot( plrModel->item( menuIndex.row(), 0 ) ) );
        if ( plr != nullptr
          && plr->getSocket() != nullptr )
        {
            QHostAddress ip = plr->getSocket()->peerAddress();
            if ( Helper::confirmAction( this, title, prompt ) )
            {
                admin->getBanDialog()->addIPBan( ip, sernum );
                plr->getSocket()->abort();
            }
        }
    }
    menuIndex = QModelIndex();
}

void ReMix::on_actionBANISHSerNum_triggered()
{
    ;  //TODO: Implement hooks to the BannedSernum class. Ban the sernum.
}
