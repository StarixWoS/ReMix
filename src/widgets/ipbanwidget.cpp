
#include "includes.hpp"
#include "ipbanwidget.hpp"
#include "ui_ipbanwidget.h"

IPBanWidget::IPBanWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IPBanWidget)
{
    ui->setupUi(this);

    //Setup the IP-Ban TableView.
    ipModel = new QStandardItemModel( 0, 3, 0 );
    ipModel->setHeaderData( 0, Qt::Horizontal, "IP Address" );
    ipModel->setHeaderData( 1, Qt::Horizontal, "Ban Reason" );
    ipModel->setHeaderData( 2, Qt::Horizontal, "Ban Time" );

    //Proxy model to support sorting without actually
    //altering the underlying model
    ipProxy = new IPSortProxyModel();
    ipProxy->setDynamicSortFilter( true );
    ipProxy->setSourceModel( ipModel );
    ipProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->ipBanTable->setModel( ipProxy );

    QItemSelectionModel* ipSelModel = ui->ipBanTable->selectionModel();
    QObject::connect( ipSelModel, &QItemSelectionModel::currentChanged,
                      this, &IPBanWidget::ipBanTableChangedRowSlot );

    //Load Banned IP Addresses.
    this->loadBannedIPs();

    //Install Event Filter to enable Row-Deslection.
    ui->ipBanTable->viewport()->installEventFilter(
                new TblEventFilter( ui->ipBanTable,
                                    ipProxy ) );
}

IPBanWidget::~IPBanWidget()
{
    ipModel->deleteLater();
    ipProxy->deleteLater();

    delete ui;
}

//Public Members.
void IPBanWidget::addIPBan(QString ip, QString& reason)
{
    QString msg{ reason };
    if ( msg.isEmpty() )
    {
        msg = "Manual-Banish; Unknown reason: [ %1 ]";
        msg = msg.arg( ip );
    }

    if ( !ip.isEmpty() )
        this->addIPBanImpl( ip, msg );
}

void IPBanWidget::removeIPBan(QString ip)
{
    QList<QStandardItem *> list = ipModel->findItems( ip, Qt::MatchExactly, 0 );
    if ( list.count() > 1 && list.count() > 0 )
    {
        return; //Too many listed Bans, do nothing. --Inform the User later?
    }
    else if ( list.count() )
    {
        QModelIndex index = list.value( 0 )->index();
        if ( index.isValid() )
            this->removeIPBanImpl( index );
    }
}

bool IPBanWidget::getIsIPBanned(QString ipAddr)
{
    QSettings banData( "banData.ini", QSettings::IniFormat );
    QStringList bans = banData.childGroups();

    return bans.contains( ipAddr );
}

//Private Members.
void IPBanWidget::loadBannedIPs()
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
            if ( banType.compare( "ip", Qt::CaseInsensitive ) == 0 )
            {
                banDate = banData.value( group % "/banDate", 0 ).toInt();

                row = ipModel->rowCount();
                ipModel->insertRow( row );

                ipModel->setData( ipModel->index( row, 0 ),
                                  group,
                                  Qt::DisplayRole );

                ipModel->setData( ipModel->index( row, 1 ),
                                  banData.value( group % "/banReason", 0 ),
                                  Qt::DisplayRole );

                ipModel->setData( ipModel->index( row, 2 ),
                                  QDateTime::fromTime_t( banDate )
                                  .toString( "ddd MMM dd HH:mm:ss yyyy" ),
                                  Qt::DisplayRole );
            }
        }
        ui->ipBanTable->resizeColumnsToContents();
    }
}

void IPBanWidget::addIPBanImpl(QString& ip, QString& reason)
{
    QSettings banData( "banData.ini", QSettings::IniFormat );
    quint64 date = QDateTime::currentDateTime().toTime_t();

    int row{ -1 };

    if ( !ip.isEmpty() )
    {
        //Prevent adding new rows for previously-banned users.
        if ( !this->getIsIPBanned( ip ) )
        {
            row = ipModel->rowCount();
            ipModel->insertRow( row );

            ipModel->setData( ipModel->index( row, 0 ),
                              ip,
                              Qt::DisplayRole );

            ipModel->setData( ipModel->index( row, 2 ),
                              QDateTime::fromTime_t( date )
                                   .toString( "ddd MMM dd HH:mm:ss yyyy" ),
                              Qt::DisplayRole );
        }

        ipModel->setData( ipModel->index( row, 1 ),
                          reason,
                          Qt::DisplayRole );

        banData.setValue( ip % "/banType", "ip" );
        banData.setValue( ip % "/banDate", date );
        banData.setValue( ip % "/banReason", reason );

        QString log{ QDate::currentDate()
                      .toString( "banLog/yyyy-MM-dd.txt" ) };
        Helper::logToFile( log, reason, true, true );
    }
}

void IPBanWidget::removeIPBanImpl(QModelIndex& index)
{
    QSettings banData( "banData.ini", QSettings::IniFormat );
    if ( index.isValid() )
    {
        banData.remove( ipModel->data( ipModel->index( index.row(), 0 ) )
                                  .toString() );
        ipModel->removeRow( index.row() );
    }
    ui->ipBanTable->resizeColumnsToContents();
}

//Public Slots.
void IPBanWidget::ipBanTableChangedRowSlot(const QModelIndex &index,
                                           const QModelIndex&)
{
    if ( index.isValid() )
        ui->removeIPBan->setEnabled( true );
    else
        ui->removeIPBan->setEnabled( false );
}

//Private Slots.
void IPBanWidget::on_ipBanTable_clicked(const QModelIndex &index)
{
    if ( index.isValid() )
        ui->removeIPBan->setEnabled( true );
    else
        ui->removeIPBan->setEnabled( false );
}

void IPBanWidget::on_addIPBan_clicked()
{
    QString tmp{ ui->ipBanReason->text() };
    QString ip = ui->trgIPAddr->text();

    QString reason{ "Manual Banish; [ %1 ]. %2" };
            reason = reason.arg( ui->trgIPAddr->text() );

    if ( !tmp.isEmpty() )
        reason = reason.arg( "Reason: [ " % tmp % " ]." );
    else
        reason = reason.arg( "Unknown Reason." );

    this->addIPBan( ip, reason );

    ui->trgIPAddr->setText( "127.0.0.1" );
    ui->ipBanReason->clear();
}

void IPBanWidget::on_removeIPBan_clicked()
{
    QModelIndex index = ipProxy->mapToSource( ui->ipBanTable->currentIndex() );
    this->removeIPBanImpl( index );

    ui->removeIPBan->setEnabled( false );
}
