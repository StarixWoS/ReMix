
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
    tblModel = new QStandardItemModel( 0, 7, 0 );
    tblModel->setHeaderData( 0, Qt::Horizontal, "SerNum" );
    tblModel->setHeaderData( 1, Qt::Horizontal, "Last Seen" );
    tblModel->setHeaderData( 2, Qt::Horizontal, "Last IP" );
    tblModel->setHeaderData( 3, Qt::Horizontal, "Rank" );
    tblModel->setHeaderData( 4, Qt::Horizontal, "Banned" );
    tblModel->setHeaderData( 5, Qt::Horizontal, "Ban Reason" );
    tblModel->setHeaderData( 6, Qt::Horizontal, "Ban Date" );

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
            userData->setValue( sernum % "/rank", Ranks::rGAMEMASTER );

        userData->setValue( sernum % "/hash", hash );
        userData->setValue( sernum % "/salt", salt );

        this->loadUserInfo();
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

    this->loadUserInfo();
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
            this->loadUserInfo();
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

    this->loadUserInfo();

    return true;
}

bool User::getIsBanned(QString value)
{
    if ( value.isEmpty() )
        return false;

    QStringList sernums = userData->childGroups();
    QString sernum{ "" };
    QString var{ "" };

    bool isValue{ false };
    quint32 banned{ 0 };

    for ( int i = 0; i < sernums.count(); ++i )
    {
        sernum = sernums.at( i );
        if ( sernum.compare( value ) != 0 )
        {
            if ( !isValue )
            {
                var = getData( sernum, keys[ Keys::kIP ] ).toString();
                if (  var == value )
                    isValue = true;
            }

            if ( !isValue )
            {
                var = getData( sernum, keys[ Keys::kIP ] ).toString();
                if (  var == value )
                    isValue = true;
            }

            if ( !isValue )
            {
                var = getData( sernum, keys[ Keys::kIP ] ).toString();
                if (  var == value )
                    isValue = true;
            }
        }
        else
            isValue = true;

        if ( isValue )
        {
            banned = getData( sernum, keys[ Keys::kBANNED ] ).toUInt();
            break;
        }
    }
    return banned > 0;
}

void User::logBIO(QString& serNum, QHostAddress& ip, QString& dv,
                  QString& wv, QString& bio)
{
    QString sernum{ serNum };
    if ( sernum.contains( "SOUL", Qt::CaseInsensitive ) )
        sernum = Helper::serNumToHexStr( serNum, 8 );

    userData->setValue( sernum % "/bio", bio.mid( 1 ) );
    userData->setValue( sernum % "/ip", ip.toString() );
    userData->setValue( sernum % "/dv", dv );
    userData->setValue( sernum % "/wv", wv );

    userData->setValue( sernum % "/seen",
                       QDateTime::currentDateTime().toTime_t() );

    this->loadUserInfo();
}

//Private Functions.
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

        QString banDate_s{ "" };
        quint64 banDate_i{ 0 };

        QString seen_s{ "Never Seen" };
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

            tblModel->setData( tblModel->index( row, 0 ),
                               Helper::serNumToIntStr( sernum ),
                               Qt::DisplayRole );

            if ( seen_i > 0 )
            {
                seen_s = QDateTime::fromTime_t( seen_i )
                         .toString( "ddd MMM dd HH:mm:ss yyyy" );
            }
            tblModel->setData( tblModel->index( row, 1 ),
                               seen_s,
                               Qt::DisplayRole );

            tblModel->setData( tblModel->index( row, 2 ),
                               ip, Qt::DisplayRole );

            tblModel->setData( tblModel->index( row, 3 ),
                               rank, Qt::DisplayRole );

            tblModel->setData( tblModel->index( row, 4 ),
                               banned, Qt::DisplayRole );

            tblModel->setData( tblModel->index( row, 5 ),
                               reason, Qt::DisplayRole );

            if ( banDate_i > 0 )
            {
                banDate_s = QDateTime::fromTime_t( banDate_i )
                                 .toString( "ddd MMM dd HH:mm:ss yyyy" );
            }
            tblModel->setData( tblModel->index( row, 6 ),
                               banDate_s,
                               Qt::DisplayRole );

            banDate_s = "";
            seen_s = "Never Seen";
        }
        ui->userTable->selectRow( 0 );
        ui->userTable->resizeColumnsToContents();
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
                QString reason{ "Manual Banish; %1" };
                QString date{ "" };

                bool banned = tblModel->data( index ).toBool();
                if ( banned )
                {
                    value = QDateTime::currentDateTime().toTime_t();
                    date = QDateTime::fromTime_t( value.toUInt() )
                                .toString( "ddd MMM dd HH:mm:ss yyyy" );

                    reason = reason.arg( Helper::getBanishReason( this ) );
                }
                else
                {
                    reason.clear();
                    value = 0;
                }

                setData( sernum, keys[ Keys::kBANNED ], value );

                value = reason;
                setData( sernum, keys[ Keys::kREASON ], value );

                tblModel->setData( tblModel->index( index.row(),
                                                    Cols::cREASON ),
                                   reason,
                                   Qt::DisplayRole );

                tblModel->setData( tblModel->index( index.row(),
                                                    Cols::cDATE ),
                                   date,
                                   Qt::DisplayRole );
            }
        break;
        default:
        break;
    }
}
