
//Class includes.
#include "user.hpp"
#include "ui_user.h"

//ReMix Widget includes.
#include "widgets/userdelegate.hpp"

//ReMix includes.
#include "views/usersortproxymodel.hpp"
#include "views/tbleventfilter.hpp"
#include "settings.hpp"
#include "randdev.hpp"
#include "logger.hpp"
#include "helper.hpp"
#include "player.hpp"
#include "remix.hpp"

//Qt Includes.
#include <QStandardItemModel>
#include <QHostAddress>
#include <QSettings>
#include <QVariant>
#include <QString>
#include <QObject>
#include <QtCore>

const QMap<UKeys, QString> User::uKeys =
{
    { UKeys::Seen,          "seen"        },
    { UKeys::Bio,           "bio"         },
    { UKeys::IP,            "ip"          },
    { UKeys::DV,            "dv"          },
    { UKeys::WV,            "wv"          },
    { UKeys::Rank,          "rank"        },
    { UKeys::Hash,          "hash"        },
    { UKeys::Salt,          "salt"        },
    { UKeys::Muted,         "muted"       },
    { UKeys::MuteDuration,  "mutedUntil"  },
    { UKeys::MuteReason,    "muteReason"  },
    { UKeys::Banned,        "banned"      },
    { UKeys::BanDuration,   "bannedUntil" },
    { UKeys::BanReason,     "banReason"   },
    { UKeys::Pings,         "pings"       },
    { UKeys::Calls,         "calls"       },
    { UKeys::Vanished,      "isVanished"  },
};

const QMap<PunishDurations, QString> User::punishDurations =
{
    { PunishDurations::Invalid,         "No Duration" },
    { PunishDurations::THIRTY_SECONDS,  "30 Seconds"  },
    { PunishDurations::ONE_MINUTE,      "1 Minute"    },
    { PunishDurations::TEN_MINUTES,     "10 Minutes"  },
    { PunishDurations::THIRTY_MINUTES,  "30 Minutes"  },
    { PunishDurations::ONE_HOUR,        "1 Hour"      },
    { PunishDurations::ONE_DAY,         "24 Hours"    },
    { PunishDurations::SEVEN_DAYS,      "7 Days"      },
    { PunishDurations::THIRTY_DAYS,     "30 Days"     },
    { PunishDurations::SIX_MONTHS,      "6 Months"    },
    { PunishDurations::ONE_YEAR,        "1 Year"      },
    { PunishDurations::PERMANENT,       "Permanent"   },
};

const QMap<GMRanks, QString> User::adminRanks =
{
    { GMRanks::User,    "User"        },
    { GMRanks::GMaster, "Game Master" },
    { GMRanks::CoAdmin, "Co-Admin"    },
    { GMRanks::Admin,   "Admin"       },
    { GMRanks::Owner,   "Owner"       },
    { GMRanks::Creator, "Creator"     },
};

QSortFilterProxyModel* User::tblProxy{ nullptr };
QStandardItemModel* User::tblModel{ nullptr };
QSettings* User::userData{ new QSettings( "userInfo.ini", QSettings::IniFormat ) };
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
        this->restoreGeometry( Settings::getSetting( SKeys::Positions, "User" ).toByteArray() );

    //Setup the Server TableView.
    tblModel = new QStandardItemModel( 0, *UserCols::ColCount, nullptr );
    tblModel->setHeaderData( *UserCols::MuteDuration, Qt::Horizontal, "Muted Until" );
    tblModel->setHeaderData( *UserCols::BanDuration, Qt::Horizontal, "Banned Until" );
    tblModel->setHeaderData( *UserCols::MuteReason, Qt::Horizontal, "Mute Reason" );
    tblModel->setHeaderData( *UserCols::BanReason, Qt::Horizontal, "Ban Reason" );
    tblModel->setHeaderData( *UserCols::MuteDate, Qt::Horizontal, "Mute Date" );
    tblModel->setHeaderData( *UserCols::LastSeen, Qt::Horizontal, "Last Seen" );
    tblModel->setHeaderData( *UserCols::BanDate, Qt::Horizontal, "Ban Date" );
    tblModel->setHeaderData( *UserCols::IPAddr, Qt::Horizontal, "Last IP" );
    tblModel->setHeaderData( *UserCols::Banned, Qt::Horizontal, "Banned" );
    tblModel->setHeaderData( *UserCols::SerNum, Qt::Horizontal, "SerNum" );
    tblModel->setHeaderData( *UserCols::Muted, Qt::Horizontal, "Muted" );
    //tblModel->setHeaderData( *UserCols::Pings, Qt::Horizontal, "Pings" );
    tblModel->setHeaderData( *UserCols::Calls, Qt::Horizontal, "Calls" );
    tblModel->setHeaderData( *UserCols::Rank, Qt::Horizontal, "Rank" );

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
        Settings::setSetting( this->saveGeometry(), SKeys::Positions, "User" );

    userData->sync();
    userData->deleteLater();
    delete ui;
}

