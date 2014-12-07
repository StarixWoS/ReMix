
#include "bandialog.hpp"
#include "ui_bandialog.h"

#include "adminhelper.hpp"
#include "helper.hpp"

BanDialog::BanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BanDialog)
{
    ui->setupUi(this);

    //Remove the "Help" button from the window title bars.
    {
        QIcon icon = this->windowIcon();
        Qt::WindowFlags flags = this->windowFlags();
        flags &= ~Qt::WindowContextHelpButtonHint;

        this->setWindowFlags( flags );
        this->setWindowIcon( icon );
        this->setWindowModality( Qt::WindowModal );
    }

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
    ui->ipBanTable->setModel( ipProxy );

    //Setup the Sernum-Ban TableView.
    snModel = new QStandardItemModel( 0, 3, 0 );
    snModel->setHeaderData( 0, Qt::Horizontal, "Sernum" );
    snModel->setHeaderData( 1, Qt::Horizontal, "Ban Reason" );
    snModel->setHeaderData( 2, Qt::Horizontal, "Ban Time" );

    //Proxy model to support sorting without actually altering the underlying model
    snProxy = new QSortFilterProxyModel();
    snProxy->setDynamicSortFilter( true );
    snProxy->setSourceModel( snModel );
    snProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->snBanTable->setModel( snProxy );

    QItemSelectionModel* ipSelModel = ui->snBanTable->selectionModel();
    QObject::connect( ipSelModel, &QItemSelectionModel::currentChanged,
                      this, &BanDialog::ipBanTableChangedRowSlot );

    QItemSelectionModel* snSelModel = ui->snBanTable->selectionModel();
    QObject::connect( snSelModel, &QItemSelectionModel::currentChanged,
                      this, &BanDialog::snBanTableChangedRowSlot );

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

        int banDate{ 0 };
        int row{ -1 };

        for ( int i = 0; i < groups.count(); ++i )
        {
            group = groups.at( i );
            banDate = banData.value( group % "/banDate", 0 ).toInt();

            row = ipModel->rowCount();
            ipModel->insertRow( row );

            ipModel->setData( ipModel->index( row, 0 ), group, Qt::DisplayRole );

            ipModel->setData( ipModel->index( row, 1 ), banData.value( group % "/banReason", 0 ), Qt::DisplayRole );

            ipModel->setData( ipModel->index( row, 2 ),
                                 QDateTime::fromTime_t( banDate ).toString( "ddd MMM dd HH:mm:ss yyyy" ),
                                 Qt::DisplayRole );
        }
        ui->ipBanTable->selectRow( 0 );
        ui->ipBanTable->resizeColumnsToContents();
    }
}

bool BanDialog::getIsIPBanned(QHostAddress& ipAddr)
{
    QString ip{ ipAddr.toString() };
    return getIsIPBanned( ip );
}

bool BanDialog::getIsIPBanned(QString ipAddr)
{
    QSettings banData( "ipBanData.ini", QSettings::IniFormat );
    QStringList bans = banData.childGroups();

    return bans.contains( ipAddr );
}

void BanDialog::on_addIPBan_clicked()
{
    QString ip = ui->trgIPAddr->text();
    QString reason{ "Manual Banish; %1" };
            reason = reason.arg( ui->ipBanReason->text() );

    this->addIPBan( ip, reason );

    ui->trgIPAddr->setText( "127.0.0.1" );
    ui->ipBanReason->clear();
}

void BanDialog::addIPBan(QHostAddress& ipInfo, QString& reason)
{
    QString ip{ ipInfo.toString() };
    this->addIPBan( ip, reason );
}

void BanDialog::addIPBan(QString ip, QString& reason)
{
    if ( reason.isEmpty() )
    {
        reason = "Manual Banish; Unknown reason: [ %1 ]";
        reason = reason.arg( ip );
    }

    if ( !ip.isEmpty() )
    {
        this->addIPBanImpl( ip, reason );

        QString log{ QDate::currentDate().toString( "banLog/yyyy-MM-dd.txt" ) };
        Helper::logToFile( log, reason, true, true );
    }
}

void BanDialog::addIPBanImpl(QString& ip, QString& reason)
{
    QSettings banData( "ipBanData.ini", QSettings::IniFormat );
    quint64 date = QDateTime::currentDateTime().toTime_t();

    int row{ -1 };

    if ( !ip.isEmpty() )
    {
        //Prevent adding new rows for previously-banned users.
        if ( !this->getIsIPBanned( ip ) )
        {
            row = ipModel->rowCount();
            ipModel->insertRow( row );

            ipModel->setData( ipModel->index( row, 0 ), ip, Qt::DisplayRole );

            ipModel->setData( ipModel->index( row, 1 ), reason, Qt::DisplayRole );

            ipModel->setData( ipModel->index( row, 2 ),
                              QDateTime::fromTime_t( date ).toString( "ddd MMM dd HH:mm:ss yyyy" ),
                              Qt::DisplayRole );
        }
        banData.setValue( ip % "/banDate", date );
        banData.setValue( ip % "/banReason", reason );
    }
    ui->ipBanTable->resizeColumnsToContents();
}

