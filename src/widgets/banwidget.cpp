
#include "includes.hpp"
#include "banwidget.hpp"
#include "ui_banwidget.h"

const QString BanWidget::banKeys[ BAN_KEY_COUNT ] =
{
    "BanCount",
    "IP",
    "SN",
    "DV",
    "WV",
    "Reason",
    "Date"
};

BanWidget::BanWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BanWidget)
{
    ui->setupUi(this);

    //Setup the IP-Ban TableView.
    tblModel = new QStandardItemModel( 0, 6, 0 );
    tblModel->setHeaderData( 0, Qt::Horizontal, "IP Address" );
    tblModel->setHeaderData( 1, Qt::Horizontal, "SerNum" );
    tblModel->setHeaderData( 2, Qt::Horizontal, "Device" );
    tblModel->setHeaderData( 3, Qt::Horizontal, "Date" );
    tblModel->setHeaderData( 4, Qt::Horizontal, "Reason" );
    tblModel->setHeaderData( 5, Qt::Horizontal, "Ban Date" );

    tblProxy = new BanSortProxyModel();
    tblProxy->setDynamicSortFilter( true );
    tblProxy->setSourceModel( tblModel );
    tblProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->banTable->setModel( tblProxy );

    QItemSelectionModel* dvSelModel = ui->banTable->selectionModel();
    QObject::connect( dvSelModel, &QItemSelectionModel::currentChanged,
                      this, &BanWidget::banTableChangedRowSlot );

    //Install Event Filter to enable Row-Deslection.
    ui->banTable->viewport()->installEventFilter(
                new TblEventFilter( ui->banTable, tblProxy ) );

    //Load Banned IP Addresses.
    this->loadBans();
}

BanWidget::~BanWidget()
{
    delete ui;
}

void BanWidget::loadBans()
{
    tblModel->removeRows( 0, tblModel->rowCount() );
    tblProxy->removeRows( 0, tblProxy->rowCount() );
    if ( QFile( "banData.ini" ).exists() )
    {
        QSettings bans( "banData.ini", QSettings::IniFormat );
        QStringList banIDs = bans.childGroups();

        quint32 banDate{ 0 };
        int row{ -1 };

        QString banID{ "" };
        for ( int i = 0; i < banIDs.count(); ++i )
        {
            banID = banIDs.at( i ) % "/";

            row = tblModel->rowCount();
            tblModel->insertRow( row );

            tblModel->setData( tblModel->index( row, Rows::ip ),
                               bans.value( banID % banKeys[ Types::IP ],
                               Qt::DisplayRole ));

            tblModel->setData( tblModel->index( row, Rows::sn ),
                               Helper::serNumToIntStr(
                                   bans.value( banID % banKeys[ Types::SN ] )
                                       .toString() ),
                               Qt::DisplayRole );

            tblModel->setData( tblModel->index( row, Rows::dv ),
                               bans.value( banID % banKeys[ Types::DV ],
                               Qt::DisplayRole ));

            tblModel->setData( tblModel->index( row, Rows::wv ),
                               bans.value( banID % banKeys[ Types::WV ],
                               Qt::DisplayRole ));

            tblModel->setData( tblModel->index( row, Rows::reason ),
                               bans.value( banID % banKeys[ Types::Reason ],
                               Qt::DisplayRole ));

            banDate = bans.value( banID % banKeys[ Types::Date ], 0 )
                          .toUInt();
            tblModel->setData( tblModel->index( row, Rows::date ),
                               QDateTime::fromTime_t( banDate )
                                    .toString( "ddd MMM dd HH:mm:ss yyyy" ),
                               Qt::DisplayRole );
        }
    }
}

void BanWidget::addBan(Player* plr, QString& reason)
{
    if ( plr == nullptr )
        return;

    QString ip{ plr->getPublicIP() };
    QString sn{ plr->getSernumHex_s() };
    QString dv{ plr->getDVar() };
    QString wv{ plr->getWVar() };

    this->addBanImpl( ip, sn, dv, wv, reason );
}

