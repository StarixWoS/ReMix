
#include "remix.hpp"
#include "ui_remix.h"

#include "helper.hpp"
#include "usermessage.hpp"
#include "messages.hpp"
#include "server.hpp"
#include "settings.hpp"
#include "admin.hpp"

ReMix::ReMix(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReMix)
{
    ui->setupUi(this);

    //Setup our Random Device
    randDev = new RandDev();

    //Create our Context Menus
    contextMenu = new QMenu( this );

    //Initialize our Context Menu Items.
    this->initContextMenu();

    //Setup the PlayerInfo TableView.
    plrViewModel = new QStandardItemModel( 0, 8, 0 );
    plrViewModel->setHeaderData( 0, Qt::Horizontal, "Player IP:Port" );
    plrViewModel->setHeaderData( 1, Qt::Horizontal, "SerNum" );
    plrViewModel->setHeaderData( 2, Qt::Horizontal, "Age" );
    plrViewModel->setHeaderData( 3, Qt::Horizontal, "Alias" );
    plrViewModel->setHeaderData( 4, Qt::Horizontal, "Time" );
    plrViewModel->setHeaderData( 5, Qt::Horizontal, "IN" );
    plrViewModel->setHeaderData( 6, Qt::Horizontal, "OUT" );
    plrViewModel->setHeaderData( 7, Qt::Horizontal, "BIO" );

    //Proxy model to support sorting without actually altering the underlying model
    plrViewProxy = new QSortFilterProxyModel();
    plrViewProxy->setDynamicSortFilter( true );
    plrViewProxy->setSourceModel( plrViewModel );
    plrViewProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->playerView->setModel( plrViewProxy );

    //Setup Dialog Objects.
    sysMessages = new Messages( this );
    usrMsg = new UserMessage( this );
    admin = new Admin( this );
    settings = new Settings( this );

    //Setup Server/Player Info objects.
    serverInfo = new ServerInfo();
    serverInfo->setServerID( Helper::getServerID() );
    if ( serverInfo->getServerID() <= 0 )
    {
        QVariant value = this->genServerID();
        serverInfo->setServerID( value.toInt() );

        Helper::setServerID( value );
    }
    serverInfo->setHostInfo( QHostInfo() );
    serverInfo->setServerRules( Helper::getServerRules() );

    this->parseCMDLArgs();

    //Create and Connect Lamda Objects
    QObject::connect( serverInfo->getUpTimer(), &QTimer::timeout, [=]()
    {
        quint64 time = serverInfo->getUpTime();
        ui->onlineTime->setText( QString( "%1:%2:%3" )
                                 .arg( time / 3600, 2, 10, QChar( '0' ) )
                                 .arg(( time / 60 ) % 60, 2, 10, QChar( '0' ) )
                                 .arg( time % 60, 2, 10, QChar( '0' ) ) );

        ui->callCount->setText( QString( "#Calls: %1" ).arg( serverInfo->getUserCalls() ) );
        ui->packetINBD->setText( QString( "#IN: %1Bd" ).arg( serverInfo->getBaudIn() ) );
        ui->packetOUTBD->setText( QString( "#OUT: %1Bd" ).arg( serverInfo->getBaudOut() ) );

        //Update other Info as well.
        if ( serverInfo->getIsSetUp() )
        {
            QString tmp = QString( "Listening for incoming calls to %1:%2" )
                                .arg( serverInfo->getPrivateIP() )
                                .arg( serverInfo->getPrivatePort() );
            if ( serverInfo->getIsPublic() )
            {
                QString tmp2 = QString( " ( Need port forward from %1:%2 )" )
                                     .arg( serverInfo->getPublicIP() )
                                     .arg( serverInfo->getPublicPort() );
                tmp.append( tmp2 );
            }
            ui->networkStatus->setText( tmp );
        }
        ui->playerView->resizeColumnsToContents();
    });

    if ( serverInfo->getMasterIP().isEmpty() )
        this->getSynRealData();

    //Setup Networking Objects.
    if ( tcpServer == nullptr )
        tcpServer = new Server( serverInfo, plrViewModel );
}

