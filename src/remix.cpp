
#include "remix.hpp"
#include "ui_remix.h"

#include "bannedsernum.hpp"
#include "usermessage.hpp"
#include "messages.hpp"
#include "bannedip.hpp"
#include "server.hpp"

ReMix::ReMix(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReMix)
{
    ui->setupUi(this);

    //Parse Commandline Arguments.
//    QStringList args{ "/mixmaster=23999", "/game=WoS", "/Listen=50001", "/public", "/fudge", "/name=Well of Lost Souls" }; //= qApp->arguments();
//    for ( int i = 0; i < args.count(); ++i )
//    {
//        int index = 0;

//        QString arg = args.at( i );
//        if ( arg.startsWith( "/Game", Qt::CaseInsensitive ) )
//        {
//            index = arg.indexOf( '=' );
//            if ( index > 0 )
//                qDebug() << arg.mid( index + 1 );   //Set the Game name.
//        }
//        else if ( arg.startsWith( "/mixmaster", Qt::CaseInsensitive ) )
//        {
//            index = arg.indexOf( '=' );
//            if ( index > 0 )
//                qDebug() << arg.mid( index + 1 );   //Enable MixMaster Mode and listen to port. Default Port = 23999
//        }
//        else if ( arg.startsWith( "/master", Qt::CaseInsensitive ) )
//        {
//            index = arg.indexOf( '=' );
//            if ( index > 0 )
//                qDebug() << arg.mid( index + 1 );   //Set the Master Server IP:Port. Default = 209.233.24.166:23999
//        }
//        else if ( arg.startsWith( "/public", Qt::CaseInsensitive ) )
//        {
//            index = arg.indexOf( '=' );
//            if ( index > 0 )
//                qDebug() << arg.mid( index + 1 );   //Set the Master Server IP:Port. Default = 209.233.24.166:23999
//        }
//        else if ( arg.startsWith( "/listen", Qt::CaseInsensitive ) )
//        {
//            index = arg.indexOf( '=' );
//            if ( index > 0 )
//                qDebug() << arg.mid( index + 1 );   //Set Server Port. Default = 0
//        }
//        else if ( arg.startsWith( "/url", Qt::CaseInsensitive ) )
//        {
//            index = arg.indexOf( '=' );
//            if ( index > 0 )
//                qDebug() << arg.mid( index + 1 );   //Set the Server Website.
//        }
//        else if ( arg.startsWith( "/name", Qt::CaseInsensitive ) )
//        {
//            index = arg.indexOf( '=' );
//            if ( index > 0 )
//                qDebug() << arg.mid( index + 1 );   //Set the Server name.
//        }
//        else if ( arg.startsWith( "/fudge", Qt::CaseInsensitive ) )
//            qDebug() << arg.mid( index + 1 );   //Enable fileLogging.
//    }

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

    tcpServer->close();
    tcpServer->deleteLater();

    delete ui;
}

void ReMix::on_enableNetworking_clicked()
{
    ui->enableNetworking->setEnabled( false );

    QString port = ui->serverPort->text();
    tcpServer->setupServerInfo( port );

    ui->serverName->setText( tcpServer->serverAddress().toString() );
}

void ReMix::on_isPublicServer_clicked()
{
    if ( ui->isPublicServer->isChecked() )
    {
        ;   //Setup a connection with the Master Server.
    }
    else
    {
        ;   //Disconnect from the Master Server if applicable.
    }
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