void BanWidget::addBanImpl(QString& ip, QString& sn, QString& dv, QString& wv,
                           QString& reason)
{
    QSettings bans( "banData.ini", QSettings::IniFormat );
    qint32 count = bans.value( banKeys[ Types::COUNT ], 0 ).toUInt();

    qint32 banID_i{ -1 };
    if ( !ip.isEmpty() )
        banID_i = this->getIsBanned( ip );

    if ( banID_i < 0 )
    {
        if ( !sn.isEmpty() )
            banID_i = this->getIsBanned( sn );

        if ( banID_i < 0 )
        {
            if ( !dv.isEmpty() )
                banID_i = this->getIsBanned( dv );

            if ( banID_i < 0 )
            {
                if ( !wv.isEmpty() )
                    banID_i = this->getIsBanned( wv );
            }
        }
    }

    if ( banID_i < 0 )
    {
        banID_i = count + 1;
        bans.setValue( banKeys[ Types::COUNT ], banID_i );
    }

    QString banID{ QString::number( banID_i ) };
    quint64 date{ QDateTime::currentDateTime().toTime_t() };
    if ( !ip.isEmpty() || !sn.isEmpty()
      || !dv.isEmpty() || !wv.isEmpty() )
    {
        bans.setValue( banID % "/" % banKeys[ Types::IP ], ip );
        bans.setValue( banID % "/" % banKeys[ Types::SN ], sn );
        bans.setValue( banID % "/" % banKeys[ Types::DV ], dv );
        bans.setValue( banID % "/" % banKeys[ Types::WV ], wv );
        bans.setValue( banID % "/" % banKeys[ Types::Reason ], reason );
        bans.setValue( banID % "/" % banKeys[ Types::Date ], date );

        this->loadBans();
    }
}

void BanWidget::removeBan(QString& value, qint32 type)
{
    QList<QStandardItem *> list = tblModel->findItems( value,
                                                       Qt::MatchExactly,
                                                       type );
    if ( list.count() > 1 && list.count() > 0 )
    {
        return; //Too many listed Bans, do nothing. --Inform the User later?
    }
    else if ( list.count() )
    {
        QModelIndex index = list.value( 0 )->index();
        if ( index.isValid() )
            this->removeBanImpl( index, value );
    }
}

void BanWidget::removeBanImpl(QModelIndex& index, QString& value)
{
    QSettings banData( "banData.ini", QSettings::IniFormat );
    QStringList bans = banData.allKeys();

    QString data{ "" };
    QString key{ "" };
    for ( int i = 0; i < bans.count(); ++i )
    {
        key = bans.at( i );
        data = banData.value( key , "" ).toString();
        if ( data.compare( value ) == 0 )
        {
            banData.remove( key.left( key.indexOf( "/" ) ) );
            break;
        }
    }
    tblModel->removeRow( index.row() );
}

qint32 BanWidget::getIsBanned(QString value)
{
    QSettings banData( "banData.ini", QSettings::IniFormat );
    QStringList bans = banData.allKeys();

    QString data{ "" };
    QString key{ "" };
    qint32 banned{ -1 };

    for ( int i = 0; i < bans.count(); ++i )
    {
        key = bans.at( i );
        data = banData.value( key , "" ).toString();
        if ( data.compare( value ) == 0 )
        {
            banned = key.left( key.indexOf( "/" ) ).toInt();
            break;
        }
    }
    return banned;
}

void BanWidget::banTableChangedRowSlot(const QModelIndex& index,
                                       const QModelIndex&)
{
    if ( index.isValid() )
        ui->removeBan->setEnabled( true );
    else
        ui->removeBan->setEnabled( false );
}

void BanWidget::on_addBan_clicked()
{
    QString ip{ ui->trgIP->text() };
    QString sn{ ui->trgSN->text() };
            sn = Helper::serNumToHexStr( sn );
    QString dv{ ui->trgDV->text() };
    QString wv{ ui->trgWV->text() };

    QString reason{ ui->reason->text() };

    this->addBanImpl( ip, sn, dv, wv, reason );
}

void BanWidget::on_removeBan_clicked()
{
    QModelIndex index = tblProxy->mapToSource( ui->banTable->currentIndex() );

    if ( index.isValid() )
    {
        QString tmp{ tblModel->item( index.row(), ip )->text() };
        if ( tmp.isEmpty() )
        {
            tmp = tblModel->item( index.row(), sn )->text();
            if ( tmp.isEmpty() )
            {
                tmp = tblModel->item( index.row(), dv )->text();
                if ( tmp.isEmpty() )
                    tmp = tblModel->item( index.row(), wv )->text();
            }
        }

        if ( !tmp.isEmpty() )
            this->removeBanImpl( index, tmp );
    }
}
