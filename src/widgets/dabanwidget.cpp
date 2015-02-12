
#include "includes.hpp"
#include "dabanwidget.hpp"
#include "ui_dabanwidget.h"

DABanWidget::DABanWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DABanWidget)
{
    ui->setupUi(this);

    //Setup the IP-Ban TableView.
    daModel = new QStandardItemModel( 0, 3, 0 );
    daModel->setHeaderData( 0, Qt::Horizontal, "Device ID" );
    daModel->setHeaderData( 1, Qt::Horizontal, "Ban Reason" );
    daModel->setHeaderData( 2, Qt::Horizontal, "Ban Time" );

    //Proxy model to support sorting without actually
    //altering the underlying model
    daProxy = new DVSortProxyModel();
    daProxy->setDynamicSortFilter( true );
    daProxy->setSourceModel( daModel );
    daProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->daBanTable->setModel( daProxy );

    QItemSelectionModel* daSelModel = ui->daBanTable->selectionModel();
    QObject::connect( daSelModel, &QItemSelectionModel::currentChanged,
                      this, &DABanWidget::daBanTableChangedRowSlot );

    //Install Event Filter to enable Row-Deslection.
    ui->daBanTable->viewport()->installEventFilter(
                new TblEventFilter( ui->daBanTable,
                                    daProxy ) );

    //Load Banned IP Addresses.
    this->loadBannedDAs();
}

DABanWidget::~DABanWidget()
{
    daModel->deleteLater();
    daProxy->deleteLater();

    delete ui;
}

//Public Members.
void DABanWidget::addDABan(QString ip, QString& reason)
{
    QString msg{ reason };
    if ( msg.isEmpty() )
    {
        msg = "Manual-Banish; Unknown reason: [ %1 ]";
        msg = msg.arg( ip );
    }

    if ( !ip.isEmpty() )
        this->addDABanImpl( ip, msg );
}

void DABanWidget::removeDABan(QString& ip)
{
    QList<QStandardItem *> list = daModel->findItems( ip, Qt::MatchExactly, 0 );
    if ( list.count() > 1 && list.count() > 0 )
    {
        return; //Too many listed Bans, do nothing. --Inform the User later?
    }
    else if ( list.count() )
    {
        QModelIndex index = list.value( 0 )->index();
        if ( index.isValid() )
            this->removeDABanImpl( index );
    }
}

bool DABanWidget::getIsDABanned(QString ipAddr)
{
    QSettings banData( "banData.ini", QSettings::IniFormat );
    QStringList bans = banData.childGroups();

    return bans.contains( ipAddr );
}

//Private Members.
void DABanWidget::loadBannedDAs()
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
            if ( banType.compare( "da", Qt::CaseInsensitive ) == 0 )
            {
                banDate = banData.value( group % "/banDate", 0 ).toInt();

                row = daModel->rowCount();
                daModel->insertRow( row );

                daModel->setData( daModel->index( row, 0 ),
                                  group,
                                  Qt::DisplayRole );

                daModel->setData( daModel->index( row, 1 ),
                                  banData.value( group % "/banReason", 0 ),
                                  Qt::DisplayRole );

                daModel->setData( daModel->index( row, 2 ),
                                  QDateTime::fromTime_t( banDate )
                                  .toString( "ddd MMM dd HH:mm:ss yyyy" ),
                                  Qt::DisplayRole );
            }
        }
        ui->daBanTable->selectRow( 0 );
        ui->daBanTable->resizeColumnsToContents();
    }
}

void DABanWidget::addDABanImpl(QString& ip, QString& reason)
{
    QSettings banData( "banData.ini", QSettings::IniFormat );
    quint64 date = QDateTime::currentDateTime().toTime_t();

    int row{ -1 };

    if ( !ip.isEmpty() )
    {
        //Prevent adding new rows for previously-banned users.
        if ( !this->getIsDABanned( ip ) )
        {
            row = daModel->rowCount();
            daModel->insertRow( row );

            daModel->setData( daModel->index( row, 0 ),
                              ip,
                              Qt::DisplayRole );

            daModel->setData( daModel->index( row, 2 ),
                              QDateTime::fromTime_t( date )
                                   .toString( "ddd MMM dd HH:mm:ss yyyy" ),
                              Qt::DisplayRole );
        }

        daModel->setData( daModel->index( row, 1 ),
                          reason,
                          Qt::DisplayRole );

        banData.setValue( ip % "/banType", "da" );
        banData.setValue( ip % "/banDate", date );
        banData.setValue( ip % "/banReason", reason );

        QString log{ QDate::currentDate()
                      .toString( "banLog/yyyy-MM-dd.txt" ) };
        Helper::logToFile( log, reason, true, true );
    }
}

void DABanWidget::removeDABanImpl(QModelIndex& index)
{
    QSettings banData( "banData.ini", QSettings::IniFormat );
    if ( index.isValid() )
    {
        banData.remove( daModel->data( daModel->index( index.row(), 0 ) )
                                  .toString() );
        daModel->removeRow( index.row() );
    }
    ui->daBanTable->resizeColumnsToContents();
}

//Public Slots.
void DABanWidget::daBanTableChangedRowSlot(const QModelIndex &index,
                                           const QModelIndex&)
{
    if ( index.isValid() )
        ui->removeDABan->setEnabled( true );
    else
        ui->removeDABan->setEnabled( false );
}

//Private Slots.
void DABanWidget::on_addDateBan_clicked()
{
    QString tmp{ ui->daBanReason->text() };
    QString da = ui->trgDate->text();

    QString reason{ "Manual Banish; [ %1 ]. %2" };
            reason = reason.arg( da );

    if ( !tmp.isEmpty() )
        reason = reason.arg( "Reason: [ " % tmp % " ]." );
    else
        reason = reason.arg( "Unknown Reason." );

    this->addDABan( da, reason );

    ui->trgDate->clear();
    ui->daBanReason->clear();
}

void DABanWidget::on_removeDABan_clicked()
{
    QModelIndex index = daProxy->mapToSource( ui->daBanTable->currentIndex() );
    this->removeDABanImpl( index );

    ui->removeDABan->setEnabled( false );
}

void DABanWidget::on_daBanTable_clicked(const QModelIndex &index)
{
    if ( index.isValid() )
        ui->removeDABan->setEnabled( true );
    else
        ui->removeDABan->setEnabled( false );
}
