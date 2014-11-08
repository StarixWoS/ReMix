
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

    QItemSelectionModel *selModel = ui->sernumTable->selectionModel();
    QObject::connect( selModel, &QItemSelectionModel::currentChanged,
                      this, &BannedSernum::currentRowChangedSlot );

    this->setWindowModality( Qt::WindowModal );

    //Load Banned SerNum Data.
    this->loadBannedSernums();
}

BannedSernum::~BannedSernum()
{
    delete ui;
}

void BannedSernum::loadBannedSernums()
{
    if ( QFile( "banData.ini" ).exists() )
    {
        QSettings banData( "banData.ini", QSettings::IniFormat );
        QStringList groups = banData.childGroups();

        QString group;
        int row = -1;

        int curTime = 0;
        int banTime = 0;

        for ( int i = 0; i < groups.count(); ++i )
        {
            group = groups.at( i );
            curTime = QDateTime::currentDateTime().toTime_t();
            banTime = banData.value( group + "/banUntil", 0 ).toInt();

            row = tableModel->rowCount();
            tableModel->insertRow( row );

            tableModel->setData( tableModel->index( row, 0 ), group, Qt::DisplayRole );

            tableModel->setData( tableModel->index( row, 1 ),
                                 QDateTime::fromTime_t( banTime ).toString( "ddd MMM dd HH:mm:ss yyyy" ),
                                 Qt::DisplayRole );

            tableModel->setData( tableModel->index( row, 2 ),
                                 QString::number( (banTime - curTime) / 60 / 60 / 24 ) + " days",
                                 Qt::DisplayRole );

            tableModel->setData( tableModel->index( row, 3 ), banData.value( group + "/banReason", 0 ), Qt::DisplayRole );
        }
        ui->sernumTable->selectRow( 0 );
        ui->sernumTable->resizeColumnsToContents();
    }
}

void BannedSernum::on_sernumTable_clicked(const QModelIndex &index)
{
    if ( index.row() >= 0 )
        ui->forgiveButton->setEnabled( true );
}

void BannedSernum::currentRowChangedSlot(const QModelIndex &index, const QModelIndex&)
{
    if ( index.row() >= 0 )
        ui->forgiveButton->setEnabled( true );
}

void BannedSernum::on_forgiveButton_clicked()
{
    QModelIndex index = tableProxy->mapToSource( ui->sernumTable->currentIndex() );
    QSettings banData( "banData.ini", QSettings::IniFormat );

    banData.remove( tableModel->data( tableModel->index( index.row(), 0 ) ).toString() );
    tableModel->removeRow( index.row() );

    ui->forgiveButton->setEnabled( false );
}

void BannedSernum::on_cancelButton_clicked()
{
    this->close();
}