User* User::getInstance()
{
    if ( instance == nullptr )
        instance = new User( ReMix::getInstance() );

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
    bool ok;
    QString item{ QInputDialog::getItem( parent, "ReMix", "Punishment Duration:", punishDurations.values(), 0, false, &ok) };
    if ( ok && !item.isEmpty() )
        return punishDurations.key( item, PunishDurations::Invalid );

    return PunishDurations::Invalid;
}

GMRanks User::requestRank(QWidget* parent)
{
    bool ok;
    const QString item{ QInputDialog::getItem( parent, "ReMix", "Admin Rank:", adminRanks.values(), 0, false, &ok) };
    if ( ok && !item.isEmpty() )
        return adminRanks.key( item, GMRanks::User );

    return GMRanks::Invalid;
}

QSettings* User::getUserData()
{
    if ( userData == nullptr )
        userData = new QSettings( "userInfo.ini", QSettings::IniFormat );

    return userData;
}

//Public Functions
void User::setData(const QString& key, const QString& subKey, const QVariant& value)
{
    userData->sync();
    userData->setValue( key % "/" % subKey, value );
    userData->sync();
}

QVariant User::getData(const QString& key, const QString& subKey)
{
    userData->sync();

    if ( subKey == User::uKeys.value( UKeys::Rank ) )
        return userData->value( key % "/" % subKey, 0 );

    return userData->value( key % "/" % subKey );
}

bool User::makeAdmin(const QString& sernum, const QString& pwd)
{
    GMRanks rank{ static_cast<GMRanks>( getAdminRank( sernum ) ) };

    if ( !sernum.isEmpty()
      && !pwd.isEmpty() )
    {
        QString salt{ genPwdSalt( *Globals::SALT_LENGTH ) };
        QString hash{ salt + pwd };
                hash = Helper::hashPassword( hash );

        if ( rank == GMRanks::User )
        {
            setData( sernum, User::uKeys.value( UKeys::Rank ), *GMRanks::GMaster );

            User* user{ User::getInstance() };
            QModelIndex index{ user->findModelIndex( Helper::serNumToIntStr( sernum, true ), UserCols::SerNum ) };
            if ( index.isValid() )
                user->updateRowData( index.row(), *UserCols::Rank, *GMRanks::GMaster );
        }

        setData( sernum, User::uKeys.value( UKeys::Hash ), hash );
        setData( sernum, User::uKeys.value( UKeys::Salt ), salt );
        return true;
    }
    return false;
}

bool User::getIsAdmin(const QString& sernum)
{
    return getData( sernum, User::uKeys.value( UKeys::Rank ) ).toInt() >= 1;
}

bool User::getHasPassword(const QString& sernum)
{
    QString pwd{ getData( sernum, User::uKeys.value( UKeys::Hash ) ).toString() };

    return pwd.length() > 0;
}

bool User::cmpAdminPwd(const QString& sernum, const QString& value)
{
    const QString recSalt{ getData( sernum, User::uKeys.value( UKeys::Salt ) ).toString() };
    const QString recHash{ getData( sernum, User::uKeys.value( UKeys::Hash ) ).toString() };
    const QString hash{ Helper::hashPassword( recSalt % value ) };

    return ( hash == recHash );
}

