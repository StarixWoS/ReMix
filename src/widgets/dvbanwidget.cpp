
#include "includes.hpp"
#include "dvbanwidget.hpp"
#include "ui_dvbanwidget.h"

DVBanWidget::DVBanWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DVBanWidget)
{
    ui->setupUi(this);

    //Setup the IP-Ban TableView.
    dvModel = new QStandardItemModel( 0, 3, 0 );
    dvModel->setHeaderData( 0, Qt::Horizontal, "Device ID" );
    dvModel->setHeaderData( 1, Qt::Horizontal, "Ban Reason" );
    dvModel->setHeaderData( 2, Qt::Horizontal, "Ban Time" );

    //Proxy model to support sorting without actually
    //altering the underlying model
    dvProxy = new DVSortProxyModel();
    dvProxy->setDynamicSortFilter( true );
    dvProxy->setSourceModel( dvModel );
    dvProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->dvBanTable->setModel( dvProxy );

    QItemSelectionModel* dvSelModel = ui->dvBanTable->selectionModel();
    QObject::connect( dvSelModel, &QItemSelectionModel::currentChanged,
                      this, &DVBanWidget::dvBanTableChangedRowSlot );

    //Install Event Filter to enable Row-Deslection.
    ui->dvBanTable->viewport()->installEventFilter(
                new TblEventFilter( ui->dvBanTable,
                                    dvProxy ) );

    //Load Banned IP Addresses.
    this->loadBannedDVs();
}

DVBanWidget::~DVBanWidget()
{
    dvModel->deleteLater();
    dvProxy->deleteLater();

    delete ui;
}

//Public Members.
void DVBanWidget::addDVBan(QString ip, QString& reason)
{
    QString msg{ reason };
    if ( msg.isEmpty() )
    {
        msg = "Manual-Banish; Unknown reason: [ %1 ]";
        msg = msg.arg( ip );
    }

    if ( !ip.isEmpty() )
        this->addDVBanImpl( ip, msg );
}

void DVBanWidget::removeDVBan(QString& ip)
{
    QList<QStandardItem *> list = dvModel->findItems( ip, Qt::MatchExactly, 0 );
    if ( list.count() > 1 && list.count() > 0 )
    {
        return; //Too many listed Bans, do nothing. --Inform the User later?
    }
    else if ( list.count() )
    {
        QModelIndex index = list.value( 0 )->index();
        if ( index.isValid() )
            this->removeDVBanImpl( index );
    }
}

bool DVBanWidget::getIsDVBanned(QString ipAddr)
{
    QSettings banData( "banData.ini", QSettings::IniFormat );
    QStringList bans = banData.childGroups();

    return bans.contains( ipAddr );
}

//Private Members.
void DVBanWidget::loadBannedDVs()
{
    if ( QFile( "banData.ini" ).exists() )
    {
        QSettings banData( "banData.ini", QSettings::IniFormat );
        QStringList groups = banData.childGroups();

        QString banType{ "" };
        QString group{ "" };

        int banDate{ 0 };
        int row{ -1 };

        for ( int i = 0; i < groups.count(); ++i )
        {
            group = groups.at( i );
            banType = banData.value( group % "/banType", "" ).toString();
            if ( banType.compare( "dv", Qt::CaseInsensitive ) == 0 )
            {
                banDate = banData.value( group % "/banDate", 0 ).toInt();

                row = dvModel->rowCount();
                dvModel->insertRow( row );

                dvModel->setData( dvModel->index( row, 0 ),
                                  group,
                                  Qt::DisplayRole );

                dvModel->setData( dvModel->index( row, 1 ),
                                  banData.value( group % "/banReason", 0 ),
                                  Qt::DisplayRole );

                dvModel->setData( dvModel->index( row, 2 ),
                                  QDateTime::fromTime_t( banDate )
                                  .toString( "ddd MMM dd HH:mm:ss yyyy" ),
                                  Qt::DisplayRole );
            }
        }
        ui->dvBanTable->resizeColumnsToContents();
    }
}

void DVBanWidget::addDVBanImpl(QString& ip, QString& reason)
{
    QSettings banData( "banData.ini", QSettings::IniFormat );
    quint64 date = QDateTime::currentDateTime().toTime_t();

    int row{ -1 };

    if ( !ip.isEmpty() )
    {
        //Prevent adding new rows for previously-banned users.
        if ( !this->getIsDVBanned( ip ) )
        {
            row = dvModel->rowCount();
            dvModel->insertRow( row );

            dvModel->setData( dvModel->index( row, 0 ),
                              ip,
                              Qt::DisplayRole );

            dvModel->setData( dvModel->index( row, 2 ),
                              QDateTime::fromTime_t( date )
                                   .toString( "ddd MMM dd HH:mm:ss yyyy" ),
                              Qt::DisplayRole );
        }

        dvModel->setData( dvModel->index( row, 1 ),
                          reason,
                          Qt::DisplayRole );

        banData.setValue( ip % "/banType", "dv" );
        banData.setValue( ip % "/banDate", date );
        banData.setValue( ip % "/banReason", reason );

        QString log{ QDate::currentDate()
                      .toString( "banLog/yyyy-MM-dd.txt" ) };
        Helper::logToFile( log, reason, true, true );
    }
}

void DVBanWidget::removeDVBanImpl(QModelIndex& index)
{
    QSettings banData( "banData.ini", QSettings::IniFormat );
    if ( index.isValid() )
    {
        banData.remove( dvModel->data( dvModel->index( index.row(), 0 ) )
                                  .toString() );
        dvModel->removeRow( index.row() );
    }
    ui->dvBanTable->resizeColumnsToContents();
}

//Public Slots.
void DVBanWidget::dvBanTableChangedRowSlot(const QModelIndex &index,
                                           const QModelIndex&)
{
    if ( index.isValid() )
        ui->removeDVBan->setEnabled( true );
    else
        ui->removeDVBan->setEnabled( false );
}

//Private Slots.
void DVBanWidget::on_dvBanTable_clicked(const QModelIndex &index)
{
    if ( index.isValid() )
        ui->removeDVBan->setEnabled( true );
    else
        ui->removeDVBan->setEnabled( false );
}

void DVBanWidget::on_addDeviceBan_clicked()
{
    QString tmp{ ui->dvBanReason->text() };
    QString dv = ui->trgDevice->text();

    QString reason{ "Manual Banish; [ %1 ]. %2" };
            reason = reason.arg( dv );

    if ( !tmp.isEmpty() )
        reason = reason.arg( "Reason: [ " % tmp % " ]." );
    else
        reason = reason.arg( "Unknown Reason." );

    this->addDVBan( dv, reason );

    ui->trgDevice->clear();
    ui->dvBanReason->clear();
}

void DVBanWidget::on_removeDVBan_clicked()
{
    QModelIndex index = dvProxy->mapToSource( ui->dvBanTable->currentIndex() );
    this->removeDVBanImpl( index );

    ui->removeDVBan->setEnabled( false );
}
