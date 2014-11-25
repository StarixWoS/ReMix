
#include "bandialog.hpp"
#include "ui_bandialog.h"

#include "adminhelper.hpp"
#include "helper.hpp"

BanDialog::BanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BanDialog)
{
    ui->setupUi(this);
    this->setWindowModality( Qt::WindowModal );

    //Setup the IP-Ban TableView.
    ipModel = new QStandardItemModel( 0, 3, 0 );
    ipModel->setHeaderData( 0, Qt::Horizontal, "User IP" );
    ipModel->setHeaderData( 1, Qt::Horizontal, "Ban Reason" );
    ipModel->setHeaderData( 2, Qt::Horizontal, "Ban Time" );

    //Proxy model to support sorting without actually altering the underlying model
    ipProxy = new QSortFilterProxyModel();
    ipProxy->setDynamicSortFilter( true );
    ipProxy->setSourceModel( ipModel );
    ipProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->banTable->setModel( ipProxy );

    //Setup the Sernum-Ban TableView.
    snModel = new QStandardItemModel( 0, 4, 0 );
    snModel->setHeaderData( 0, Qt::Horizontal, "Sernum" );
    snModel->setHeaderData( 1, Qt::Horizontal, "Banned Until" );
    snModel->setHeaderData( 2, Qt::Horizontal, "Expires" );
    snModel->setHeaderData( 3, Qt::Horizontal, "Reason" );

    //Proxy model to support sorting without actually altering the underlying model
    snProxy = new QSortFilterProxyModel();
    snProxy->setDynamicSortFilter( true );
    snProxy->setSourceModel( snModel );
    snProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->sernumTable->setModel( snProxy );

    QItemSelectionModel* snSelModel = ui->sernumTable->selectionModel();
    QObject::connect( snSelModel, &QItemSelectionModel::currentChanged,
                      this, &BanDialog::currentRowChangedSlot );

    //Load BannedIP data.
    this->loadBannedIPs();

    //Load Banned SerNum Data.
    this->loadBannedSernums();
}

BanDialog::~BanDialog()
{
    ipModel->deleteLater();
    ipProxy->deleteLater();

    snModel->deleteLater();
    snProxy->deleteLater();

    delete ui;
}

void BanDialog::loadBannedIPs()
{
    Helper::readBinaryFile( ipBans, "mutants.dat" );

    int row = -1;
    for ( int i = 0; i < 400; ++i )
    {
        QString name = QString( ipBans[ i ].reason );
        if ( !name.isEmpty() && ipBans[ i ].banDate > 0 )
        {
            row = ipModel->rowCount();
            ipModel->insertRow( row );

            ipModel->setData( ipModel->index( row, 0 ),
                                 QHostAddress( qFromBigEndian( ipBans[ i ].ip )).toString(),
                                 Qt::DisplayRole );

            ipModel->setData( ipModel->index( row, 1 ), name, Qt::DisplayRole );

            ipModel->setData( ipModel->index( row, 2 ),
                                 QDateTime::fromTime_t( ipBans[ i ].banDate ).toString( "ddd MMM dd HH:mm:ss yyyy" ),
                                 Qt::DisplayRole );
        }
    }
    ui->banTable->selectRow( 0 );
    ui->banTable->resizeColumnsToContents();
}

void BanDialog::on_removeIPBan_clicked()
{
    QString trgIP = ui->banIP->text();
    QString srcIP;

    QModelIndex index;
    for ( int i = 0; i < ipModel->rowCount(); ++i )
    {
        index = ipModel->index( i, 0 );
        srcIP = ipModel->data( index, Qt::DisplayRole ).toString();
        if ( srcIP == trgIP )
            ipModel->removeRow( index.row() );
    }

    unsigned int ip = qToBigEndian( QHostAddress( trgIP ).toIPv4Address() );
    for ( int i = 0; i < 400; ++i )
    {
        if  ( ip == ipBans[ i ].ip )
        {
            ipBans[ i ].banDate = 0;
            ipBans[ i ].ip = 0;
            ipBans[ i ].reason[ 0 ] = {};
        }
    }
    Helper::writeBinaryFile( ipBans, "mutants.dat" );
}

void BanDialog::on_addIPBan_clicked()
{
    int row = -1;
    QString trgIP = ui->banIP->text();
    QString reason = ui->banReason->text();

    unsigned int ip = qToBigEndian( QHostAddress( trgIP ).toIPv4Address() );
    unsigned int date = QDateTime::currentDateTime().toTime_t();
    for ( int i = 0; i < 400; ++i )
    {
        IpBans* ban = &ipBans[ i ];
        if ( ban != nullptr
          && ban->ip == 0 )
        {
            row = ipModel->rowCount();
            ipModel->insertRow( row );

            ipModel->setData( ipModel->index( row, 0 ), trgIP, Qt::DisplayRole );

            ipModel->setData( ipModel->index( row, 1 ), reason, Qt::DisplayRole );

            ipModel->setData( ipModel->index( row, 2 ),
                                 QDateTime::fromTime_t( date ).toString( "ddd MMM dd HH:mm:ss yyyy" ),
                                 Qt::DisplayRole );

            ban->banDate = date;
            ban->ip = ip;

            //Copy the Reason into the Reason char array.
            for ( int i  = 0; i < reason.length() && i < 256; ++ i )
            {
                ban->reason[ i ] = reason.at( i ).toLatin1();
                if ( i == 255 || i == reason.length() - 1 )
                    ban->reason[ i + 1 ] = '\0';
            }
            break;
        }
    }
    Helper::writeBinaryFile( ipBans, "mutants.dat" );
}

//Sernum-Ban Tab
void BanDialog::loadBannedSernums()
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

            row = snModel->rowCount();
            snModel->insertRow( row );

            snModel->setData( snModel->index( row, 0 ), group, Qt::DisplayRole );

            snModel->setData( snModel->index( row, 1 ),
                                 QDateTime::fromTime_t( banTime ).toString( "ddd MMM dd HH:mm:ss yyyy" ),
                                 Qt::DisplayRole );

            snModel->setData( snModel->index( row, 2 ),
                                 QString::number( (banTime - curTime) / 60 / 60 / 24 ) + " days",
                                 Qt::DisplayRole );

            snModel->setData( snModel->index( row, 3 ), banData.value( group + "/banReason", 0 ), Qt::DisplayRole );
        }
        ui->sernumTable->selectRow( 0 );
        ui->sernumTable->resizeColumnsToContents();
    }
}

void BanDialog::on_sernumTable_clicked(const QModelIndex &index)
{
    if ( index.row() >= 0 )
        ui->forgiveButton->setEnabled( true );
}

void BanDialog::currentRowChangedSlot(const QModelIndex &index, const QModelIndex&)
{
    if ( index.row() >= 0 )
        ui->forgiveButton->setEnabled( true );
}

void BanDialog::on_forgiveButton_clicked()
{
    QModelIndex index = snProxy->mapToSource( ui->sernumTable->currentIndex() );
    QSettings banData( "banData.ini", QSettings::IniFormat );

    banData.remove( snModel->data( snModel->index( index.row(), 0 ) ).toString() );
    snModel->removeRow( index.row() );

    ui->forgiveButton->setEnabled( false );
}