qint32 User::getAdminRank(const QString& sernum)
{
    return getData( sernum, User::uKeys.value( UKeys::Rank ) ).toInt();
}

void User::setAdminRank(const QString& sernum, const GMRanks& newRank)
{
    User* user{ User::getInstance() };
    QModelIndex index{ user->findModelIndex( sernum, UserCols::SerNum ) };

    const GMRanks currentRank{ static_cast<GMRanks>( getAdminRank( sernum ) ) };
    if ( newRank == GMRanks::User )
    {
        static const QString title{ "Remove Password:" };
        static const QString promptTxt{ "Do you wish to remove this Admin's password information?" };

        bool hasPassword{ getHasPassword( sernum ) };
        if ( ( currentRank != newRank )
          && hasPassword )
        {
            bool removePassword{ Helper::confirmAction( User::getInstance(), title, promptTxt ) };
            if ( removePassword )
            {
                setData( sernum, User::uKeys.value( UKeys::Hash ), "" );
                setData( sernum, User::uKeys.value( UKeys::Salt ), "" );
            }
        }
    }

    setData( sernum, User::uKeys.value( UKeys::Rank ), *newRank );

    if ( index.isValid() )
        user->updateRowData( index.row(), *UserCols::Rank, *newRank );
}

void User::setAdminRank(QSharedPointer<Player> plr, const GMRanks& rank, const bool&)
{
    if ( plr != nullptr )
    {
        setAdminRank( plr->getSernumHex_s(), rank );
        plr->setAdminRank( rank );
    }
}

quint64 User::getIsPunished(const PunishTypes& punishType, const QString& value, const PunishTypes& type)
{
    if ( value.isEmpty() )
        return false;

    QStringList sernums{ userData->childGroups() };
    QString sernum{ "" };
    QString var{ "" };

    bool isValue{ false };

    for ( int i = 0; i < sernums.count(); ++i )
    {
        sernum = sernums.at( i );
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
                var = getData( sernum, User::uKeys.value( UKeys::IP ) ).toString();
                if ( Helper::cmpStrings( var, value ) )
                {
                    isValue = true;
                    break;
                }
            }
            break;
            case PunishTypes::DV:
            case PunishTypes::WV:
            case PunishTypes::Mute:
            case PunishTypes::Ban:
                break;
        }

        if ( isValue )
            break;
    }

    quint64 punishDuration{ getData( sernum, User::uKeys.value( UKeys::BanDuration ) ).toUInt() };
    QString punishReason{ getData( sernum, User::uKeys.value( UKeys::BanReason ) ).toString() };
    quint64 punishDate{ getData( sernum, User::uKeys.value( UKeys::Banned ) ).toUInt() };
    if ( punishType == PunishTypes::Mute )
    {
        punishDuration = getData( sernum, User::uKeys.value( UKeys::MuteDuration ) ).toUInt();
        punishReason = getData( sernum, User::uKeys.value( UKeys::BanReason ) ).toString();
        punishDate = getData( sernum, User::uKeys.value( UKeys::Muted ) ).toUInt();
    }

    if ( punishDate > 0 )
    {
        quint64 date{ static_cast<quint64>( QDateTime::currentDateTimeUtc().toSecsSinceEpoch() ) };
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
    QList<QStandardItem*> list{ tblModel->findItems( value, Qt::MatchFixedString, *UserCols::IPAddr ) };
    if ( type == PunishTypes::SerNum )
        list = tblModel->findItems( Helper::serNumToIntStr( value, true ), Qt::MatchFixedString, *UserCols::SerNum );

    if ( list.isEmpty() )
        return;

    User* user{ User::getInstance() };
    for ( const QStandardItem* item : list )
    {
        QModelIndex index{ item->index() };
        if ( !index.isValid() )
            continue;

        QStringList sernums{ userData->childGroups() };
        QString sernum{ "" };
        QString ip{ "" };
        QString punishReason{ "" };
        quint64 punishDuration{ 0 };
        quint64 punishDate{ 0 };

        for ( int i = 0; i < sernums.count(); ++i )
        {
            sernum = sernums.at(i);
            if ( punishType == PunishTypes::Mute )
            {
                punishDuration = getData( sernum, User::uKeys.value( UKeys::MuteDuration ) ).toUInt();
                punishReason = getData( sernum, User::uKeys.value( UKeys::MuteReason ) ).toString();
                punishDate = getData( sernum, User::uKeys.value( UKeys::Muted ) ).toUInt();
            }
            else if ( punishType == PunishTypes::Ban )
            {
                punishDuration = getData( sernum, User::uKeys.value( UKeys::BanDuration ) ).toUInt();
                punishReason = getData( sernum, User::uKeys.value( UKeys::BanReason ) ).toString();
                punishDate = getData( sernum, User::uKeys.value( UKeys::Banned ) ).toUInt();
            }

            if ( type == PunishTypes::IP )
            {
                userData->sync();
                ip = userData->value( sernum % "/" % value ).toString();
            }

            if ( Helper::cmpStrings( sernum, value )
              || Helper::cmpStrings( ip, value ) )
            {
                userData->sync();
                if ( punishType == PunishTypes::Ban )
                {
                    userData->remove( sernum % "/" % User::uKeys.value( UKeys::Banned ) );
                    userData->remove( sernum % "/" % User::uKeys.value( UKeys::BanReason ) );
                    userData->remove( sernum % "/" % User::uKeys.value( UKeys::BanDuration ) );
                    user->updateRowData( index.row(), *UserCols::Banned, false );
                    user->updateRowData( index.row(), *UserCols::BanReason, "" );
                    user->updateRowData( index.row(), *UserCols::BanDate, 0 );
                    user->updateRowData( index.row(), *UserCols::BanDuration, 0 );
                }
                else if ( punishType == PunishTypes::Mute )
                {
                    userData->remove( sernum % "/" % User::uKeys.value( UKeys::Muted ) );
                    userData->remove( sernum % "/" % User::uKeys.value( UKeys::MuteReason ) );
                    userData->remove( sernum % "/" % User::uKeys.value( UKeys::MuteDuration ) );
                    user->updateRowData( index.row(), *UserCols::Muted, false );
                    user->updateRowData( index.row(), *UserCols::MuteDate, 0 );
                    user->updateRowData( index.row(), *UserCols::MuteDuration, 0 );
                    user->updateRowData( index.row(), *UserCols::MuteReason, "" );
                }
                logPunishmentRemoval( sernum, punishDate, punishDuration, punishReason, punishType );
            }
        }
    }
}

