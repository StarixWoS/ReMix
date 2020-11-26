
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

const QStringList User::keys =
{
    "seen",
    "bio",
    "ip",
    "dv",
    "wv",
    "rank",
    "hash",
    "salt",
    "muted",
    "mutedUntil",
    "muteReason",
    "banned",
    "bannedUntil",
    "banReason",
    "pings",
    "calls",
    "sceneOptOut",
};

const QVector<PunishDurations> User::punishDurations =
{
    PunishDurations::Invalid,
    PunishDurations::THIRTY_SECONDS,
    PunishDurations::ONE_MINUTE,
    PunishDurations::TEN_MINUTES,
    PunishDurations::THIRTY_MINUTES,
    PunishDurations::ONE_HOUR,
    PunishDurations::ONE_DAY,
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
QMutex User::mutex;

User::User(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::User)
{
    ui->setupUi(this);

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &User::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );

    if ( instance == nullptr )
        this->setInstance( this );

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        this->restoreGeometry( Settings::getSetting( SKeys::Positions, this->metaObject()->className() ).toByteArray() );

    //Setup our QSettings Object.
    userData = new QSettings( "userInfo.ini", QSettings::IniFormat );

    //Setup the ServerInfo TableView.
    tblModel = new QStandardItemModel( 0, static_cast<int>( UserCols::ColCount ), nullptr );
    tblModel->setHeaderData( static_cast<int>( UserCols::MuteDuration ), Qt::Horizontal, "Muted Until" );
    tblModel->setHeaderData( static_cast<int>( UserCols::BanDuration ), Qt::Horizontal, "Banned Until" );
    tblModel->setHeaderData( static_cast<int>( UserCols::MuteReason ), Qt::Horizontal, "Mute Reason" );
    tblModel->setHeaderData( static_cast<int>( UserCols::BanReason ), Qt::Horizontal, "Ban Reason" );
    tblModel->setHeaderData( static_cast<int>( UserCols::MuteDate ), Qt::Horizontal, "Mute Date" );
    tblModel->setHeaderData( static_cast<int>( UserCols::LastSeen ), Qt::Horizontal, "Last Seen" );
    tblModel->setHeaderData( static_cast<int>( UserCols::BanDate ), Qt::Horizontal, "Ban Date" );
    tblModel->setHeaderData( static_cast<int>( UserCols::IPAddr ), Qt::Horizontal, "Last IP" );
    tblModel->setHeaderData( static_cast<int>( UserCols::Banned ), Qt::Horizontal, "Banned" );
    tblModel->setHeaderData( static_cast<int>( UserCols::SerNum ), Qt::Horizontal, "SerNum" );
    tblModel->setHeaderData( static_cast<int>( UserCols::Muted ), Qt::Horizontal, "Muted" );
    //tblModel->setHeaderData( static_cast<int>( UserCols::Pings ), Qt::Horizontal, "Pings" );
    tblModel->setHeaderData( static_cast<int>( UserCols::Calls ), Qt::Horizontal, "Calls" );
    tblModel->setHeaderData( static_cast<int>( UserCols::Rank ), Qt::Horizontal, "Rank" );

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

    QObject::connect( tblModel, &QAbstractItemModel::dataChanged, this, &User::updateDataValueSlot );
}

