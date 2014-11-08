#include "bannedip.hpp"
#include "ui_bannedip.h"

BannedIP::BannedIP(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BannedIP)
{
    ui->setupUi(this);

    //Setup the ServerInfo TableView.
    tableModel = new QStandardItemModel( 0, 3, 0 );
    tableModel->setHeaderData( 0, Qt::Horizontal, "User IP" );
    tableModel->setHeaderData( 1, Qt::Horizontal, "Ban Reason" );
    tableModel->setHeaderData( 2, Qt::Horizontal, "Ban Time" );

    //Proxy model to support sorting without actually altering the underlying model
    tableProxy = new QSortFilterProxyModel();
    tableProxy->setDynamicSortFilter( true );
    tableProxy->setSourceModel( tableModel );
    tableProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->banTable->setModel( tableProxy );

    this->setWindowModality( Qt::WindowModal );
}

BannedIP::~BannedIP()
{
    delete ui;
}