void User::logPunishmentRemoval(const QString& sernum, const quint64& punishDate, const quint64& punishDuration,
                                const QString& punishReason, const PunishTypes& type)
{
    User* object{ User::getInstance() };
    if ( object != nullptr )
    {
        QString message{ "Removing the %1 User [ %2 ]. %1 on [ %3 ] until [ %4 ]; With the reason [ %5 ]." };
        QString banned{ "Banned" };
        QString muted{ "Muted" };
        QString typeFill{ "" };

        if ( type == PunishTypes::Ban )
            typeFill = banned;
        else
            typeFill = muted;

        message = message.arg( typeFill )
                         .arg( Helper::serNumToIntStr( sernum, true ) )
                         .arg( Helper::getTimeAsString( punishDate ) )
                         .arg( Helper::getTimeAsString( punishDuration ) )
                         .arg( punishReason );
        emit object->insertLogSignal( "User", message, LKeys::PunishmentLog, true, true );
    }
}

bool User::addBan(QSharedPointer<Player> admin, QSharedPointer<Player> target, const QString& reason, const bool remote, const PunishDurations duration)
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
    return addBanImpl( target, msg, duration );
}

bool User::addBan(QSharedPointer<Player> target, const QString& reason, const PunishDurations& duration)
{
    QString msg{ reason };
    if ( msg.isEmpty() )
    {
        msg = "Manual-Banish; Unknown reason: [ %1 ]";
        msg = msg.arg( target->getSernum_s() );
    }
    return addBanImpl( target, msg, duration );
}