User::~User()
{
    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        Settings::setSetting( this->saveGeometry(), SKeys::Positions, this->metaObject()->className() );

    userData->sync();
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

QString User::requestReason(QWidget* parent)
{
    QString label{ "Punishment Reason ( Sent to User ):" };
    QInputDialog* dialog{ Helper::createInputDialog( parent, label, QInputDialog::TextInput, 355, 170 ) };
    dialog->exec();
    dialog->deleteLater();

    return dialog->textValue();
}

PunishDurations User::requestDuration(QWidget* parent)
{
    const static QStringList items{ "No Duration", "30 Seconds", "1 Minute", "10 Minutes", "30 Minutes", "1 Hour",
                                    "24 Hours", "7 Days", "30 Days", "6 Months", "1 Year", "Permanent" };

    bool ok;
    QString item{ QInputDialog::getItem( parent, "ReMix", "Punishment Duration:", items, 0, false, &ok) };
    if ( ok && !item.isEmpty() )
        return punishDurations[ items.indexOf( item ) ];

    return PunishDurations::Invalid;
}

QSettings* User::getUserData()
{
    return userData;
}

//Public Functions
void User::setData(const QString& key, const QString& subKey, const QVariant& value)
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
    User* user{ User::getInstance() };

    if ( !sernum.isEmpty()
      && !pwd.isEmpty() )
    {
        QString salt{ Helper::genPwdSalt( SALT_LENGTH ) };
        QString hash{ salt + pwd };
                hash = Helper::hashPassword( hash );

        if ( rank == GMRanks::User )
        {
            setData( sernum, keys[ UserKeys::kRANK ], static_cast<int>( GMRanks::GMaster ) );

            QModelIndex index{ user->findModelIndex( Helper::serNumToIntStr( sernum, true ), UserCols::SerNum ) };
            if ( index.isValid() )
                user->updateRowData( index.row(), static_cast<int>( UserCols::Rank ), static_cast<int>( GMRanks::GMaster ) );
        }

        setData( sernum, keys[ UserKeys::kHASH ], hash );
        setData( sernum, keys[ UserKeys::kSALT ], salt );
        return true;
    }
    return false;
}

bool User::getIsAdmin(const QString& sernum)
{
    return getData( sernum, keys[ UserKeys::kRANK ] ).toInt() >= 1;
}

bool User::getHasPassword(const QString& sernum)
{
    QString pwd{ getData( sernum, keys[ UserKeys::kHASH ] ).toString() };

    return pwd.length() > 0;
}

bool User::cmpAdminPwd(const QString& sernum, const QString& value)
{
    QString recSalt{ getData( sernum, keys[ UserKeys::kSALT ] ).toString() };
    QString recHash{ getData( sernum, keys[ UserKeys::kHASH ] ).toString() };

    QString hash{ recSalt + value };
            hash = Helper::hashPassword( hash );

    return hash == recHash;
}

qint32 User::getAdminRank(const QString& sernum)
{
    return getData( sernum, keys[ UserKeys::kRANK ] ).toInt();
}

void User::setAdminRank(const QString& sernum, const GMRanks& rank)
{
    User* user{ User::getInstance() };

    setData( sernum, keys[ UserKeys::kRANK ], static_cast<int>( rank ) );

    QModelIndex index{ user->findModelIndex( sernum, UserCols::SerNum ) };
    if ( index.isValid() )
        user->updateRowData( index.row(), static_cast<int>( UserCols::Rank ), static_cast<int>( rank ) );
}

quint64 User::getIsPunished(const PunishTypes& punishType, const QString& value, const PunishTypes& type, const QString& plrSernum)
{
    if ( value.isEmpty() )
        return false;

    QStringList sernums{ userData->childGroups() };
    QString sernum{ "" };
    QString var{ "" };

    bool isValue{ false };
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
            case PunishTypes::SerNum:
            {
                if ( Helper::cmpStrings( sernum, value ) )
                    isValue = true;
            }
            break;
            case PunishTypes::IP:
            {
                if ( skip )
                    break;

                var = getData( sernum, keys[ UserKeys::kIP ] ).toString();
                if ( Helper::cmpStrings( var, value ) )
                    isValue = true;
            }
            break;
            case PunishTypes::DV:
            case PunishTypes::WV:
            case PunishTypes::Mute:
            case PunishTypes::Ban:
                break;
        }

        skip = false;
        if ( isValue )
            break;
    }

    quint64 punishDuration{ getData( sernum, keys[ UserKeys::kBANDURATION ] ).toUInt() };
    quint64 punishDate{ getData( sernum, keys[ UserKeys::kBANNED ] ).toUInt() };
    quint64 date{ QDateTime::currentDateTimeUtc().toTime_t() };
    if ( punishType == PunishTypes::Mute )
    {
        punishDuration = getData( sernum, keys[ UserKeys::kMUTEDURATION ] ).toUInt();
        punishDate = getData( sernum, keys[ UserKeys::kMUTED ] ).toUInt();
    }

    if ( punishDate > 0 )
    {
        if ( ( punishDuration <= punishDate )
          || ( punishDuration == 0 )
          || ( punishDuration <= date ) )
        {
            removePunishment( value, punishType, type );
            punishDuration = 0;
        }
    }
    return punishDuration;
}

