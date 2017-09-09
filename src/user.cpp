
#include "user.hpp"
#include "ui_user.h"

#include "widgets/userdelegate.hpp"

const QString User::keys[ USER_KEY_COUNT ] =
{
    "seen", "bio", "ip", "dv", "wv",
    "rank", "hash", "salt", "reason",
    "banned", "pings", "calls"
};

//Initialize our QSettings Object globally to make things more responsive.
QSettings* User::userData{ new QSettings( "userInfo.ini",
                                          QSettings::IniFormat ) };

User::User(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::User)
{
    ui->setupUi(this);

    if ( Settings::getSaveWindowPositions() )
    {
        this->restoreGeometry( Settings::getWindowPositions(
                                   this->metaObject()->className() ) );
    }

    //Setup our Random Device
    randDev = new RandDev();

    //Setup the ServerInfo TableView.
    tblModel = new QStandardItemModel( 0, UserColumns::cCOLS, 0 );
    tblModel->setHeaderData( UserColumns::cSERNUM,
                             Qt::Horizontal,
                             "SerNum" );
    tblModel->setHeaderData( UserColumns::cPINGS,
                             Qt::Horizontal,
                             "Pings" );
    tblModel->setHeaderData( UserColumns::cCALLS,
                             Qt::Horizontal,
                             "Calls" );
    tblModel->setHeaderData( UserColumns::cSEENDATE,
                             Qt::Horizontal,
                             "Last Seen" );
    tblModel->setHeaderData( UserColumns::cIP,
                             Qt::Horizontal,
                             "Last IP" );
    tblModel->setHeaderData( UserColumns::cRANK,
                             Qt::Horizontal,
                             "Rank" );
    tblModel->setHeaderData( UserColumns::cBANNED,
                             Qt::Horizontal,
                             "Banned" );
    tblModel->setHeaderData( UserColumns::cBANDATE,
                             Qt::Horizontal,
                             "Ban Date" );
    tblModel->setHeaderData( UserColumns::cREASON,
                             Qt::Horizontal,
                             "Ban Reason" );

    //Proxy model to support sorting without actually
    //altering the underlying model
    tblProxy = new UserSortProxyModel();
    tblProxy->setDynamicSortFilter( true );
    tblProxy->setSourceModel( tblModel );
    tblProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->userTable->setModel( tblProxy );

    ui->userTable->setItemDelegate( new UserDelegate( this ) );

    //Load Information.
    this->loadUserInfo();

    QObject::connect( tblModel, &QAbstractItemModel::dataChanged,
                      this, &User::updateDataValue );
}

User::~User()
{
    if ( Settings::getSaveWindowPositions() )
    {
        Settings::setWindowPositions( this->saveGeometry(),
                                      this->metaObject()->className() );
    }

    userData->deleteLater();
    delete ui;
}

//Public Functions
void User::setData(const QString& key, const QString& subKey,
                   const QVariant& value)
{
    userData->setValue( key % "/" % subKey, value );
}

QVariant User::getData(const QString& key, const QString& subKey)
{
    if ( subKey == keys[ UserKeys::kRANK ] )
        return userData->value( key % "/" % subKey, 0 );

    return userData->value( key % "/" % subKey );
}

bool User::makeAdmin(QString& sernum, QString& pwd)
{
    qint32 rank{ getAdminRank( sernum ) };
    if ( !sernum.isEmpty()
      && !pwd.isEmpty() )
    {
        QString salt = Helper::genPwdSalt( randDev, SALT_LENGTH );
        QString hash( salt + pwd );
                hash = Helper::hashPassword( hash );

        if ( rank == PlayerRanks::rUSER )
        {
            setData( sernum, keys[ UserKeys::kRANK ], PlayerRanks::rGAMEMASTER );
            QModelIndex index = this->findModelIndex(
                                    Helper::serNumToIntStr( sernum ),
                                    UserColumns::cSERNUM );
            if ( index.isValid() )
            {
                this->updateRowData( index.row(),
                                     UserColumns::cRANK,
                                     QVariant( PlayerRanks::rGAMEMASTER ) );
            }
        }

        setData( sernum, keys[ UserKeys::kHASH ], hash );
        setData( sernum, keys[ UserKeys::kSALT ], salt );
        return true;
    }
    return false;
}

bool User::getIsAdmin(QString& sernum)
{
    return getData( sernum, keys[ UserKeys::kRANK ] )
              .toInt() >= 1;
}