bool User::addBanImpl(QSharedPointer<Player> target, const QString& reason, const PunishDurations& duration)
{
    User* user{ User::getInstance() };
    if ( user == nullptr )
        return false;

    QString serNum{ target->getSernumHex_s() };

    if ( !serNum.isEmpty() )
    {
        quint64 date{ static_cast<quint64>( QDateTime::currentDateTimeUtc().toSecsSinceEpoch() ) };
        quint64 banDuration{ date + *duration };

        setData( serNum, User::uKeys.value( UKeys::BanReason ), reason );
        setData( serNum, User::uKeys.value( UKeys::BanDuration ), banDuration );
        setData( serNum, User::uKeys.value( UKeys::Banned ), date );

        QModelIndex index{ user->findModelIndex( serNum, UserCols::SerNum ) };
        if ( index.isValid() )
        {
            user->updateRowData( index.row(), *UserCols::BanReason, reason );
            user->updateRowData( index.row(), *UserCols::BanDate, date );
            user->updateRowData( index.row(), *UserCols::Banned, ( date > 0 ) );
            user->updateRowData( index.row(), *UserCols::BanDuration, banDuration );
        }
    }
    return true;
}

bool User::addMute(QSharedPointer<Player> admin, QSharedPointer<Player> target, const QString& reason, const bool& remote, const bool& autoMute,
                   const PunishDurations& duration)
{
    User* user{ User::getInstance() };
    if ( user == nullptr )
        return false;

    if ( target == nullptr )
        return false;

    QString msg{ reason };
    if ( msg.isEmpty() )
    {
        if ( autoMute == true )
        {
            msg = "Automatic-Mute; Unknown reason: [ %1 ]";
            msg = msg.arg( target->getSernum_s() );
        }
        else
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
        }
    }
    return addMuteImpl( target, msg, duration );
}

bool User::addMute(QSharedPointer<Player> target, const QString& reason, const bool& remote, const PunishDurations& duration)
{
    QString msg{ reason };
    if ( msg.isEmpty() )
    {
        if ( remote == true )
        {
            msg = "Automatic-Mute; Unknown reason: [ %1 ]";
            msg = msg.arg( target->getSernum_s() );
        }
        else
        {
            msg = "Manual-Mute; Unknown reason: [ %1 ]";
            msg = msg.arg( target->getSernum_s() );
        }
    }
    return addMuteImpl( target, msg, duration );
}

bool User::addMuteImpl(QSharedPointer<Player> target, const QString& reason, const PunishDurations& duration)
{
    User* user{ User::getInstance() };
    if ( user == nullptr )
        return false;

    quint64 date{ static_cast<quint64>( QDateTime::currentDateTimeUtc().toSecsSinceEpoch() ) };
    quint64 muteDuration{ date + *duration };

    QString serNum{ target->getSernumHex_s() };
    if ( target->getSernum_i() == 0 )
        serNum = "00000000"; //Special case.

    emit getInstance()->mutedSerNumDurationSignal( serNum, muteDuration, reason );

    setData( serNum, User::uKeys.value( UKeys::MuteReason ), reason );
    setData( serNum, User::uKeys.value( UKeys::MuteDuration ), muteDuration );
    setData( serNum, User::uKeys.value( UKeys::Muted ), date );

    QModelIndex index{ user->findModelIndex( serNum, UserCols::SerNum ) };
    if ( index.isValid() )
    {
        user->updateRowData( index.row(), *UserCols::MuteReason, reason );
        user->updateRowData( index.row(), *UserCols::Muted, ( date > 0 ) );
        user->updateRowData( index.row(), *UserCols::MuteDate, date );
        user->updateRowData( index.row(), *UserCols::MuteDuration, muteDuration );

        User* object{ User::getInstance() };
        if ( object != nullptr )
        {
            QString message{ "Adding a Mute for User [ %1 ]. Muted on [ %2 ] until [ %3 ]; With the reason [ %4 ]." };
                    message = message.arg( Helper::serNumToIntStr( serNum, true ) )
                                     .arg( Helper::getTimeAsString( date ) )
                                     .arg( Helper::getTimeAsString( muteDuration ) )
                                     .arg( reason );
            emit object->insertLogSignal( "User", message, LKeys::PunishmentLog, true, true );
        }
    }
    return true;
}

