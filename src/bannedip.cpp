#include "bannedip.hpp"
#include "ui_bannedip.h"

#include "helper.hpp"
#include <readmin.hpp>

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

    ui->allowDupedIP->setChecked( Helper::getAllowDupedIP() );
    ui->banDupedIP->setChecked( Helper::getBanDupedIP() );
    ui->banHackers->setChecked( Helper::getBanHackers() );
    ui->reqSernums->setChecked( Helper::getReqSernums() );
    ui->disconnectIdles->setChecked( Helper::getDisconnectIdles() );
    ui->allowSSV->setChecked( Helper::getAllowSSV() );

    //Load BannedIP data.
    this->loadBannedIPs();
}

BannedIP::~BannedIP()
{
    delete ui;
}

void BannedIP::loadBannedIPs()
{
    Helper::readBinaryFile( ipBans, "mutants.dat" );

    int row = -1;
    for ( int i = 0; i < 400; ++i )
    {
        QString name = QString( ipBans[ i ].reason );
        if ( !name.isEmpty() && ipBans[ i ].banDate > 0 )
        {
            row = tableModel->rowCount();
            tableModel->insertRow( row );

            tableModel->setData( tableModel->index( row, 0 ),
                                 QHostAddress( qFromBigEndian( ipBans[ i ].ip )).toString(),
                                 Qt::DisplayRole );

            tableModel->setData( tableModel->index( row, 1 ), name, Qt::DisplayRole );

            tableModel->setData( tableModel->index( row, 2 ),
                                 QDateTime::fromTime_t( ipBans[ i ].banDate ).toString( "ddd MMM dd HH:mm:ss yyyy" ),
                                 Qt::DisplayRole );
        }
    }
    ui->banTable->selectRow( 0 );
    ui->banTable->resizeColumnsToContents();
}

void BannedIP::on_removeIPBan_clicked()
{
    QString trgIP = ui->banIP->text();
    QString srcIP;

    QModelIndex index;
    for ( int i = 0; i < tableModel->rowCount(); ++i )
    {
        index = tableModel->index( i, 0 );
        srcIP = tableModel->data( index, Qt::DisplayRole ).toString();
        if ( srcIP == trgIP )
            tableModel->removeRow( index.row() );
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

void BannedIP::on_addIPBan_clicked()
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
            row = tableModel->rowCount();
            tableModel->insertRow( row );

            tableModel->setData( tableModel->index( row, 0 ), trgIP, Qt::DisplayRole );

            tableModel->setData( tableModel->index( row, 1 ), reason, Qt::DisplayRole );

            tableModel->setData( tableModel->index( row, 2 ),
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

void BannedIP::on_allowDupedIP_clicked()
{
    QVariant value = ui->allowDupedIP->isChecked();
    Helper::setAllowDupedIP( value );
}

void BannedIP::on_banDupedIP_clicked()
{
    QVariant value = ui->banDupedIP->isChecked();
    Helper::setBanDupedIP( value );
}

void BannedIP::on_banHackers_clicked()
{
    QVariant value = ui->banHackers->isChecked();
    Helper::setBanHackers( value );
}

void BannedIP::on_reqSernums_clicked()
{
    QVariant value = ui->reqSernums->isChecked();
    Helper::setReqSernums( value );
}

void BannedIP::on_disconnectIdles_clicked()
{
    QVariant value = ui->disconnectIdles->isChecked();
    Helper::setDisconnectIdles( value );
}

void BannedIP::on_allowSSV_clicked()
{
    QVariant value = ui->allowSSV->isChecked();
    Helper::setAllowSSV( value );
}

void BannedIP::on_reqAdminAuth_clicked()
{
    QVariant value = ui->reqAdminAuth->isChecked();
    Admin::setReqAdminAuth( value );
}
