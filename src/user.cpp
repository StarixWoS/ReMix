
#include "user.hpp"
#include "ui_user.h"

#include "widgets/userdelegate.hpp"

const QString User::keys[ USER_KEY_COUNT ] =
{
    "seen", "bio", "ip", "dv", "wv",
    "rank", "hash", "salt", "reason",
    "banned"
};

//Initialize our QSettings Object globally to make things more responsive.
QSettings* User::userData{ new QSettings( "userInfo.ini",
                                          QSettings::IniFormat ) };

User::User(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::User)
{
    ui->setupUi(this);

    {
        QIcon icon = this->windowIcon();
        Qt::WindowFlags flags = this->windowFlags();
        flags &= ~Qt::WindowContextHelpButtonHint;

        this->setWindowFlags( flags );
        this->setWindowIcon( icon );
    }

    if ( Settings::getSaveWindowPositions() )
    {
        this->restoreGeometry( Settings::getWindowPositions(
                                   this->metaObject()->className() ) );
    }

    //Setup our Random Device
    randDev = new RandDev();

    //Setup the ServerInfo TableView.
    tblModel = new QStandardItemModel( 0, Cols::cCOLS, 0 );
    tblModel->setHeaderData( Cols::cSERNUM, Qt::Horizontal, "SerNum" );
    tblModel->setHeaderData( Cols::cSEEN, Qt::Horizontal, "Last Seen" );
    tblModel->setHeaderData( Cols::cIP, Qt::Horizontal, "Last IP" );
    tblModel->setHeaderData( Cols::cRANK, Qt::Horizontal, "Rank" );
    tblModel->setHeaderData( Cols::cBANNED, Qt::Horizontal, "Banned" );
    tblModel->setHeaderData( Cols::cREASON, Qt::Horizontal, "Ban Reason" );
    tblModel->setHeaderData( Cols::cDATE, Qt::Horizontal, "Ban Date" );

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
                   QVariant& value)
{
    userData->setValue( key % "/" % subKey, value );
}

QVariant User::getData(const QString& key, const QString& subKey)
{
    if ( subKey == keys[ Keys::kRANK ] )
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

        if ( rank == Ranks::rUSER )
        {
            userData->setValue( sernum % "/rank", Ranks::rGAMEMASTER );
            QModelIndex index = this->findModelIndex(
                                    Helper::serNumToIntStr( sernum ),
                                    Cols::cSERNUM );
            if ( index.isValid() )
            {
                this->updateRowData( index.row(),
                                     Cols::cRANK,
                                     QVariant( Ranks::rGAMEMASTER ) );
            }
        }
        userData->setValue( sernum % "/hash", hash );
        userData->setValue( sernum % "/salt", salt );

        return true;
    }
    return false;
}

bool User::getIsAdmin(QString& sernum)
{
    return getData( sernum, keys[ Keys::kRANK ] )
              .toInt() >= 1;
}

bool User::getHasPassword(QString sernum)
{
    QString pwd{ getData( sernum, keys[ Keys::kHASH ] )
                    .toString() };

    return pwd.length() > 0;
}

bool User::cmpAdminPwd(QString& sernum, QString& value)
{
    QString recSalt = getData( sernum, keys[ Keys::kSALT ] )
                         .toString();
    QString recHash = getData( sernum, keys[ Keys::kHASH ] )
                         .toString();

    QString hash{ recSalt + value };
            hash = Helper::hashPassword( hash );

    return hash == recHash;
}

qint32 User::getAdminRank(QString& sernum)
{
    return getData( sernum, keys[ Keys::kRANK ] )
              .toUInt();
}

