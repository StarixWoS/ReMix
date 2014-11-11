
#include "remix.hpp"
#include "ui_remix.h"

#include "bannedsernum.hpp"
#include "preferences.hpp"
#include "usermessage.hpp"
#include "messages.hpp"
#include "bannedip.hpp"
#include "server.hpp"

namespace Helper
{
    namespace RandDev
    {
        std::mt19937 randDevice( QDateTime::currentMSecsSinceEpoch() );
    }

    int genRandNum(int min, int max)
    {
        std::uniform_int_distribution<int> randInt( min, max );
        return randInt( RandDev::randDevice );
    }
}

ReMix::ReMix(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReMix)
{
    ui->setupUi(this);

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

    //Setup the ServerInfo TableView.
    svrViewModel = new QStandardItemModel( 0, 6, 0 );
    svrViewModel->setHeaderData( 0, Qt::Horizontal, "Server IP:Port" );
    svrViewModel->setHeaderData( 1, Qt::Horizontal, "Since" );
    svrViewModel->setHeaderData( 2, Qt::Horizontal, "#Players" );
    svrViewModel->setHeaderData( 3, Qt::Horizontal, "GameID" );
    svrViewModel->setHeaderData( 4, Qt::Horizontal, "Version" );
    svrViewModel->setHeaderData( 5, Qt::Horizontal, "Settings" );

    //Proxy model to support sorting without actually altering the underlying model
    svrViewProxy = new QSortFilterProxyModel();
    svrViewProxy->setDynamicSortFilter( true );
    svrViewProxy->setSourceModel( svrViewModel );
    svrViewProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->serverView->setModel( svrViewProxy );

    //ui->serverView->hide();

    //Setup Dialog Objects.
    sernumBan = new BannedSernum( this );
    sysMessages = new Messages( this );
    usrMsg = new UserMessage( this );
    banIP = new BannedIP( this );

    //Setup Server/Player Info objects.
    serverInfo = new ServerInfo();
    serverInfo->serverID = this->genServerID();
    serverInfo->hostInfo = QHostInfo();
    serverInfo->serverRules = Preferences::getServerRules();
    this->parseCMDLArgs();
    this->getSynRealData();

    ui->serverView->hide();
    if ( serverInfo->isMaster )
        ui->serverView->show();

    //Setup Networking Objects.
    tcpServer = new Server( this );

    //Connect Objects to Slots.

}

ReMix::~ReMix()
{
    sysMessages->close();
    sysMessages->deleteLater();

    sernumBan->close();
    sernumBan->deleteLater();

    usrMsg->close();
    usrMsg->deleteLater();

    banIP->close();
    banIP->deleteLater();

    tcpServer->disconnectFromMaster();
    tcpServer->close();
    tcpServer->deleteLater();

    delete ui;
}

int ReMix::genServerID()
{
    int id = Helper::genRandNum();
        id = id << 4;
        id = id ^ Helper::genRandNum();
        id = id << 4;
        id = id ^ ( Helper::genRandNum() << 10 );
        id = id ^ Helper::genRandNum();
        id = id << 4;
        id = id ^ ( Helper::genRandNum() << 10 );
        id = id ^ Helper::genRandNum();
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
                serverInfo->gameName = arg.mid( index + 1 );
            else
                serverInfo->gameName = "WoS";
        }
        else if ( arg.startsWith( "/mixmaster", Qt::CaseInsensitive ) )
        {
            index = arg.indexOf( '=' );
            if ( index > 0 )
            {
                serverInfo->isMaster = true;
                serverInfo->masterPort = arg.mid( index + 1 ).toInt();
            }
            else
                serverInfo->masterPort = 23999;
        }
        else if ( arg.startsWith( "/master", Qt::CaseInsensitive ) )
        {
            index = arg.indexOf( '=' );
            if ( index > 0 )
            {
               tmpArg = arg.mid( index + 1 );
               if ( !tmpArg.isEmpty() )
               {
                   serverInfo->masterIP = tmpArg.left( tmpArg.indexOf( ':' ) );
                   serverInfo->masterPort = tmpArg.mid( tmpArg.indexOf( ':' ) + 1 ).toInt();
               }
               else
               {
                   serverInfo->masterIP = "209.233.24.166";
                   serverInfo->masterPort = 23999;
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
                   serverInfo->masterIP = tmpArg.left( tmpArg.indexOf( ':' ) );
                   serverInfo->masterPort = tmpArg.mid( tmpArg.indexOf( ':' ) + 1 ).toInt();
                   serverInfo->isPublic = true;
               }
               else
               {
                   serverInfo->masterIP = "209.233.24.166";
                   serverInfo->masterPort = 23999;
               }
            }

        }
        else if ( arg.startsWith( "/listen", Qt::CaseInsensitive ) )
        {
            index = arg.indexOf( '=' );
            if ( index > 0 )
                serverInfo->privatePort = arg.mid( index + 1 ).toInt();
            else
                serverInfo->privatePort = 0;
        }
        else if ( arg.startsWith( "/name", Qt::CaseInsensitive ) )
        {
            tmpArg = arg.mid( arg.indexOf( '=' ) + 1 );
            if ( !tmpArg.isEmpty() )
                serverInfo->name = tmpArg;
            else
                serverInfo->name = "AHitB ReMix Server";
        }
//        else if ( arg.startsWith( "/url", Qt::CaseInsensitive ) )
//        {
//            index = arg.indexOf( '=' );
//            if ( index > 0 )
//                qDebug() << arg.mid( index + 1 );   //Set the Server Website?
//        }
//        else if ( arg.startsWith( "/fudge", Qt::CaseInsensitive ) )
//            qDebug() << arg.mid( index + 1 );   //Enable fileLogging.
    }
    ui->serverPort->setText( QString::number( serverInfo->privatePort ) );
    ui->isPublicServer->setChecked( serverInfo->isPublic );
    ui->serverName->setText( serverInfo->name );
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
            {
              synreal.write( socket->readAll() );
            }
            synreal.close();

            QSettings settings( "synReal.ini", QSettings::IniFormat );
            QString str = settings.value( "WoS/master" ).toString();
            int index = str.indexOf( ":" );
            if ( index > 0 )
            {
                serverInfo->masterIP = str.left( index );
                serverInfo->masterPort = str.mid( index + 1 ).toInt();
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
            serverInfo->masterIP = str.left( index );
            serverInfo->masterPort = str.mid( index + 1 ).toInt();
        }
    }
}

void ReMix::on_enableNetworking_clicked()
{
    ui->enableNetworking->setEnabled( false );
    ui->serverPort->setEnabled( false );
    tcpServer->setupServerInfo( serverInfo );
}

void ReMix::on_isPublicServer_stateChanged(int)
{
    if ( ui->isPublicServer->isChecked() )
        tcpServer->setupPublicServer(true);   //Setup a connection with the Master Server.
    else
        tcpServer->setupPublicServer(false);   //Disconnect from the Master Server if applicable.
}

void ReMix::on_openSysMessages_clicked()
{
    if ( sysMessages->isVisible() )
        sysMessages->hide();
    else
        sysMessages->show();
}

void ReMix::on_openBanIP_clicked()
{
    if ( banIP->isVisible() )
        banIP->hide();
    else
        banIP->show();
}

void ReMix::on_openBannedSernums_clicked()
{
    if ( sernumBan->isVisible() )
        sernumBan->hide();
    else
        sernumBan->show();
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

    serverInfo->privatePort = val;
}

void ReMix::on_serverName_textChanged(const QString &arg1)
{
    serverInfo->name = arg1;
}