bool User::getHasPassword(QString sernum)
{
    QString pwd{ getData( sernum, keys[ UserKeys::kHASH ] )
                    .toString() };

    return pwd.length() > 0;
}

bool User::cmpAdminPwd(QString& sernum, QString& value)
{
    QString recSalt = getData( sernum, keys[ UserKeys::kSALT ] )
                         .toString();
    QString recHash = getData( sernum, keys[ UserKeys::kHASH ] )
                         .toString();

    QString hash{ recSalt + value };
            hash = Helper::hashPassword( hash );

    return hash == recHash;
}

qint32 User::getAdminRank(QString& sernum)
{
    return getData( sernum, keys[ UserKeys::kRANK ] )
              .toUInt();
}

void User::setAdminRank(QString& sernum, qint32 rank)
{
    QVariant value{ rank };
    setData( sernum, keys[ UserKeys::kRANK ], value );

    QModelIndex index = this->findModelIndex( sernum,
                                              UserColumns::cSERNUM );
    if ( index.isValid() )
    {
        this->updateRowData( index.row(),
                             UserColumns::cRANK,
                             value );
    }
}

void User::removeBan(QString& value, qint32 type)
{
    QList<QStandardItem *> list = tblModel->findItems( value, Qt::MatchExactly,
                                                       type );
    if ( list.count() > 1 && list.count() > 0 )
    {
        return; //Too many listed Bans, do nothing. --Inform the User later?
    }
    else if ( list.count() )
    {
        QModelIndex index = list.value( 0 )->index();
        if ( index.isValid() )
        {
            QStringList sernums = userData->allKeys();
            QString sernum{ "" };

            for ( int i = 0; i < sernums.count(); ++i )
            {
                sernum = sernums.at( i );
                if ( sernum.compare( value ) == 0 )
                {
                    userData->remove( sernum % "/" % keys[ UserKeys::kBANNED ] );
                    userData->remove( sernum % "/" % keys[ UserKeys::kREASON ] );
                    break;
                }
            }
            this->updateRowData( index.row(),
                                 UserColumns::cBANNED,
                                 QVariant( false ) );

            this->updateRowData( index.row(),
                                 UserColumns::cREASON,
                                 QVariant( "" ) );

            this->updateRowData( index.row(),
                                 UserColumns::cBANDATE,
                                 QVariant( 0 ) );
        }
    }
}

bool User::addBan(Player* admin, Player* target, QString& reason, bool remote)
{
    if ( target == nullptr )
        return false;

    QString msg{ reason };
    if ( msg.isEmpty() )
    {
        if ( remote )
        {
            if ( admin == nullptr )
                return false;

            msg = "Remote-Banish by [ %1 ]; Unknown Reason: [ %2 ]";
            msg = msg.arg( admin->getSernum_s() )
                     .arg( target->getSernum_s() );
        }
        else
        {
            msg = "Manual-Banish; Unknown reason: [ %1 ]";
            msg = msg.arg( target->getSernum_s() );
        }
    }

    quint64 date{ QDateTime::currentDateTime().toTime_t() };
    setData( target->getSernumHex_s(), keys[ UserKeys::kBANNED ], date );
    setData( target->getSernumHex_s(), keys[ UserKeys::kREASON ], msg );

    QModelIndex index = this->findModelIndex( target->getSernumHex_s(),
                                              UserColumns::cSERNUM );
    if ( index.isValid() )
    {
        this->updateRowData( index.row(),
                             UserColumns::cBANNED,
                             QVariant( date > 0 ) );

        this->updateRowData( index.row(),
                             UserColumns::cREASON,
                             QVariant( msg ) );

        this->updateRowData( index.row(),
                             UserColumns::cBANDATE,
                             QVariant( date ) );
    }
    return true;
}

bool User::getIsBanned(QString value, BanTypes type)
{
    if ( value.isEmpty() )
        return false;

    QString sernum{ "" };
    QString var{ "" };

    bool isValue{ false };
    bool banned{ false };

    QStringList sernums = userData->childGroups();
    for ( int i = 0; i < sernums.count(); ++i )
    {
        sernum = sernums.at( i );
        switch ( type )
        {
            case BanTypes::tSERNUM:
            {
                if ( sernum.compare( value, Qt::CaseInsensitive ) == 0 )
                    isValue = true;
            }
            break;
            case BanTypes::tIP:
            {
                var = getData( sernum, keys[ UserKeys::kIP ] ).toString();
                if ( var.compare( value, Qt::CaseInsensitive ) == 0 )
                    isValue = true;
            }
            break;
            case BanTypes::tDV:
            {
                var = getData( sernum, keys[ UserKeys::kDV ] ).toString();
                if ( var.compare( value, Qt::CaseInsensitive ) == 0 )
                    isValue = true;
            }
            break;
            case BanTypes::tWV:
            {
                var = getData( sernum, keys[ UserKeys::kWV ] ).toString();
                if ( var.compare( value, Qt::CaseInsensitive ) == 0 )
                    isValue = true;
            }
            break;
            default:
            {
                isValue = false;
                banned = false;
            }
            break;
        }

        if ( isValue )
            break;
        else
            continue;
    }

    if ( isValue )
        banned = getData( sernum, keys[ UserKeys::kBANNED ] ).toUInt() > 0;

    return banned;
}

