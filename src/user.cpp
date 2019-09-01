
//Class includes.
#include "user.hpp"
#include "ui_user.h"

//ReMix Widget includes.
#include "widgets/userdelegate.hpp"

//ReMix includes.
#include "views/usersortproxymodel.hpp"
#include "settings.hpp"
#include "randdev.hpp"
#include "logger.hpp"
#include "helper.hpp"
#include "player.hpp"

//Qt Includes.
#include <QStandardItemModel>
#include <QHostAddress>
#include <QSettings>
#include <QVariant>
#include <QString>
#include <QObject>
#include <QtCore>

const QString User::keys[ USER_KEY_COUNT ] =
{
    "seen",
    "bio",
    "ip",
    "dv",
    "wv",
    "rank",
    "hash",
    "salt",
    "reason",
    "banned",
    "bannedUntil",
    "pings",
    "calls"
};

const PunishDurations User::punishDurations[ PUNISH_DURATION_COUNT ] =
{
    PunishDurations::Invalid,
    PunishDurations::One_Day,
    PunishDurations::SEVEN_DAYS,
    PunishDurations::THIRTY_DAYS,
    PunishDurations::SIX_MONTHS,
    PunishDurations::ONE_YEAR,
    PunishDurations::PERMANENT,
};

QSortFilterProxyModel* User::tblProxy{ nullptr };
QStandardItemModel* User::tblModel{ nullptr };
QSettings* User::userData{ nullptr };
User* User::instance{ nullptr };

