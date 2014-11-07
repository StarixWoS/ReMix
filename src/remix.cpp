
#include "remix.hpp"
#include "ui_remix.h"

#include "messages.hpp"
#include "bannedip.hpp"

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
    sysMessages = new Messages( this );
    banIP = new BannedIP( this );
}

ReMix::~ReMix()
{
    sysMessages->close();
    sysMessages->deleteLater();

    banIP->close();
    banIP->deleteLater();

    delete ui;
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