void BanDialog::on_ipBanTable_clicked(const QModelIndex &index)
{
    if ( index.row() >= 0 )
        ui->removeIPBan->setEnabled( true );
}

void BanDialog::ipBanTableChangedRowSlot(const QModelIndex &index, const QModelIndex&)
{
    if ( index.row() >= 0 )
        ui->removeIPBan->setEnabled( true );
}

void BanDialog::on_removeIPBan_clicked()
{
    QModelIndex index = ipProxy->mapToSource( ui->ipBanTable->currentIndex() );
    QSettings banData( "ipBanData.ini", QSettings::IniFormat );

    banData.remove( ipModel->data( ipModel->index( index.row(), 0 ) ).toString() );
    ipModel->removeRow( index.row() );

    ui->removeIPBan->setEnabled( false );
}

//Sernum-Ban Tab
void BanDialog::loadBannedSernums()
{
    if ( QFile( "snBanData.ini" ).exists() )
    {
        QSettings banData( "snBanData.ini", QSettings::IniFormat );
        QStringList groups = banData.childGroups();

        QString group;

        int banDate{ 0 };
        int row{ -1 };

        for ( int i = 0; i < groups.count(); ++i )
        {
            group = groups.at( i );
            banDate = banData.value( group % "/banDate", 0 ).toInt();

            row = snModel->rowCount();
            snModel->insertRow( row );

            snModel->setData( snModel->index( row, 0 ), group, Qt::DisplayRole );

            snModel->setData( snModel->index( row, 1 ), banData.value( group % "/banReason", 0 ), Qt::DisplayRole );

            snModel->setData( snModel->index( row, 2 ),
                                 QDateTime::fromTime_t( banDate ).toString( "ddd MMM dd HH:mm:ss yyyy" ),
                                 Qt::DisplayRole );
        }
        ui->snBanTable->selectRow( 0 );
        ui->snBanTable->resizeColumnsToContents();
    }
}

bool BanDialog::getIsSernumBanned(QString sernum)
{
    if ( !sernum.isEmpty() )
    {
        QSettings banData( "snBanData.ini", QSettings::IniFormat );
        QStringList keys = banData.childGroups();

        return keys.contains( sernum, Qt::CaseInsensitive );
    }
    return false;
}

void BanDialog::on_addSernumBan_clicked()
{
    QString sernum = ui->trgSerNum->text();
    QString reason{ "Manual Banish; %1" };
            reason = reason.arg( ui->snBanReason->text() );

    this->addSerNumBan( sernum, reason );

    ui->trgSerNum->clear();
    ui->snBanReason->clear();
}

void BanDialog::addSerNumBan(QString& sernum, QString& reason)
{
    if ( reason.isEmpty() )
    {
        reason = "Manual Banish; Unknown reason: [ %1 ]";
        reason = reason.arg( sernum );
    }

    if ( !sernum.isEmpty() )
    {
        this->addSernumBanImpl( sernum, reason );

        QString log{ QDate::currentDate().toString( "banLog/yyyy-MM-dd.txt" ) };
        Helper::logToFile( log, reason, true, true );
    }
}

void BanDialog::addSernumBanImpl(QString& sernum, QString& reason)
{
    QSettings banData( "snBanData.ini", QSettings::IniFormat );
    quint64 date = QDateTime::currentDateTime().toTime_t();

    int row{ -1 };
    if ( !sernum.isEmpty() )
    {
        //Prevent adding new rows for previously-banned users.
        if ( !this->getIsSernumBanned( sernum ) )
        {
            row = snModel->rowCount();
            snModel->insertRow( row );

            snModel->setData( snModel->index( row, 0 ), sernum, Qt::DisplayRole );

            snModel->setData( snModel->index( row, 1 ), reason, Qt::DisplayRole );

            snModel->setData( snModel->index( row, 2 ),
                              QDateTime::fromTime_t( date ).toString( "ddd MMM dd HH:mm:ss yyyy" ),
                              Qt::DisplayRole );
        }
        banData.setValue( sernum % "/banDate", date );
        banData.setValue( sernum % "/banReason", reason );
    }
    ui->snBanTable->resizeColumnsToContents();
}

void BanDialog::on_snBanTable_clicked(const QModelIndex &index)
{
    if ( index.row() >= 0 )
        ui->forgiveButton->setEnabled( true );
}

void BanDialog::snBanTableChangedRowSlot(const QModelIndex &index, const QModelIndex&)
{
    if ( index.row() >= 0 )
        ui->forgiveButton->setEnabled( true );
}

void BanDialog::on_forgiveButton_clicked()
{
    QModelIndex index = snProxy->mapToSource( ui->snBanTable->currentIndex() );
    QSettings banData( "snBanData.ini", QSettings::IniFormat );

    banData.remove( snModel->data( snModel->index( index.row(), 0 ) ).toString() );
    snModel->removeRow( index.row() );

    ui->forgiveButton->setEnabled( false );
}