void User::removePunishment(const QString& value, const PunishTypes& punishType, const PunishTypes& type)
{
    QList<QStandardItem*> list;
    if ( type == PunishTypes::SerNum )
        list = tblModel->findItems( Helper::serNumToIntStr( value, true ), Qt::MatchFixedString, static_cast<qint32>( UserCols::SerNum ) );
    else
        list = tblModel->findItems( value, Qt::MatchFixedString, static_cast<qint32>( UserCols::IPAddr ) );

    User* user = User::getInstance();
    if ( list.count() )
    {
        QModelIndex index{ list.value( 0 )->index() };
        if ( index.isValid() )
        {
            QStringList sernums{ userData->childGroups() };
            QString sernum{ "" };
            QString ip{ "" };

            for ( int i = 0; i < sernums.count(); ++i )
            {
                sernum = sernums.at( i );
                if ( type == PunishTypes::IP )
                    ip = userData->value( sernum % "/" % value ).toString();

                if ( Helper::cmpStrings( sernum, value )
                  || Helper::cmpStrings( ip, value ) )
                {
                    if ( punishType == PunishTypes::Ban )
                    {
                        userData->remove( sernum % "/" % keys[ UserKeys::kBANNED ] );
                        userData->remove( sernum % "/" % keys[ UserKeys::kBANREASON ] );
                        userData->remove( sernum % "/" % keys[ UserKeys::kBANDURATION ] );
                    }
                    else if ( punishType == PunishTypes::Mute )
                    {
                        userData->remove( sernum % "/" % keys[ UserKeys::kMUTED ] );
                        userData->remove( sernum % "/" % keys[ UserKeys::kMUTEREASON ] );
                        userData->remove( sernum % "/" % keys[ UserKeys::kMUTEDURATION ] );
                    }
                    break;
                }
            }

            if ( punishType == PunishTypes::Ban )
            {
                user->updateRowData( index.row(), static_cast<int>( UserCols::Banned ), false );
                user->updateRowData( index.row(), static_cast<int>( UserCols::BanReason ), "" );
                user->updateRowData( index.row(), static_cast<int>( UserCols::BanDate ), 0 );
                user->updateRowData( index.row(), static_cast<int>( UserCols::BanDuration ), 0 );
            }
            else if ( punishType == PunishTypes::Mute )
            {
                user->updateRowData( index.row(), static_cast<int>( UserCols::Muted ), false );
                user->updateRowData( index.row(), static_cast<int>( UserCols::MuteDate ), 0 );
                user->updateRowData( index.row(), static_cast<int>( UserCols::MuteDuration ), 0 );
                user->updateRowData( index.row(), static_cast<int>( UserCols::MuteReason ), "" );
            }
        }
    }
}

bool User::addBan(const Player* admin, const Player* target, const QString& reason, const bool remote, const PunishDurations duration)
{
    User* user{ User::getInstance() };
    if ( user == nullptr )
        return false;

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

    quint64 date{ QDateTime::currentDateTimeUtc().toTime_t() };
    quint64 banDuration{ date + static_cast<quint64>( duration ) };
    QString serNum{ target->getSernumHex_s() };

    setData( serNum, keys[ UserKeys::kBANREASON ], msg );
    setData( serNum, keys[ UserKeys::kBANDURATION ], banDuration );
    setData( serNum, keys[ UserKeys::kBANNED ], date );

    QModelIndex index{ user->findModelIndex( serNum, UserCols::SerNum ) };
    if ( index.isValid() )
    {
        user->updateRowData( index.row(), static_cast<int>( UserCols::BanReason ), msg );
        user->updateRowData( index.row(), static_cast<int>( UserCols::BanDate ), date );
        user->updateRowData( index.row(), static_cast<int>( UserCols::Banned ), ( date > 0 ) );
        user->updateRowData( index.row(), static_cast<int>( UserCols::BanDuration ), banDuration );
    }
    return true;
}