void User::updateCallCount(QString serNum)
{
    quint32 callCount{ getData( serNum, keys[ UserKeys::kCALLS ] )
                          .toUInt() + 1 };
    setData( serNum, keys[ UserKeys::kCALLS ], callCount );

    QModelIndex index = this->findModelIndex( serNum,
                                              UserColumns::cSERNUM );
    if ( index.isValid() )
    {
        this->updateRowData( index.row(),
                             UserColumns::cCALLS,
                             callCount );
    }
}

void User::logBIO(QString& serNum, QHostAddress& ip, QString& dv,
                  QString& wv, QString& bio)
{
    QString sernum{ serNum };
    if ( sernum.contains( "SOUL", Qt::CaseInsensitive ) )
        sernum = Helper::serNumToHexStr( serNum, 8 );

    setData( sernum, keys[ UserKeys::kBIO ], bio.mid( 1 ) );

    QString ip_s{ ip.toString() };
    setData( sernum, keys[ UserKeys::kIP ], ip_s );
    setData( sernum, keys[ UserKeys::kDV ], dv );
    setData( sernum, keys[ UserKeys::kWV ], wv );

    quint32 pings{ getData( sernum, keys[ UserKeys::kPINGS ] )
                      .toUInt() + 1 };
    setData( sernum, keys[ UserKeys::kPINGS ], pings );

    quint64 date{ QDateTime::currentDateTime().toTime_t() };
    setData( sernum, keys[ UserKeys::kSEEN ], date );

    QModelIndex index = this->findModelIndex( sernum, UserColumns::cSERNUM );
    if ( !index.isValid() )
    {
        quint32 row = tblModel->rowCount();
        tblModel->insertRow( row );
        index = tblModel->index( row, UserColumns::cSERNUM );
    }

    if ( index.isValid() )
    {
        this->updateRowData( index.row(),
                             UserColumns::cSERNUM,
                             QVariant( Helper::serNumToIntStr( sernum ) ) );

        this->updateRowData( index.row(),
                             UserColumns::cIP,
                             QVariant( ip_s ) );

        this->updateRowData( index.row(),
                             UserColumns::cPINGS,
                             QVariant( pings ) );

        this->updateRowData( index.row(),
                             UserColumns::cSEENDATE,
                             QVariant( date ) );
    }
}

//Private Functions.
QModelIndex User::findModelIndex(QString value, UserColumns col)
{
    if ( col == UserColumns::cSERNUM )
        value = Helper::serNumToIntStr( value );

    QList<QStandardItem*> list = tblModel->findItems( value,
                                                      Qt::MatchExactly,
                                                      col );
    QModelIndex index;
    if ( list.count() )
    {
        index = list.value( 0 )->index();
        if ( index.isValid() )
            return index;
    }
    return QModelIndex();
}