QString User::getMuteReason(const QString& serNum)
{
    return getData( User::uKeys.value( UKeys::MuteReason ), serNum ).toString();
}

void User::updateCallCount(const QString& serNum)
{
    quint32 callCount{ getData( serNum, User::uKeys.value( UKeys::Calls ) ).toUInt() + 1 };
    User* user{ User::getInstance() };

    setData( serNum, User::uKeys.value( UKeys::Calls ), callCount );

    QModelIndex index{ user->findModelIndex( serNum, UserCols::SerNum ) };
    if ( index.isValid() )
        user->updateRowData( index.row(), *UserCols::Calls, callCount );
}

void User::logBIOSlot(const QString& serNum, const QHostAddress& ip, const QString& bio)
{
    User* user{ User::getInstance() };
    QString sernum{ serNum };
    if ( Helper::strContainsStr( sernum, "SOUL" ) )
        sernum = Helper::serNumToHexStr( serNum, IntFills::QuadWord );

    //quint32 pings{ getData( sernum, keys[ UserKeys::kPINGS ] ).toUInt() + 1 };
    quint64 date{ static_cast<quint64>( QDateTime::currentDateTimeUtc().toSecsSinceEpoch() ) };
    QString ip_s{ ip.toString() };

    setData( sernum, User::uKeys.value( UKeys::Bio ), bio.mid( 1 ) );
    //setData( sernum, keys[ UserKeys::kPINGS ], pings );
    setData( sernum, User::uKeys.value( UKeys::IP ), ip_s );
    setData( sernum, User::uKeys.value( UKeys::Seen ), date );

    QModelIndex index{ user->findModelIndex( sernum, UserCols::SerNum ) };
    if ( !index.isValid() )
    {
        qint32 row{ tblModel->rowCount() };
        tblModel->insertRow( row );
        index = tblModel->index(row, *UserCols::SerNum);
    }
    user->updateRowData( index.row(), *UserCols::SerNum, Helper::serNumToIntStr( sernum, true) );
    user->updateRowData( index.row(), *UserCols::IPAddr, ip_s );
    user->updateRowData( index.row(), *UserCols::LastSeen, date );
}

QByteArray User::getBIOData(const QString& sernum)
{
    return getData( sernum, User::uKeys.value( UKeys::Bio ) ).toByteArray();
}

bool User::validateSalt(const QString& salt)
{
    QStringList groups{ userData->childGroups() };
    QString value{ "" };

    for ( const QString& group : groups )
    {
        value = userData->value( group % "/salt" ).toString();
        if ( value == salt )
            return false;
    }
    return true;
}

QString User::genPwdSalt(const qint32& length)
{
    QString salt{ "" };
    static const QString charList
    {
        "0123456789 `~!@#$%^&*-_=+{([])}|;:'\"\\,./?<>"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz"
    };

    for ( qint32 i = 0; i < length; ++i )
    {
        qint32 chrPos{ RandDev::getInstance().getGen( 0, static_cast<int>( charList.length() - 1 ) ) };
        salt.append( charList.at( chrPos ) );
    }

    if ( !validateSalt( salt ) )
        salt = genPwdSalt( length );

    return salt;
}