bool User::addMute(const Player* admin, Player* target, const QString& reason, const bool& remote, const bool& autoMute, const PunishDurations duration)
{
    User* user{ User::getInstance() };
    if ( user == nullptr )
        return false;

    if ( target == nullptr )
        return false;

    QString msg{ reason };
    if ( msg.isEmpty() )
    {
        if ( remote )
        {
            if ( admin == nullptr )
                return false;

            msg = "Remote-Mute by [ %1 ]; Unknown Reason: [ %2 ]";
            msg = msg.arg( admin->getSernum_s() )
                     .arg( target->getSernum_s() );
        }
        else
        {
            msg = "Manual-Mute; Unknown reason: [ %1 ]";
            msg = msg.arg( target->getSernum_s() );
        }

        if ( autoMute == true )
        {
            msg = "Automatic-Mute; Unknown reason: [ %1 ]";
            msg = msg.arg( target->getSernum_s() );
        }
    }

    quint64 date{ QDateTime::currentDateTimeUtc().toTime_t() };
    quint64 muteDuration{ date + static_cast<quint64>( duration ) };

    QString serNum{ target->getSernumHex_s() };
    if ( target->getSernum_i() == 0 )
        serNum = "00000000"; //Special case.

    target->setMuteDuration( muteDuration );
    setData( serNum, keys[ UserKeys::kMUTEREASON ], msg );
    setData( serNum, keys[ UserKeys::kMUTEDURATION ], muteDuration );
    setData( serNum, keys[ UserKeys::kMUTED ], date );

    QModelIndex index{ user->findModelIndex( serNum, UserCols::SerNum ) };
    if ( index.isValid() )
    {
        user->updateRowData( index.row(), static_cast<int>( UserCols::MuteReason ), msg );
        user->updateRowData( index.row(), static_cast<int>( UserCols::Muted ), ( date > 0 ) );
        user->updateRowData( index.row(), static_cast<int>( UserCols::MuteDate ), date );
        user->updateRowData( index.row(), static_cast<int>( UserCols::MuteDuration ), muteDuration );
    }
    return true;
}

QString User::getMuteReason(const QString& serNum)
{
    return getData( keys[ UserKeys::kMUTEREASON ], serNum ).toString();
}

void User::updateCallCount(const QString& serNum)
{
    quint32 callCount{ getData( serNum, keys[ UserKeys::kCALLS ] ).toUInt() + 1 };
    User* user{ User::getInstance() };

    setData( serNum, keys[ UserKeys::kCALLS ], callCount );

    QModelIndex index{ user->findModelIndex( serNum, UserCols::SerNum ) };
    if ( index.isValid() )
        user->updateRowData( index.row(), static_cast<int>( UserCols::Calls ), callCount );
}

void User::logBIO(const QString& serNum, const QHostAddress& ip, const QString& bio)
{
    QMutexLocker locker( &mutex );

    User* user{ User::getInstance() };
    QString sernum{ serNum };
    if ( Helper::strContainsStr( sernum, "SOUL" ) )
        sernum = Helper::serNumToHexStr( serNum, 8 );

    //quint32 pings{ getData( sernum, keys[ UserKeys::kPINGS ] ).toUInt() + 1 };
    quint64 date{ QDateTime::currentDateTimeUtc().toTime_t() };
    QString ip_s{ ip.toString() };

    setData( sernum, keys[ UserKeys::kBIO ], bio.mid( 1 ) );
    //setData( sernum, keys[ UserKeys::kPINGS ], pings );
    setData( sernum, keys[ UserKeys::kIP ], ip_s );
    setData( sernum, keys[ UserKeys::kSEEN ], date );

    QModelIndex index{ user->findModelIndex( sernum, UserCols::SerNum ) };
    if ( !index.isValid() )
    {
        qint32 row{ tblModel->rowCount() };
        tblModel->insertRow( row );
        index = tblModel->index( row, static_cast<int>( UserCols::SerNum ) );
    }

    if ( index.isValid() )
    {
        user->updateRowData( index.row(), static_cast<int>( UserCols::SerNum ), Helper::serNumToIntStr( sernum, true ) );
        user->updateRowData( index.row(), static_cast<int>( UserCols::IPAddr ), ip_s );
        //user->updateRowData( index.row(), static_cast<int>( UserCols::Pings ), pings );
        user->updateRowData( index.row(), static_cast<int>( UserCols::LastSeen ), date );
    }
}