void User::setAdminRank(QString& sernum, qint32 rank)
{
    QVariant value{ rank };
    setData( sernum, keys[ Keys::kRANK ], value );

    QModelIndex index = this->findModelIndex( sernum,
                                              Cols::cSERNUM );
    if ( index.isValid() )
    {
        this->updateRowData( index.row(),
                             Cols::cRANK,
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
                    userData->remove( sernum % "/" % keys[ Keys::kBANNED ] );
                    userData->remove( sernum % "/" % keys[ Keys::kREASON ] );
                    break;
                }
            }
            this->updateRowData( index.row(),
                                 Cols::cBANNED,
                                 QVariant( false ) );

            this->updateRowData( index.row(),
                                 Cols::cREASON,
                                 QVariant( "" ) );

            this->updateRowData( index.row(),
                                 Cols::cDATE,
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
    userData->setValue( target->getSernumHex_s() % "/" % keys[ Keys::kBANNED ],
                        date );

    userData->setValue( target->getSernumHex_s() % "/" % keys[ Keys::kREASON ],
                        msg );

    QModelIndex index = this->findModelIndex( target->getSernumHex_s(),
                                              Cols::cSERNUM );
    if ( index.isValid() )
    {
        this->updateRowData( index.row(),
                             Cols::cBANNED,
                             QVariant( date > 0 ) );

        this->updateRowData( index.row(),
                             Cols::cREASON,
                             QVariant( msg ) );

        this->updateRowData( index.row(),
                             Cols::cDATE,
                             QVariant( date ) );
    }
    return true;
}

bool User::getIsBanned(QString value, Types type)
{
    if ( value.isEmpty() )
        return false;

    QFuture<bool> future = QtConcurrent::run( [&]()
    {
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
                case Types::tSERNUM:
                    {
                        if ( sernum.compare( value, Qt::CaseInsensitive ) == 0 )
                            isValue = true;
                        else
                            continue;
                    }
                break;
                case Types::tIP:
                    {
                        var = getData( sernum, keys[ Keys::kIP ] ).toString();
                        if ( var.compare( value, Qt::CaseInsensitive ) == 0 )
                            isValue = true;
                        else
                            continue;
                    }
                break;
                case Types::tDV:
                    {
                        var = getData( sernum, keys[ Keys::kDV ] ).toString();
                        if ( var.compare( value, Qt::CaseInsensitive ) == 0 )
                            isValue = true;
                        else
                            continue;
                    }
                break;
                case Types::tWV:
                    {
                        var = getData( sernum, keys[ Keys::kWV ] ).toString();
                        if ( var.compare( value, Qt::CaseInsensitive ) == 0 )
                            isValue = true;
                        else
                            continue;
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
        }

        if ( isValue )
            banned = getData( sernum, keys[ Keys::kBANNED ] ).toUInt() > 0;

        return banned;
    });

    future.waitForFinished();
    return future.result();
}

void User::logBIO(QString& serNum, QHostAddress& ip, QString& dv,
                  QString& wv, QString& bio)
{
    QString sernum{ serNum };
    if ( sernum.contains( "SOUL", Qt::CaseInsensitive ) )
        sernum = Helper::serNumToHexStr( serNum, 8 );

    userData->setValue( sernum % "/bio", bio.mid( 1 ) );

    QString ip_s{ ip.toString() };
    userData->setValue( sernum % "/ip", ip_s );
    userData->setValue( sernum % "/dv", dv );
    userData->setValue( sernum % "/wv", wv );

    quint64 date{ QDateTime::currentDateTime().toTime_t() };
    userData->setValue( sernum % "/seen", date );

    QModelIndex index = this->findModelIndex( sernum, Cols::cSERNUM );
    if ( !index.isValid() )
    {
        quint32 row = tblModel->rowCount();
        tblModel->insertRow( row );
        index = tblModel->index( row, Cols::cSERNUM );
    }

    if ( index.isValid() )
    {
        this->updateRowData( index.row(),
                             Cols::cSERNUM,
                             QVariant( Helper::serNumToIntStr( sernum ) ) );

        this->updateRowData( index.row(),
                             Cols::cIP,
                             QVariant( ip_s ) );

        this->updateRowData( index.row(),
                             Cols::cSEEN,
                             QVariant( date ) );
    }
}

//Private Functions.
QModelIndex User::findModelIndex(QString value, Cols col)
{
    if ( col == Cols::cSERNUM )
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
    if ( QFile( "userInfo.ini" ).exists() )
    {
        QStringList sernums = userData->childGroups();

        QString sernum{ "" };
        QString reason{ "" };
        QString ip{ "" };

        quint64 banDate_i{ 0 };
        quint64 seen_i{ 0 };

        quint32 rank{ 0 };
        bool banned{ false };

        int row{ -1 };
        for ( int i = 0; i < sernums.count(); ++i )
        {
            sernum = sernums.at( i );

            reason = userData->value( sernum % "/reason", "" ).toString();
            banDate_i = userData->value( sernum % "/banned", 0 ).toUInt();
            seen_i = userData->value( sernum % "/seen", 0 ).toUInt();
            rank = userData->value( sernum % "/rank", 0 ).toUInt();
            ip = userData->value( sernum % "/ip", "" ).toString();

            banned = banDate_i > 0;

            row = tblModel->rowCount();
            tblModel->insertRow( row );

            tblProxy->rowCount();
            tblModel->setData( tblModel->index( row, 0 ),
                               Helper::serNumToIntStr( sernum ),
                               Qt::DisplayRole );

            this->updateRowData( row,
                                 Cols::cSEEN,
                                 QVariant( seen_i ) );

            this->updateRowData( row,
                                 Cols::cIP,
                                 QVariant( ip ) );

            this->updateRowData( row,
                                 Cols::cRANK,
                                 QVariant( rank ) );

            this->updateRowData( row,
                                 Cols::cBANNED,
                                 QVariant( banned ) );

            this->updateRowData( row,
                                 Cols::cREASON,
                                 QVariant( reason ) );

            this->updateRowData( row,
                                 Cols::cDATE,
                                 QVariant( banDate_i ) );
        }
    }

    ui->userTable->selectRow( 0 );
    ui->userTable->resizeColumnsToContents();
}

void User::updateRowData(quint32 row, quint32 col, QVariant data)
{
    if ( col == Cols::cSEEN
      || col == Cols::cDATE )
    {
        quint64 date{ data.toUInt() };
        if ( date > 0 )
        {
            data = QDateTime::fromTime_t( date )
                        .toString( "ddd MMM dd HH:mm:ss yyyy" );
        }
        else
        {
            if ( col == Cols::cSEEN )
                data = "Never Seen";
            else
                data = "";
        }
    }

    QModelIndex index = tblModel->index( row, col );
    if ( index.isValid() )
        tblModel->setData( index, data, Qt::DisplayRole );
}

//Private Slots.
void User::updateDataValue(const QModelIndex& index, const QModelIndex&,
                           const QVector<int>&)
{
    QString sernum{ "" };
    QVariant value;

    sernum = tblModel->data(
                 tblModel->index(
                     index.row(), Cols::cSERNUM ) ).toString();
    sernum = Helper::sanitizeSerNum( sernum );

    switch ( index.column() )
    {
        case Cols::cRANK:
            {
                value = tblModel->data( index );
                setData( sernum, keys[ Keys::kRANK ], value );
            }
        break;
        case Cols::cBANNED:
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

                    reason = getData( sernum, keys[ Keys::kREASON ] )
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

                setData( sernum, keys[ Keys::kBANNED ], value );
                this->updateRowData( index.row(),
                                     Cols::cDATE,
                                     value );

                if ( setReason )
                {
                    value = reason;
                    setData( sernum, keys[ Keys::kREASON ], value );

                    this->updateRowData( index.row(),
                                         Cols::cREASON,
                                         value );
                }
            }
        break;
        default:
        break;
    }
}