ReMix::~ReMix()
{
    sysMessages->close();
    sysMessages->deleteLater();

    usrMsg->close();
    usrMsg->deleteLater();

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
                serverInfo->setGameName( arg.mid( index + 1 ) );
        }
        else if ( arg.startsWith( "/master", Qt::CaseInsensitive ) )
        {
            index = arg.indexOf( '=' );
            if ( index > 0 )
            {
               tmpArg = arg.mid( index + 1 );
               if ( !tmpArg.isEmpty() )
               {
                   serverInfo->setMasterIP( tmpArg.left( tmpArg.indexOf( ':' ) ) );
                   serverInfo->setMasterPort( tmpArg.mid( tmpArg.indexOf( ':' ) + 1 ).toInt() );
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
                   serverInfo->setMasterIP( tmpArg.left( tmpArg.indexOf( ':' ) ) );
                   serverInfo->setMasterPort( tmpArg.mid( tmpArg.indexOf( ':' ) + 1 ).toInt() );
               }
            }
            ui->isPublicServer->setChecked( true );
        }
        else if ( arg.startsWith( "/listen", Qt::CaseInsensitive ) )
        {
            index = arg.indexOf( '=' );
            if ( index > 0 )
                serverInfo->setPrivatePort( arg.mid( index + 1 ).toInt() );

            if ( serverInfo->getMasterIP().isEmpty() )
                this->getSynRealData();

            emit ui->enableNetworking->clicked();
        }
        else if ( arg.startsWith( "/name", Qt::CaseInsensitive ) )
        {
            tmpArg = arg.mid( arg.indexOf( '=' ) + 1 );
            if ( !tmpArg.isEmpty() )
                serverInfo->setName( tmpArg );
        }
    }

    ui->serverPort->setText( QString::number( serverInfo->getPrivatePort() ) );
    ui->isPublicServer->setChecked( serverInfo->getIsPublic() );
    ui->serverName->setText( serverInfo->getName() );
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
            QString str = settings.value( serverInfo->getGameName() + "/master" ).toString();
            int index = str.indexOf( ":" );
            if ( index > 0 )
            {
                serverInfo->setMasterIP( str.left( index ) );
                serverInfo->setMasterPort( str.mid( index + 1 ).toInt() );
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
            serverInfo->setMasterIP( str.left( index ) );
            serverInfo->setMasterPort( str.mid( index + 1 ).toInt() );
        }
    }
}

void ReMix::initContextMenu()
{
    contextMenu->clear();
    contextMenu->addAction( ui->actionSendMessage );
    contextMenu->addAction( ui->actionRevokeAdmin );
    contextMenu->addAction( ui->actionMakeAdmin );
    contextMenu->addAction( ui->actionBANISHIPAddress );
    contextMenu->addAction( ui->actionBANISHSerNum );

    contextMenu->insertSeparator( ui->actionBANISHIPAddress );
}

void ReMix::on_enableNetworking_clicked()
{
    //Setup Networking Objects.
    if ( tcpServer == nullptr )
        tcpServer = new Server( serverInfo, plrViewModel );

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
    if ( usrMsg->isVisible() )
        usrMsg->hide();
    else
        usrMsg->show();
}

void ReMix::on_serverPort_textChanged(const QString &arg1)
{
    int val = arg1.toInt();
    if ( val < 0 || val > 65535 )
        val = 0;

    serverInfo->setPrivatePort( val );
}

void ReMix::on_serverName_textChanged(const QString &arg1)
{
    serverInfo->setName( arg1 );
}

void ReMix::on_playerView_customContextMenuRequested(const QPoint &pos)
{
    menuIndex = plrViewProxy->mapToSource( ui->playerView->indexAt( pos ) );
    if ( menuIndex.row() < 0 )
        return;

    QString sernum = plrViewModel->data( plrViewModel->index( menuIndex.row(), 1 ) ).toString();

    this->initContextMenu();
    if ( !AdminHelper::getIsRemoteAdmin( sernum ) )
        contextMenu->removeAction( ui->actionRevokeAdmin );
    else
        contextMenu->removeAction( ui->actionMakeAdmin );

    contextMenu->popup( ui->playerView->viewport()->mapToGlobal( pos ) );
}

void ReMix::on_actionSendMessage_triggered()
{
}

void ReMix::on_actionRevokeAdmin_triggered()
{
    ; //Delete the User from the Admin storage. If the User is online, inform them of this change.
}

void ReMix::on_actionMakeAdmin_triggered()
{
    QString sernum{ "" };
    if ( menuIndex.isValid() )
    {
        sernum = plrViewModel->data( plrViewModel->index( menuIndex.row(), 1 ) ).toString();
        if ( !sernum.isEmpty() && !AdminHelper::getIsRemoteAdmin( sernum ) )
        {
            ; //TODO: Request a Password from the selected User.
        }
    }
}

void ReMix::on_actionBANISHIPAddress_triggered()
{
    ; //TODO: Implement hooks to the BannedIP class. Ban the IP address..
}

void ReMix::on_actionBANISHSerNum_triggered()
{
    ;  //TODO: Implement hooks to the BannedSernum class. Ban the sernum.
}