QByteArray User::getBIOData(const QString& sernum)
{
    return getData( sernum, keys[ UserKeys::kBIO ] ).toByteArray();
}

bool User::getCampOptOut(const QString& plrSernum)
{
    return getData( plrSernum, keys[ UserKeys::kSceneOptOut ] ).toBool();
}

void User::setCampOptOut(const QString& plrSernum, const bool& optOut)
{
    setData( plrSernum, keys[ UserKeys::kSceneOptOut ], optOut );
}

//Private Functions.
QModelIndex User::findModelIndex(const QString& value, const UserCols& col)
{
    QString serNum{ value };
    if ( col == UserCols::SerNum )
        serNum = Helper::serNumToIntStr( value, true );

    QList<QStandardItem*> list{ tblModel->findItems( serNum, Qt::MatchExactly, static_cast<int>( col ) ) };
    QModelIndex index;
    if ( list.count() )
    {
        index = list.value( 0 )->index();
        if ( index.isValid() )
            return index;
    }
    return QModelIndex{ };
}

void User::loadUserInfo()
{
    tblModel->removeRows( 0, tblModel->rowCount() );
    tblProxy->removeRows( 0, tblProxy->rowCount() );

    QString title{ "User Information:" };
    if ( QFile( "userInfo.ini" ).exists() )
    {
        QStringList sernums{ userData->childGroups() };

        title = title.append( " [ %1 ] Users" )
                     .arg( sernums.count() );

        QString sernum{ "" };
        QString muteReason{ "" };
        QString banReason{ "" };
        QString ip{ "" };

        quint64 muteDate_i{ 0 };
        quint64 muteDuration_i{ 0 };
        quint64 banDate_i{ 0 };
        quint64 banDuration_i{ 0 };
        quint64 seen_i{ 0 };
        //quint32 pings_i{ 0 };
        quint32 calls_i{ 0 };

        quint32 rank{ 0 };
        bool banned{ false };
        bool muted{ false };

        int row{ -1 };
        for ( int i = 0; i < sernums.count(); ++i )
        {
            sernum = sernums.at( i );
            muteDuration_i = getData( sernum, keys[ UserKeys::kMUTEDURATION ] ).toUInt();
            banDuration_i = getData( sernum, keys[ UserKeys::kBANDURATION ] ).toUInt();
            muteReason = getData( sernum, keys[ UserKeys::kMUTEREASON ] ).toString();
            banReason = getData( sernum, keys[ UserKeys::kBANREASON ] ).toString();
            muteDate_i = getData( sernum, keys[ UserKeys::kMUTED ] ).toUInt();
            banDate_i = getData( sernum, keys[ UserKeys::kBANNED ] ).toUInt();
            //pings_i = getData( sernum, keys[ UserKeys::kPINGS ] ).toUInt();
            calls_i = getData( sernum, keys[ UserKeys::kCALLS ] ).toUInt();
            seen_i = getData( sernum, keys[ UserKeys::kSEEN ] ).toUInt();
            rank = getData( sernum, keys[ UserKeys::kRANK ] ).toUInt();
            ip = getData( sernum, keys[ UserKeys::kIP ] ).toString();

            banned = banDate_i > 0;
            muted = muteDate_i > 0;

            row = tblModel->rowCount();
            tblModel->insertRow( row );

            tblProxy->rowCount();
            tblModel->setData( tblModel->index( row, 0 ), Helper::serNumToIntStr( sernum, true ), Qt::DisplayRole );

            this->updateRowData( row, static_cast<int>( UserCols::MuteDuration ), muteDuration_i );
            this->updateRowData( row, static_cast<int>( UserCols::BanDuration ), banDuration_i );
            this->updateRowData( row, static_cast<int>( UserCols::MuteReason ), muteReason );
            this->updateRowData( row, static_cast<int>( UserCols::BanReason ), banReason );
            this->updateRowData( row, static_cast<int>( UserCols::MuteDate ), muteDate_i );
            this->updateRowData( row, static_cast<int>( UserCols::BanDate ), banDate_i );
            this->updateRowData( row, static_cast<int>( UserCols::LastSeen ), seen_i );
            this->updateRowData( row, static_cast<int>( UserCols::Banned ), banned );
            //this->updateRowData( row, static_cast<int>( UserCols::Pings ), pings_i );
            this->updateRowData( row, static_cast<int>( UserCols::Calls ), calls_i );
            this->updateRowData( row, static_cast<int>( UserCols::Muted ), muted );
            this->updateRowData( row, static_cast<int>( UserCols::IPAddr ), ip );
            this->updateRowData( row, static_cast<int>( UserCols::Rank ), rank );

            ui->userTable->resizeColumnToContents( static_cast<int>( UserCols::BanReason ) );

            auto informRemoval = [=](const QString& sernum, const quint64& punishDate, const quint64& punishDuration, const QString& punishReason,
                                     const bool& isBan)
            {
                quint64 date{ QDateTime::currentDateTimeUtc().toTime_t() };
                if ( ( punishDuration <= punishDate )
                  || ( punishDuration == 0 )
                  || ( punishDuration <= date ) )
                {
                    QString message{ "Removing the %1 User [ %2 ]. %1 on [ %3 ] until [ %4 ]; With the reason [ %5 ]." };
                    QString banned{ "Banned" };
                    QString muted{ "Muted" };
                    QString fill{ "" };

                    if ( isBan )
                        fill = banned;
                    else
                        fill = muted;

                    message = message.arg( fill )
                                     .arg( Helper::serNumToIntStr( sernum, true ) )
                                     .arg( Helper::getTimeAsString( punishDate ) )
                                     .arg( Helper::getTimeAsString( punishDuration ) )
                                     .arg( punishReason );
                    Logger::getInstance()->insertLog( "User", message, LogTypes::PUNISHMENT, true, true );
                }
            };

            if ( banned )
            {
                this->removePunishment( sernum, PunishTypes::Ban, PunishTypes::SerNum );
                informRemoval( sernum, banDate_i, banDuration_i, banReason, true );
            }

            if ( muted )
            {
                this->removePunishment( sernum, PunishTypes::Mute, PunishTypes::SerNum );
                informRemoval( sernum, muteDate_i, muteDuration_i, muteReason, false );
            }
        }
    }

    this->setWindowTitle( title );
    ui->userTable->selectRow( 0 );
    ui->userTable->resizeColumnsToContents();
}