void User::loadUserInfo()
{
    tblModel->removeRows( 0, tblModel->rowCount() );
    tblProxy->removeRows( 0, tblProxy->rowCount() );

    QString title{ "User Information:" };
    if ( QFile( "userInfo.ini" ).exists() )
    {
        QStringList sernums = userData->childGroups();

        title = title.append( " [ %1 ] Users" )
                     .arg( sernums.count() );

        QString sernum{ "" };
        QString reason{ "" };
        QString ip{ "" };

        quint64 banDate_i{ 0 };
        quint64 seen_i{ 0 };
        quint32 pings_i{ 0 };
        quint32 calls_i{ 0 };

        quint32 rank{ 0 };
        bool banned{ false };

        int row{ -1 };
        for ( int i = 0; i < sernums.count(); ++i )
        {
            sernum = sernums.at( i );

            reason = getData( sernum, keys[ UserKeys::kREASON ] )
                        .toString();
            banDate_i = getData( sernum, keys[ UserKeys::kBANNED ] )
                           .toUInt();
            pings_i = getData( sernum, keys[ UserKeys::kPINGS ] )
                         .toUInt();
            calls_i = getData( sernum, keys[ UserKeys::kCALLS ] )
                         .toUInt();
            seen_i = getData( sernum, keys[ UserKeys::kSEEN ] )
                        .toUInt();
            rank = getData( sernum, keys[ UserKeys::kRANK ] )
                      .toUInt();
            ip = getData( sernum, keys[ UserKeys::kIP ] )
                    .toString();

            banned = banDate_i > 0;

            row = tblModel->rowCount();
            tblModel->insertRow( row );

            tblProxy->rowCount();
            tblModel->setData( tblModel->index( row, 0 ),
                               Helper::serNumToIntStr( sernum ),
                               Qt::DisplayRole );
            this->updateRowData( row,
                                 UserColumns::cPINGS,
                                 QVariant( pings_i ) );

            this->updateRowData( row,
                                 UserColumns::cCALLS,
                                 QVariant( calls_i ) );

            this->updateRowData( row,
                                 UserColumns::cSEENDATE,
                                 QVariant( seen_i ) );

            this->updateRowData( row,
                                 UserColumns::cIP,
                                 QVariant( ip ) );

            this->updateRowData( row,
                                 UserColumns::cRANK,
                                 QVariant( rank ) );

            this->updateRowData( row,
                                 UserColumns::cBANNED,
                                 QVariant( banned ) );

            this->updateRowData( row,
                                 UserColumns::cREASON,
                                 QVariant( reason ) );

            this->updateRowData( row,
                                 UserColumns::cBANDATE,
                                 QVariant( banDate_i ) );
        }
    }

    this->setWindowTitle( title );
    ui->userTable->selectRow( 0 );
    ui->userTable->resizeColumnsToContents();
}

void User::updateRowData(quint32 row, quint32 col, QVariant data)
{
    if ( col == UserColumns::cSEENDATE
      || col == UserColumns::cBANDATE )
    {
        quint64 date{ data.toUInt() };
        if ( date > 0 )
        {
            data = QDateTime::fromTime_t( date )
                        .toString( "ddd MMM dd HH:mm:ss yyyy" );
        }
        else
        {
            if ( col == UserColumns::cSEENDATE )
                data = "Never Seen";
            else
                data = "";
        }
    }

    QModelIndex index = tblModel->index( row, col );
    if ( index.isValid() )
    {
        tblModel->setData( index, data, Qt::DisplayRole );
        if ( col == UserColumns::cBANDATE )
            ui->userTable->resizeColumnToContents( UserColumns::cBANDATE );
    }
}

//Private Slots.
void User::updateDataValue(const QModelIndex& index, const QModelIndex&,
                           const QVector<int>&)
{
    QString sernum{ "" };
    QVariant value;

    sernum = tblModel->data(
                 tblModel->index(
                     index.row(), UserColumns::cSERNUM ) ).toString();
    sernum = Helper::sanitizeSerNum( sernum );

    switch ( index.column() )
    {
        case UserColumns::cRANK:
            {
                value = tblModel->data( index );
                setData( sernum, keys[ UserKeys::kRANK ], value );
            }
        break;
        case UserColumns::cBANNED:
            {
                bool setReason{ false };
                QString reason{ "" };
                QString date{ "" };

                bool banned = tblModel->data( index ).toBool();
                if ( banned )
                {
                    value = QDateTime::currentDateTime().toTime_t();
                    date = QDateTime::fromTime_t( value.toUInt() )
                                .toString( "ddd MMM dd HH:mm:ss yyyy" );

                    reason = getData( sernum, keys[ UserKeys::kREASON ] )
                                .toString();
                    if ( reason.isEmpty() )
                    {
                        setReason = true;
                        reason = "Manual Banish; "
                                 % Helper::getBanishReason( this );
                    }
                }
                else
                {
                    setReason = true;
                    reason.clear();
                    value = "";
                }

                setData( sernum, keys[ UserKeys::kBANNED ], value );
                this->updateRowData( index.row(),
                                     UserColumns::cBANDATE,
                                     value );

                if ( setReason )
                {
                    value = reason;
                    setData( sernum, keys[ UserKeys::kREASON ], value );

                    this->updateRowData( index.row(),
                                         UserColumns::cREASON,
                                         value );
                }
            }
        break;
        default:
        break;
    }
}
