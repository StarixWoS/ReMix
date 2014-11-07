#include "bannedsernum.hpp"
#include "ui_bannedsernum.h"

BannedSernum::BannedSernum(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BannedSernum)
{
    ui->setupUi(this);

    //Setup the ServerInfo TableView.
    tableModel = new QStandardItemModel( 0, 4, 0 );
    tableModel->setHeaderData( 0, Qt::Horizontal, "Sernum" );
    tableModel->setHeaderData( 1, Qt::Horizontal, "Banned Until" );
    tableModel->setHeaderData( 2, Qt::Horizontal, "Expires" );
    tableModel->setHeaderData( 3, Qt::Horizontal, "Reason" );

    //Proxy model to support sorting without actually altering the underlying model
    tableProxy = new QSortFilterProxyModel();
    tableProxy->setDynamicSortFilter( true );
    tableProxy->setSourceModel( tableModel );
    tableProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->sernumTable->setModel( tableProxy );
}

BannedSernum::~BannedSernum()
{
    delete ui;
}