void User::updateRowData(const qint32& row, const qint32& col, const QVariant& data)
{
    QModelIndex index{ tblModel->index( row, col ) };
    if ( index.isValid() )
    {
        QString msg{ "" };
        if ( col == static_cast<int>( UserCols::LastSeen )
          || col == static_cast<int>( UserCols::BanDate )
          || col == static_cast<int>( UserCols::BanDuration )
          || col == static_cast<int>( UserCols::MuteDate )
          || col == static_cast<int>( UserCols::MuteDuration ) )
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

            if ( col == static_cast<int>( UserCols::MuteDate ) )
                ui->userTable->resizeColumnToContents( col );

            if ( col == static_cast<int>( UserCols::MuteDuration ) )
                ui->userTable->resizeColumnToContents( col );
        }
        else
        {
            if ( col == static_cast<int>( UserCols::BanReason ) )
                ui->userTable->resizeColumnToContents( col );

            if ( col == static_cast<int>( UserCols::MuteReason ) )
                ui->userTable->resizeColumnToContents( col );

            tblModel->setData( index, data, Qt::DisplayRole );
        }
    }
}

//Private Slots.
void User::updateDataValueSlot(const QModelIndex& index, const QModelIndex&, const QVector<int>&)
{
    QString sernum{ "" };
    QVariant value;

    sernum = tblModel->data( tblModel->index( index.row(), static_cast<int>( UserCols::SerNum ) ) ).toString();
    sernum = Helper::sanitizeSerNum( sernum );

    switch ( static_cast<UserCols>( index.column() ) )
    {
        case UserCols::Rank:
            {
                QString title{ "Remove Password:" };
                QString message{ "Do you wish to remove this Admin's password information?" };

                GMRanks rank{ static_cast<GMRanks>( getAdminRank( sernum ) ) };
                bool hasPassword{ getHasPassword( sernum ) };
                bool removePassword{ false };

                value = tblModel->data( index );
                if (( rank > GMRanks::User )
                  && ( static_cast<GMRanks>( value.toInt() ) == GMRanks::User ) )
                {
                    if ( hasPassword )
                        removePassword = Helper::confirmAction( this, title, message );
                }
                else if (( rank == GMRanks::User )
                       && ( static_cast<GMRanks>( value.toInt() ) > GMRanks::User ) )
                {
                    if ( hasPassword )
                    {
                        message = "This user has a previous password stored. Do you wish to reset the password?";
                        removePassword =  Helper::confirmAction( this, title, message );
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

                quint64 banDuration{ 0 };
                quint64 banDate{ 0 };

                bool banned{ tblModel->data( index ).toBool() };
                if ( banned )
                {
                    reason = getData( sernum, keys[ UserKeys::kBANREASON ] ).toString();
                    if ( reason.isEmpty() )
                    {
                        banDate = QDateTime::currentDateTimeUtc().toTime_t();
                        setData( sernum, keys[ UserKeys::kBANNED ], banDate );
                        this->updateRowData( index.row(), static_cast<int>( UserCols::BanDate ), banDate );

                        setReason = true;
                        reason = "Manual Banish; " % requestReason( this );

                        banDuration = banDate + static_cast<uint>( requestDuration( this ) );

                        setData( sernum, keys[ UserKeys::kBANDURATION ], banDuration );
                        this->updateRowData( index.row(), static_cast<int>( UserCols::BanDuration ), banDuration );
                    }
                }
                else
                {
                    setReason = true;
                    reason.clear();
                    value = 0;

                    setData( sernum, keys[ UserKeys::kBANNED ], banDate );
                    this->updateRowData( index.row(), static_cast<int>( UserCols::BanDate ), banDate );

                    setData( sernum, keys[ UserKeys::kBANDURATION ], banDuration );
                    this->updateRowData( index.row(), static_cast<int>( UserCols::BanDuration ), banDuration );
                }

                if ( setReason )
                {
                    setData( sernum, keys[ UserKeys::kBANREASON ], reason );
                    this->updateRowData( index.row(), static_cast<int>( UserCols::BanReason ), reason );
                }
            }
        break;
        case UserCols::Muted:
            {
                bool setReason{ false };
                QString reason{ "" };

                quint64 muteDuration{ 0 };
                quint64 muteDate{ 0 };

                bool muted{ tblModel->data( index ).toBool() };
                if ( muted )
                {
                    reason = getData( sernum, keys[ UserKeys::kMUTEREASON ] ).toString();
                    if ( reason.isEmpty() )
                    {
                        setReason = true;
                        reason = "Manual Mute; " % requestReason( this );

                        muteDate = QDateTime::currentDateTimeUtc().toTime_t();
                        muteDuration = muteDate + static_cast<uint>( requestDuration( this ) );

                        emit this->mutedSerNumDurationSignal( sernum, muteDuration );

                        setData( sernum, keys[ UserKeys::kMUTED ], muteDate );
                        this->updateRowData( index.row(), static_cast<int>( UserCols::MuteDate ), muteDate );

                        setData( sernum, keys[ UserKeys::kMUTEDURATION ], muteDuration );
                        this->updateRowData( index.row(), static_cast<int>( UserCols::MuteDuration ), muteDuration );
                    }
                }
                else
                {
                    setReason = true;
                    reason.clear();

                    removePunishment( sernum, PunishTypes::Mute, PunishTypes::SerNum );
                    emit this->mutedSerNumDurationSignal( sernum, static_cast<int>( PunishDurations::Invalid ) );
                }

                if ( setReason )
                {
                    setData( sernum, keys[ UserKeys::kMUTEREASON ], reason );
                    this->updateRowData( index.row(), static_cast<int>( UserCols::MuteReason ), reason );
                }
            }
        break;
        default:
        break;
    }
}