//Private Functions.
QModelIndex User::findModelIndex(const QString& value, const UserCols& col)
{
    QString serNum{ value };
    if ( col == UserCols::SerNum )
        serNum = Helper::serNumToIntStr( value, true );

    QList<QStandardItem*> list{ tblModel->findItems( serNum, Qt::MatchExactly, *col ) };
    QModelIndex index;
    if ( !list.isEmpty() )
    {
        QStandardItem* item{ list.value( 0, nullptr ) };
        if ( item != nullptr )
        {
            index = item->index();
            if ( index.isValid() )
                return index;
        }
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

        for ( int i = 0; i < sernums.count(); ++i )
        {
            const QString sernum{ sernums.at( i ) };
            const QString muteReason{ getData( sernum, User::uKeys.value( UKeys::MuteReason ) ).toString() };
            const QString banReason{ getData( sernum, User::uKeys.value( UKeys::BanReason ) ).toString() };
            const QString ip{ getData( sernum, User::uKeys.value( UKeys::IP ) ).toString() };

            const quint64 muteDuration_i{ getData( sernum, User::uKeys.value( UKeys::MuteDuration ) ).toUInt() };
            const quint64 banDuration_i{ getData( sernum, User::uKeys.value( UKeys::BanDuration ) ).toUInt() };
            const quint64 muteDate_i{ getData( sernum, User::uKeys.value( UKeys::Muted ) ).toUInt() };
            const quint64 banDate_i{ getData( sernum, User::uKeys.value( UKeys::Banned ) ).toUInt() };
            const quint32 calls_i{ getData( sernum, User::uKeys.value( UKeys::Calls ) ).toUInt() };
            const quint64 seen_i{ getData( sernum, User::uKeys.value( UKeys::Seen ) ).toUInt() };
            const quint32 rank{ getData( sernum, User::uKeys.value( UKeys::Rank ) ).toUInt() };

            bool banned{ banDate_i > 0 };
            bool muted{ muteDate_i > 0 };

            int row{ tblModel->rowCount() };
            tblModel->insertRow( row );

            tblProxy->rowCount();
            tblModel->setData( tblModel->index( row, 0 ), Helper::serNumToIntStr( sernum, true ), Qt::DisplayRole );

            this->updateRowData( row, *UserCols::MuteDuration, muteDuration_i );

            this->updateRowData( row, *UserCols::BanDuration, banDuration_i );
            this->updateRowData( row, *UserCols::MuteReason, muteReason );
            this->updateRowData( row, *UserCols::BanReason, banReason );
            this->updateRowData( row, *UserCols::MuteDate, muteDate_i );
            this->updateRowData( row, *UserCols::BanDate, banDate_i );
            this->updateRowData( row, *UserCols::LastSeen, seen_i );
            this->updateRowData( row, *UserCols::Banned, banned );
            //this->updateRowData( row, *UserCols::Pings, pings_i );
            this->updateRowData( row, *UserCols::Calls, calls_i );
            this->updateRowData( row, *UserCols::Muted, muted );
            this->updateRowData( row, *UserCols::IPAddr, ip );
            this->updateRowData( row, *UserCols::Rank, rank );

            ui->userTable->resizeColumnToContents( *UserCols::BanReason );

            getIsPunished( PunishTypes::SerNum, sernum, PunishTypes::Ban );
            getIsPunished( PunishTypes::SerNum, sernum, PunishTypes::Mute );
        }
    }

    this->setWindowTitle( title );
    ui->userTable->selectRow( 0 );
    ui->userTable->resizeColumnsToContents();
}

void User::updateRowData(const qint32& row, const qint32& col, const QVariant& data)
{
    qDebug() << "updateRowData";
    QModelIndex index{ tblModel->index( row, col ) };
    if ( !index.isValid() )
        return;

    switch ( static_cast<UserCols>( col ) )
    {
        case UserCols::LastSeen:
        case UserCols::BanDate:
        case UserCols::BanDuration:
        case UserCols::MuteDate:
        case UserCols::MuteDuration:
        {
            uint date{ data.toUInt() };

            //Place the Date into the UserRole. Accessing this value is more convenient than the date string.
            tblModel->setData( index, date, Qt::UserRole );

            QString msg{ "" };
            if ( date > 0 )
                msg = Helper::getTimeAsString( date );
            else
                msg = ( ( static_cast<UserCols>( col ) == UserCols::LastSeen ) ? "Never Seen" : "" );

            tblModel->setData( index, msg, Qt::DisplayRole );
            ui->userTable->resizeColumnToContents( col );
            break;
        }
        case UserCols::BanReason:
        case UserCols::MuteReason:
        {
            ui->userTable->resizeColumnToContents( col );
            break;
        }
        case UserCols::Muted:
        {
            bool muted{ tblModel->data( index ).toBool() };
            if ( data.toBool() == muted )
                return;

            tblModel->setData( index, data, Qt::DisplayRole );
            break;
        }
        default:
        {
            tblModel->setData( index, data, Qt::DisplayRole );
            break;
        }
    }
}

