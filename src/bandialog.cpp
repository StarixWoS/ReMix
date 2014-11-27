
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
    ipModel->setHeaderData( 0, Qt::Horizontal, "IP Address" );
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
    if ( QFile( "ipBanData.ini" ).exists() )
    {
        QSettings banData( "ipBanData.ini", QSettings::IniFormat );
        QStringList groups = banData.childGroups();

        QString group;

        int banTime = 0;

        int row = -1;
        for ( int i = 0; i < groups.count(); ++i )
        {
            group = groups.at( i );
            banTime = banData.value( group + "/banUntil", 0 ).toInt();

            row = ipModel->rowCount();
            ipModel->insertRow( row );

            ipModel->setData( ipModel->index( row, 0 ), group, Qt::DisplayRole );

            ipModel->setData( ipModel->index( row, 1 ), banData.value( group + "/banReason", 0 ), Qt::DisplayRole );

            ipModel->setData( ipModel->index( row, 2 ),
                                 QDateTime::fromTime_t( banTime ).toString( "ddd MMM dd HH:mm:ss yyyy" ),
                                 Qt::DisplayRole );
        }
        ui->banTable->selectRow( 0 );
        ui->banTable->resizeColumnsToContents();
    }
}

void BanDialog::on_removeIPBan_clicked()
{
    QModelIndex index = ipProxy->mapToSource( ui->banTable->currentIndex() );
    QSettings banData( "ipBanData.ini", QSettings::IniFormat );

    banData.remove( ipModel->data( ipModel->index( index.row(), 0 ) ).toString() );
    ipModel->removeRow( index.row() );
}

void BanDialog::on_addIPBan_clicked()
{
    QString trgIP = ui->banIP->text();
    QString reason = ui->banReason->text();
    this->addIPBanImpl( trgIP, reason );
}

void BanDialog::addIPBanImpl(QString& ip, QString& reason)
{
    QSettings banData( "ipBanData.ini", QSettings::IniFormat );
    quint64 date = QDateTime::currentDateTime().toTime_t();

    int row = -1;
    if ( !ip.isEmpty() )
    {
        row = ipModel->rowCount();
        ipModel->insertRow( row );

        ipModel->setData( ipModel->index( row, 0 ), ip, Qt::DisplayRole );

        ipModel->setData( ipModel->index( row, 1 ), reason, Qt::DisplayRole );

        ipModel->setData( ipModel->index( row, 2 ),
                          QDateTime::fromTime_t( date ).toString( "ddd MMM dd HH:mm:ss yyyy" ),
                          Qt::DisplayRole );

        banData.setValue( ip + "/banUntil", date );
        banData.setValue( ip + "/banReason", reason );
    }
}

void BanDialog::addSernumBanImpl(QString& sernum, QString& reason)
{

}

//Sernum-Ban Tab
void BanDialog::loadBannedSernums()
{
    if ( QFile( "snBanData.ini" ).exists() )
    {
        QSettings banData( "snBanData.ini", QSettings::IniFormat );
        QStringList groups = banData.childGroups();

        QString group;

        int curTime = 0;
        int banTime = 0;

        int row = -1;
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

bool BanDialog::getIsIPBanned(QHostAddress& ipAddr)
{
    QSettings banData( "ipBanData.ini", QSettings::IniFormat );
    QStringList bans = banData.childGroups();

    return bans.contains( ipAddr.toString() );
}

bool BanDialog::getisSernumBanned(QString& sernum)
{
    if ( !sernum.isEmpty() )
    {
        QSettings banData( "snBanData.ini", QSettings::IniFormat );
        QStringList keys = banData.childGroups();

        return keys.contains( sernum, Qt::CaseInsensitive );
    }
    return false;
}

void BanDialog::addIPBan(QHostAddress& ipInfo, QString& reason)
{
    qDebug() << ipInfo;
    if ( !ipInfo.isNull() )
    {
        QString trgIP = ipInfo.toString();
        this->addIPBanImpl( trgIP, reason );
    }
}

void BanDialog::addSerNumBan(QString& sernum, QString& reason)
{
    qDebug() << sernum << reason;
//    if ( !sernum.isEmpty() )
//    {
//        if ( !reason.isEmpty() )
//            ;
//        else
//            ;
//    }
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
    QSettings banData( "snBanData.ini", QSettings::IniFormat );

    banData.remove( snModel->data( snModel->index( index.row(), 0 ) ).toString() );
    snModel->removeRow( index.row() );

    ui->forgiveButton->setEnabled( false );
}