User::User(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::User)
{
    ui->setupUi(this);

    if ( instance == nullptr )
        this->setInstance( this );

    if ( Settings::getSaveWindowPositions() )
    {
        this->restoreGeometry( Settings::getWindowPositions(
                                   this->metaObject()->className() ) );
    }

    //Setup our QSettings Object.
    userData = new QSettings( "userInfo.ini", QSettings::IniFormat );

    //Setup the ServerInfo TableView.
    tblModel = new QStandardItemModel( 0, static_cast<int>(
                                           UserCols::ColCount ), nullptr );
    tblModel->setHeaderData( static_cast<int>( UserCols::SerNum ),
                             Qt::Horizontal,
                             "SerNum" );
    tblModel->setHeaderData( static_cast<int>( UserCols::Pings ),
                             Qt::Horizontal,
                             "Pings" );
    tblModel->setHeaderData( static_cast<int>( UserCols::Calls ),
                             Qt::Horizontal,
                             "Calls" );
    tblModel->setHeaderData( static_cast<int>( UserCols::LastSeen ),
                             Qt::Horizontal,
                             "Last Seen" );
    tblModel->setHeaderData( static_cast<int>( UserCols::IPAddr ),
                             Qt::Horizontal,
                             "Last IP" );
    tblModel->setHeaderData( static_cast<int>( UserCols::Rank ),
                             Qt::Horizontal,
                             "Rank" );
    tblModel->setHeaderData( static_cast<int>( UserCols::Banned ),
                             Qt::Horizontal,
                             "Banned" );
    tblModel->setHeaderData( static_cast<int>( UserCols::BanDate ),
                             Qt::Horizontal,
                             "Ban Date" );
    tblModel->setHeaderData( static_cast<int>( UserCols::BanDuration ),
                             Qt::Horizontal,
                             "Banned Until" );
    tblModel->setHeaderData( static_cast<int>( UserCols::BanReason ),
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

User* User::getInstance()
{
    if ( instance == nullptr )
        instance = new User( nullptr );

    return instance;
}

void User::setInstance(User* value)
{
    instance = value;
}

QString User::requestBanishReason(QWidget* parent)
{
    QString label{ "Ban Reason ( Sent to User ):" };
    QInputDialog* dialog{
        Helper::createInputDialog( parent, label,
                                   QInputDialog::TextInput,
                                   355, 170 ) };
    dialog->exec();
    dialog->deleteLater();

    return dialog->textValue();
}

PunishDurations User::requestPunishDuration(QWidget* parent)
{
    QStringList items;
                items << "No Duration" << "24 Hours" << "7 Days" << "30 Days"
                      << "6 Months" << "1 Year" << "Permanent";

    bool ok;
    QString item = QInputDialog::getItem( parent, "ReMix",
                                          "Punishment Duration:",
                                          items, 0, false, &ok);
    if ( ok && !item.isEmpty() )
    {
        return punishDurations[ items.indexOf( item ) ];
    }
    return PunishDurations::Invalid;
}

QSettings* User::getUserData()
{
    return userData;
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

bool User::makeAdmin(const QString& sernum, const QString& pwd)
{
    GMRanks rank{ static_cast<GMRanks>( getAdminRank( sernum ) ) };
    User* user = User::getInstance();

    if ( !sernum.isEmpty()
      && !pwd.isEmpty() )
    {
        QString salt = Helper::genPwdSalt( SALT_LENGTH );
        QString hash( salt + pwd );
                hash = Helper::hashPassword( hash );

        if ( rank == GMRanks::User )
        {
            setData( sernum, keys[ UserKeys::kRANK ],
                     static_cast<int>( GMRanks::GMaster ) );
            QModelIndex index = user->findModelIndex(
                                    Helper::serNumToIntStr( sernum ),
                                    UserCols::SerNum );
            if ( index.isValid() )
            {
                user->updateRowData( index.row(),
                                     static_cast<int>( UserCols::Rank ),
                                     static_cast<int>( GMRanks::GMaster ) );
            }
        }

        setData( sernum, keys[ UserKeys::kHASH ], hash );
        setData( sernum, keys[ UserKeys::kSALT ], salt );
        return true;
    }
    return false;
}

bool User::getIsAdmin(const QString& sernum)
{
    return getData( sernum, keys[ UserKeys::kRANK ] )
              .toInt() >= 1;
}

bool User::getHasPassword(const QString& sernum)
{
    QString pwd{ getData( sernum, keys[ UserKeys::kHASH ] )
                    .toString() };

    return pwd.length() > 0;
}

bool User::cmpAdminPwd(const QString& sernum, const QString& value)
{
    QString recSalt = getData( sernum, keys[ UserKeys::kSALT ] )
                         .toString();
    QString recHash = getData( sernum, keys[ UserKeys::kHASH ] )
                         .toString();

    QString hash{ recSalt + value };
            hash = Helper::hashPassword( hash );

    return hash == recHash;
}

qint32 User::getAdminRank(const QString& sernum)
{
    return getData( sernum, keys[ UserKeys::kRANK ] )
              .toInt();
}

void User::setAdminRank(const QString& sernum, const GMRanks& rank)
{
    User* user = User::getInstance();

    setData( sernum, keys[ UserKeys::kRANK ], static_cast<int>( rank ) );

    QModelIndex index = user->findModelIndex( sernum,
                                              UserCols::SerNum );
    if ( index.isValid() )
    {
        user->updateRowData( index.row(),
                             static_cast<int>( UserCols::Rank ),
                             static_cast<int>( rank ) );
    }
}

void User::removeBan(const QString& value, const qint32& type)
{
    QList<QStandardItem*> list = tblModel->findItems(
                                     Helper::serNumToIntStr( value ),
                                     Qt::MatchExactly,
                                     type );
    User* user = User::getInstance();
    if ( list.count() >= 2 )
    {
        return; //Too many listed Bans, do nothing. --Inform the User later?
    }
    else if ( list.count() )
    {
        QModelIndex index = list.value( 0 )->index();
        if ( index.isValid() )
        {
            QStringList sernums = userData->childGroups();
            QString sernum{ "" };

            for ( int i = 0; i < sernums.count(); ++i )
            {
                sernum = sernums.at( i );
                if ( Helper::cmpStrings( sernum, value ) )
                {
                    userData->remove( sernum % "/"
                                    % keys[ UserKeys::kBANNED ] );
                    userData->remove( sernum % "/"
                                    % keys[ UserKeys::kREASON ] );
                    userData->remove( sernum % "/"
                                    % keys[ UserKeys::kBANDURATION ] );

                    break;
                }
            }
            user->updateRowData( index.row(),
                                 static_cast<int>( UserCols::Banned ),
                                 false );

            user->updateRowData( index.row(),
                                 static_cast<int>( UserCols::BanReason ),
                                 "" );

            user->updateRowData( index.row(),
                                 static_cast<int>( UserCols::BanDate ),
                                 0 );

            user->updateRowData( index.row(),
                                 static_cast<int>( UserCols::BanDuration ),
                                 0 );
        }
    }
}

bool User::addBan(const Player* admin, const Player* target,
                  const QString& reason, const bool remote,
                  const PunishDurations duration)
{
    User* user = User::getInstance();
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
    quint64 banDuration{ date + static_cast<int>( duration ) };
    QString serNum{ target->getSernumHex_s() };

    setData( serNum, keys[ UserKeys::kBANNED ], date );
    setData( serNum, keys[ UserKeys::kREASON ], msg );
    setData( serNum, keys[ UserKeys::kBANDURATION ], banDuration );

    QModelIndex index = user->findModelIndex( serNum, UserCols::SerNum );
    if ( index.isValid() )
    {
        user->updateRowData( index.row(),
                             static_cast<int>( UserCols::Banned ),
                             ( date > 0 ) );

        user->updateRowData( index.row(),
                             static_cast<int>( UserCols::BanReason ),
                             msg );

        user->updateRowData( index.row(),
                             static_cast<int>( UserCols::BanDate ),
                             date );

        //All Bans are default to 30 days unless manually changed by the
        //server host.
        user->updateRowData( index.row(),
                             static_cast<int>( UserCols::BanDuration ),
                             banDuration );
    }
    return true;
}

bool User::getIsBanned(const QString& value, const BanTypes& type,
                       const QString& plrSernum)
{
    if ( value.isEmpty() )
        return false;

    QString sernum{ "" };
    QString var{ "" };

    bool isValue{ false };
    bool banned{ false };

    QStringList sernums = userData->childGroups();

    bool skip{ false };

    for ( int i = 0; i < sernums.count(); ++i )
    {
        sernum = sernums.at( i );
        if ( !plrSernum.isEmpty() )
        {
            if ( Helper::cmpStrings( sernum, plrSernum ) )
                skip = true;
        }

        switch ( type )
        {
            case BanTypes::SerNum:
            {
                if ( Helper::cmpStrings( sernum, value ) )
                    isValue = true;
            }
            break;
            case BanTypes::IP:
            {
                if ( skip )
                    break;

                var = getData( sernum, keys[ UserKeys::kIP ] ).toString();
                if ( Helper::cmpStrings( var, value ) )
                    isValue = true;
            }
            break;
            case BanTypes::DV:
            {
                if ( skip )
                    break;

                var = getData( sernum, keys[ UserKeys::kDV ] ).toString();
                if ( Helper::cmpStrings( var, value ) )
                    isValue = true;
            }
            break;
            case BanTypes::WV:
            {
                if ( skip )
                    break;

                var = getData( sernum, keys[ UserKeys::kWV ] ).toString();
                if ( Helper::cmpStrings( var, value ) )
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

        skip = false;
        if ( isValue )
            break;
        else
            continue;
    }

    quint32 banDate{ getData( sernum, keys[ UserKeys::kBANNED ] ).toUInt() };
    if ( banDate > 0 )
    {
        banned = true;
        quint64 date{ QDateTime::currentDateTime().toTime_t() };
        quint64 banDuration{ getData( sernum, keys[ UserKeys::kBANDURATION ] )
                                .toUInt() };

        if ( ( banDuration <= banDate )
          || ( banDuration == 0 )
          || ( banDuration <= date ) )
        {
            banned = false;
            removeBan( plrSernum, static_cast<int>( BanTypes::SerNum ) );
        }
    }
    return banned;
}

void User::updateCallCount(const QString& serNum)
{
    quint32 callCount{ getData( serNum, keys[ UserKeys::kCALLS ] )
                          .toUInt() + 1 };
    User* user = User::getInstance();

    setData( serNum, keys[ UserKeys::kCALLS ], callCount );

    QModelIndex index = user->findModelIndex( serNum,
                                              UserCols::SerNum );
    if ( index.isValid() )
    {
        user->updateRowData( index.row(),
                             static_cast<int>( UserCols::Calls ),
                             callCount );
    }
}

void User::logBIO(const QString& serNum, const QHostAddress& ip,
                  const QString& dv, const QString& wv, const QString& bio)
{
    User* user = User::getInstance();
    QString sernum{ serNum };
    if ( Helper::strContainsStr( sernum, "SOUL" ) )
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

    QModelIndex index = user->findModelIndex( sernum,
                                              UserCols::SerNum );
    if ( !index.isValid() )
    {
        qint32 row = tblModel->rowCount();
        tblModel->insertRow( row );
        index = tblModel->index( row, static_cast<int>( UserCols::SerNum ) );
    }

    if ( index.isValid() )
    {
        user->updateRowData( index.row(),
                             static_cast<int>(
                                 UserCols::SerNum ),
                             Helper::serNumToIntStr( sernum ) );

        user->updateRowData( index.row(),
                             static_cast<int>(
                                 UserCols::IPAddr ),
                             ip_s );

        user->updateRowData( index.row(),
                             static_cast<int>(
                                 UserCols::Pings ),
                             pings );

        user->updateRowData( index.row(),
                             static_cast<int>(
                                 UserCols::LastSeen ),
                             date );
    }
}

QByteArray User::getBIOData(const QString& sernum)
{
    return getData( sernum, keys[ UserKeys::kBIO ] ).toByteArray();
}

//Private Functions.
QModelIndex User::findModelIndex(const QString& value, const UserCols& col)
{
    QString serNum{ value };
    if ( col == UserCols::SerNum )
        serNum = Helper::serNumToIntStr( value );

    QList<QStandardItem*> list = tblModel->findItems( serNum,
                                                      Qt::MatchExactly,
                                                      static_cast<int>( col ) );
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
        quint64 banDuration_i{ 0 };
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
            banDuration_i = getData( sernum, keys[ UserKeys::kBANDURATION ] )
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
                                 static_cast<int>( UserCols::Pings ),
                                 pings_i );

            this->updateRowData( row,
                                 static_cast<int>( UserCols::Calls ),
                                 calls_i );

            this->updateRowData( row,
                                 static_cast<int>( UserCols::LastSeen ),
                                 seen_i );

            this->updateRowData( row,
                                 static_cast<int>( UserCols::IPAddr ),
                                 ip );

            this->updateRowData( row,
                                 static_cast<int>( UserCols::Rank ),
                                 rank );

            this->updateRowData( row,
                                 static_cast<int>( UserCols::Banned ),
                                 banned );

            this->updateRowData( row,
                                 static_cast<int>( UserCols::BanReason ),
                                 reason );

            this->updateRowData( row,
                                 static_cast<int>( UserCols::BanDate ),
                                 banDate_i );

            this->updateRowData( row,
                                 static_cast<int>( UserCols::BanDuration ),
                                 banDuration_i );
            ui->userTable->resizeColumnToContents(
                        static_cast<int>( UserCols::BanReason ) );

            if ( banned )
            {
                quint64 date{ QDateTime::currentDateTime().toTime_t() };
                if ( ( banDuration_i <= banDate_i )
                  || ( banDuration_i == 0 )
                  || ( banDuration_i <= date ) )
                {
                    this->removeBan( sernum,
                                     static_cast<int>( BanTypes::SerNum ) );
                    QString message{ "Automatically removing the Banned User "
                                     "[ %1 ]. Banned on [ %2 ] until [ %3 ]; "
                                     "With the reason [ %4 ]."};
                    message = message.arg( Helper::serNumToIntStr(
                                               sernum ) )
                                     .arg( Helper::getTimeAsString(
                                               banDate_i ) )
                                     .arg( Helper::getTimeAsString(
                                               banDuration_i ) )
                                     .arg( reason );
                    Logger::getInstance()->insertLog( "BanLog", message,
                                                      LogTypes::BAN, true,
                                                      true );
                }
            }
        }
    }

    this->setWindowTitle( title );
    ui->userTable->selectRow( 0 );
    ui->userTable->resizeColumnsToContents();
}

void User::updateRowData(const qint32& row, const qint32& col,
                         const QVariant& data)
{
    QModelIndex index = tblModel->index( row, col );
    if ( index.isValid() )
    {
        QString msg{ "" };
        if ( col == static_cast<int>( UserCols::LastSeen )
          || col == static_cast<int>( UserCols::BanDate )
          || col == static_cast<int>( UserCols::BanDuration ) )
        {
            uint date{ data.toUInt() };
            if ( date > 0 )
            {
                msg = Helper::getTimeAsString( date );
            }
            else
            {
                if ( col == static_cast<int>( UserCols::LastSeen ) )
                    msg = "Never Seen";
                else
                    msg = "";
            }

            tblModel->setData( index, msg, Qt::DisplayRole );
            if ( col == static_cast<int>( UserCols::BanDate ) )
                ui->userTable->resizeColumnToContents( col );

            if ( col == static_cast<int>( UserCols::BanDuration ) )
                ui->userTable->resizeColumnToContents( col );

            if ( col == static_cast<int>( UserCols::BanReason ) )
                ui->userTable->resizeColumnToContents( col );
        }
        else
        {
            tblModel->setData( index, data, Qt::DisplayRole );
        }
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
                     index.row(), static_cast<int>(
                                      UserCols::SerNum ) ) ).toString();
    sernum = Helper::sanitizeSerNum( sernum );

    switch ( static_cast<UserCols>( index.column() ) )
    {
        case UserCols::Rank:
            {
                QString title{ "Remove Password:" };
                QString message{ "Do you wish to remove this "
                                 "Admin's password information?" };

                GMRanks rank{ static_cast<GMRanks>( getAdminRank( sernum ) ) };
                bool hasPassword{ getHasPassword( sernum ) };
                bool removePassword{ false };

                value = tblModel->data( index );
                if (( rank > GMRanks::User )
                  && ( static_cast<GMRanks>( value.toInt() )
                       == GMRanks::User ) )
                {
                    if ( hasPassword )
                    {
                        removePassword =  Helper::confirmAction(
                                              this, title, message );
                    }
                }
                else if (( rank == GMRanks::User )
                       && ( static_cast<GMRanks>( value.toInt() )
                            > GMRanks::User ) )
                {
                    if ( hasPassword )
                    {
                        message = "This user has a previous password stored. "
                                  "Do you wish to reset the password?";
                        removePassword =  Helper::confirmAction(
                                                    this, title, message );
                    }
                }

                if ( removePassword )
                {

                    setData( sernum, keys[ UserKeys::kHASH ], "" );
                    setData( sernum, keys[ UserKeys::kSALT ], "" );
                }

                setData( sernum, keys[ UserKeys::kRANK ], value );
            }
        break;
        case UserCols::Banned:
            {
                bool setReason{ false };
                QString reason{ "" };

                bool banned = tblModel->data( index ).toBool();
                if ( banned )
                {
                    value = QDateTime::currentDateTime().toTime_t();
                    setData( sernum, keys[ UserKeys::kBANNED ], value );
                    this->updateRowData( index.row(),
                                         static_cast<int>(
                                             UserCols::BanDate ),
                                         value );


                    reason = getData( sernum, keys[ UserKeys::kREASON ] )
                                .toString();
                    if ( reason.isEmpty() )
                    {
                        setReason = true;
                        reason = "Manual Banish; "
                                 % requestBanishReason( this );

                        value = value.toUInt() +
                                static_cast<int>(
                                    requestPunishDuration( this ) );

                        setData( sernum, keys[ UserKeys::kBANDURATION ], value );
                        this->updateRowData( index.row(),
                                             static_cast<int>(
                                                 UserCols::BanDuration ),
                                             value );

                    }
                }
                else
                {
                    setReason = true;
                    reason.clear();
                    value = 0;

                    setData( sernum, keys[ UserKeys::kBANNED ], value );
                    this->updateRowData( index.row(),
                                         static_cast<int>(
                                             UserCols::BanDate ),
                                         value );

                    setData( sernum, keys[ UserKeys::kBANDURATION ], value );
                    this->updateRowData( index.row(),
                                         static_cast<int>(
                                             UserCols::BanDuration ),
                                         value );
                }

                if ( setReason )
                {
                    value = reason;
                    setData( sernum, keys[ UserKeys::kREASON ], value );

                    this->updateRowData( index.row(),
                                         static_cast<int>(
                                             UserCols::BanReason ),
                                         value );
                }
            }
        break;
        default:
        break;
    }
}
