
#include "includes.hpp"
#include "snbanwidget.hpp"
#include "ui_snbanwidget.h"

SNBanWidget::SNBanWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SNBanWidget)
{
    ui->setupUi(this);

    //Setup the Sernum-Ban TableView.
    snModel = new QStandardItemModel( 0, 3, 0 );
    snModel->setHeaderData( 0, Qt::Horizontal, "Sernum" );
    snModel->setHeaderData( 1, Qt::Horizontal, "Ban Reason" );
    snModel->setHeaderData( 2, Qt::Horizontal, "Ban Time" );

    //Proxy model to support sorting without actually
    //altering the underlying model
    snProxy = new SNSortProxyModel();
    snProxy->setDynamicSortFilter( true );
    snProxy->setSourceModel( snModel );
    snProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->snBanTable->setModel( snProxy );

    QItemSelectionModel* snSelModel = ui->snBanTable->selectionModel();
    QObject::connect( snSelModel, &QItemSelectionModel::currentChanged,
                      this, &SNBanWidget::snBanTableChangedRowSlot );

    //Install Event Filter to enable Row-Deslection.
    ui->snBanTable->viewport()->installEventFilter(
                new TblEventFilter( ui->snBanTable,
                                    snProxy ) );

    //Load Banned SerNum Data.
    this->loadBannedSernums();
}

SNBanWidget::~SNBanWidget()
{
    snModel->deleteLater();
    snProxy->deleteLater();

    delete ui;
}

//Public Members.
bool SNBanWidget::getIsSernumBanned(QString sernum)
{
    if ( !sernum.isEmpty() )
    {
        QSettings banData( "snBanData.ini", QSettings::IniFormat );
        QStringList keys = banData.childGroups();

        return keys.contains( sernum, Qt::CaseInsensitive );
    }
    return false;
}

void SNBanWidget::addSerNumBan(QString& sernum, QString& reason)
{
    QString msg{ reason };
    if ( msg.isEmpty() )
    {
        msg = "Manual-Banish; Unknown reason: [ %1 ]";
        msg = msg.arg( sernum );
    }

    if ( !sernum.isEmpty() )
        this->addSerNumBanImpl( sernum, msg );
}

void SNBanWidget::removeSerNumBan(QString& sernum)
{
    QList<QStandardItem *> list = snModel->findItems( sernum,
                                                      Qt::MatchExactly, 0 );
    if ( list.count() > 1 && list.count() > 0 )
    {
        return; //Too many Bans, do nothing. --Inform the User later?
    }
    else if ( list.count() )
    {
        QModelIndex index = list.value( 0 )->index();
        if ( index.isValid() )
            this->removeSerNumBanImpl( index );
    }
}

//Private Members.
void SNBanWidget::loadBannedSernums()
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

            snModel->setData( snModel->index( row, 0 ),
                              group,
                              Qt::DisplayRole );

            snModel->setData( snModel->index( row, 1 ),
                              banData.value( group % "/banReason", 0 ),
                              Qt::DisplayRole );

            snModel->setData( snModel->index( row, 2 ),
                              QDateTime::fromTime_t( banDate )
                                   .toString( "ddd MMM dd HH:mm:ss yyyy" ),
                              Qt::DisplayRole );
        }
        ui->snBanTable->selectRow( 0 );
    }
}

void SNBanWidget::addSerNumBanImpl(QString& sernum, QString& reason)
{
    QSettings banData( "snBanData.ini", QSettings::IniFormat );
    quint64 date = QDateTime::currentDateTime().toTime_t();

    int row{ -1 };
    if ( !sernum.isEmpty() )
    {
        //Display the SERNUM in the correct format as required.
        if ( sernum.contains( "SOUL", Qt::CaseInsensitive )
          && !sernum.contains( " " ) )
        {
            sernum = "SOUL " % Helper::getStrStr( sernum, "SOUL", "SOUL", "" );
        }
        else if ( !( sernum.toInt( 0, 16 ) & MIN_HEX_SERNUM )
               && !sernum.contains( "SOUL " ) )
        {
            sernum.prepend( "SOUL " );
        }

        //Prevent adding new rows for previously-banned users.
        if ( !this->getIsSernumBanned( sernum ) )
        {
            row = snModel->rowCount();
            snModel->insertRow( row );

            snModel->setData( snModel->index( row, 0 ),
                              sernum,
                              Qt::DisplayRole );

            snModel->setData( snModel->index( row, 2 ),
                              QDateTime::fromTime_t( date )
                                   .toString( "ddd MMM dd HH:mm:ss yyyy" ),
                              Qt::DisplayRole );
        }

        snModel->setData( snModel->index( row, 1 ),
                          reason,
                          Qt::DisplayRole );

        banData.setValue( sernum % "/banDate", date );
        banData.setValue( sernum % "/banReason", reason );

        QString log{ QDate::currentDate()
                      .toString( "banLog/yyyy-MM-dd.txt" ) };
        Helper::logToFile( log, reason, true, true );
    }
    ui->snBanTable->resizeColumnsToContents();
}

void SNBanWidget::removeSerNumBanImpl(QModelIndex& index)
{
    QSettings banData( "snBanData.ini", QSettings::IniFormat );
    if ( index.isValid() )
    {
        banData.remove( snModel->data( snModel->index( index.row(), 0 ) )
                                  .toString() );
        snModel->removeRow( index.row() );
    }
    ui->snBanTable->resizeColumnsToContents();
}

//Public Slot Members..
void SNBanWidget::snBanTableChangedRowSlot(const QModelIndex &index,
                                           const QModelIndex&)
{
    if ( index.row() >= 0 )
        ui->forgiveButton->setEnabled( true );
}

//Private Slot Members..
void SNBanWidget::on_snBanTable_clicked(const QModelIndex &index)
{
    if ( index.row() >= 0 )
        ui->forgiveButton->setEnabled( true );
}

void SNBanWidget::on_forgiveButton_clicked()
{
    QModelIndex index = snProxy->mapToSource( ui->snBanTable->currentIndex() );
    this->removeSerNumBanImpl( index );

    ui->forgiveButton->setEnabled( false );
}

void SNBanWidget::on_addSernumBan_clicked()
{
    QString sernum = ui->trgSerNum->text();
    QString tmp{ ui->snBanReason->text() };

    QString reason{ "Manual Banish; [ %1 ]. %2" };
            reason = reason.arg( sernum );

    if ( !tmp.isEmpty() )
        reason = reason.arg( "Reason: [ " % tmp % " ]." );
    else
        reason = reason.arg( "Unknown Reason." );

    this->addSerNumBan( sernum, reason );

    ui->trgSerNum->clear();
    ui->snBanReason->clear();
}