//Private Slots.
void User::updateDataValueSlot(const QModelIndex& index, const QModelIndex&, const QVector<int>&)
{
    QString sernum{ "" };
    QVariant value;

    sernum = tblModel->data( tblModel->index( index.row(), *UserCols::SerNum ) ).toString();
    sernum = Helper::sanitizeSerNum( sernum );

    switch ( static_cast<UserCols>( index.column() ) )
    {
        case UserCols::Rank:
            {
                GMRanks rank{ static_cast<GMRanks>( tblModel->data( index ).toInt() ) };
                setAdminRank( sernum, rank );

                emit this->setAdminRankSignal( sernum, rank );
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
                    reason = getData( sernum, User::uKeys.value( UKeys::BanReason ) ).toString();
                    if ( reason.isEmpty() )
                    {
                        banDate = static_cast<quint64>( QDateTime::currentDateTimeUtc().toSecsSinceEpoch() );
                        setData( sernum, User::uKeys.value( UKeys::Banned ), banDate );
                        this->updateRowData( index.row(), *UserCols::BanDate, banDate );

                        setReason = true;
                        reason = "Manual Banish; " % requestReason( this );

                        banDuration = banDate + *requestDuration( this );

                        setData( sernum, User::uKeys.value( UKeys::BanDuration ), banDuration );
                        this->updateRowData( index.row(), *UserCols::BanDuration, banDuration );
                    }
                }
                else
                {
                    setReason = true;
                    reason.clear();

                    setData( sernum, User::uKeys.value( UKeys::Banned ), banDate );
                    this->updateRowData( index.row(), *UserCols::BanDate, banDate );

                    setData( sernum, User::uKeys.value( UKeys::BanDuration ), banDuration );
                    this->updateRowData( index.row(), *UserCols::BanDuration, banDuration );
                }

                if ( setReason )
                {
                    setData( sernum, User::uKeys.value( UKeys::BanReason ), reason );
                    this->updateRowData( index.row(), *UserCols::BanReason, reason );
                }
            }
        break;
        case UserCols::Muted:
            {
                bool setMutes{ false };
                quint64 muteDuration{ *PunishDurations::Invalid };
                quint64 muteDate{ *PunishDurations::Invalid };

                QString inform{ "The Server Host has %1 you. %2" };
                QString reason{ "Reason: Manual %1; %2" };
                QString type{ "" };

                bool muted{ tblModel->data( index ).toBool() };
                if ( muted )
                {
                    QString tempReason{ getData( sernum, User::uKeys.value( UKeys::MuteReason ) ).toString() };
                    if ( tempReason.isEmpty() )
                    {
                        setMutes = true;
                        reason = reason.arg( "Mute" )
                                       .arg( requestReason( this ) );

                        type = "Muted";

                        muteDate = static_cast<quint64>( QDateTime::currentDateTimeUtc().toSecsSinceEpoch() );
                        muteDuration = muteDate + *requestDuration( this );
                    }
                }
                else
                {
                    setMutes = true;
                    reason = reason.arg( "Un-Mute" )
                                   .arg( "" );

                    type = "Un-Muted";
                }

                if ( setMutes )
                {
                    setData( sernum, User::uKeys.value( UKeys::Muted ), muteDate );
                    this->updateRowData( index.row(), *UserCols::MuteDate, muteDate );

                    setData( sernum, User::uKeys.value( UKeys::MuteDuration ), muteDuration );
                    this->updateRowData( index.row(), *UserCols::MuteDuration, muteDuration );

                    if ( muteDuration == 0 )
                        reason.clear();

                    inform = inform.arg( type )
                                   .arg( reason );

                    setData( sernum, User::uKeys.value( UKeys::MuteReason ), reason );
                    this->updateRowData( index.row(), *UserCols::MuteReason, reason );

                    emit this->mutedSerNumDurationSignal( sernum, muteDuration, inform );
                }
            }
        break;
        default:
        break;
    }
}
