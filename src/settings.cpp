
//Class includes.
#include "settings.hpp"
#include "ui_settings.h"

//ReMix Widget includes.
#include "widgets/settingswidget.hpp"
#include "widgets/ruleswidget.hpp"
#include "widgets/motdwidget.hpp"

//ReMix includes.
#include "randdev.hpp"
#include "helper.hpp"
#include "server.hpp"

//Qt Includes.
#include <QNetworkAccessManager>
#include <QTabWidget>
#include <QUdpSocket>
#include <QString>
#include <QObject>
#include <QtCore>

//Initialize Settings keys/subKeys lists
const QStringList Settings::pKeys =
{
    "Settings",
    "WrongIPs",
    "Messages",
    "Positions",
    "Rules",
    "Logger"
};

const QStringList Settings::sKeys =
{
    //Settings.
    "serverID",
    "autoBanish",
    "discIdle",
    "reqSerNums",
    "dupeOK",
    "supportsSSV",
    "banishDupeIP",
    "MOTD",
    "logComments",
    "fwdComments",
    "informAdminLogin",
    "echoComments",
    "minimizeToTray",
    "saveWindowPositions",
    "isRunning",
    "worldDir",
    "portNumber",
    "isPublic",
    "gameName",
    "logFiles",
    "darkMode",
    "useUPNP",
    "checkForUpdates",
    "dcBlueCodedSerNums",
    "loggerAutoScroll",
    "overrideMasterIP",
    "loggerAutoClear",
    "overrideMasterHost",
    "chatAutoScroll",
    "hidePlayerView",
    "hideChatView",

    //Rules.
    "hasSvrPassword",
    "svrPassword",
    "world",
    "url",
    "allPK",
    "maxP",
    "maxIdle",
    "minV",
    "ladder",
    "noBleep",
    "noCheat",
    "noEavesdrop",
    "noMigrate",
    "noMod",
    "noPets",
    "noPK",
    "arenaPK",
    "autoRestart",
};

QVector<SSubKeys> Settings::serverSettings //Settings Specific to a Server Instance.
{
    SSubKeys::ChatAutoScroll,
    SSubKeys::HidePlayerView,
    SSubKeys::HideChatView,
    SSubKeys::PortNumber,
    SSubKeys::IsRunning,
    SSubKeys::IsPublic,
    SSubKeys::GameName,
    SSubKeys::UseUPNP,
    SSubKeys::MOTD,
};

QVector<SSubKeys> Settings::serverRules //Rules Specific to a Server Instance.
{
    SSubKeys::HasSvrPassword,
    SSubKeys::NoEavesdrop,
    SSubKeys::SvrPassword,
    SSubKeys::AutoRestart,
    SSubKeys::MinVersion,
    SSubKeys::MaxPlayers,
    SSubKeys::NoMigrate,
    SSubKeys::NoModding,
    SSubKeys::PKLadder,
    SSubKeys::ArenaPK,
    SSubKeys::NoCheat,
    SSubKeys::NoBleep,
    SSubKeys::SvrUrl,
    SSubKeys::NoPets,
    SSubKeys::MaxIdle,
    SSubKeys::World,
    SSubKeys::AllPK,
    SSubKeys::NoPK,
};

//The BIO Hash is accessible via any ReMix Server Instance.
QHash<QHostAddress, QByteArray> Settings::bioHash;

//Initialize our QSettings Object globally to make things more responsive.
QSettings* Settings::prefs{ new QSettings( "preferences.ini", QSettings::IniFormat ) };
QTabWidget* Settings::tabWidget;
Settings* Settings::instance;
QMutex Settings::mutex;

Settings::Settings(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    //Setup Objects.
    tabWidget = new QTabWidget( this );
    if ( tabWidget != nullptr )
    {
        tabWidget->insertTab( 0, SettingsWidget::getInstance( this ), "Settings" );

        ui->widget->setLayout( new QGridLayout( ui->widget ) );
        ui->widget->layout()->setContentsMargins( 5, 5, 5, 5 );
        ui->widget->layout()->addWidget( tabWidget );
    }

    if ( getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        this->restoreGeometry( Settings::getSetting( SKeys::Positions, this->metaObject()->className() ).toByteArray() );
}

Settings::~Settings()
{
    if ( getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        setSetting( this->saveGeometry(), SKeys::Positions, this->metaObject()->className() );

    tabWidget->deleteLater();

    instance->close();
    instance->deleteLater();

    prefs->sync();
    prefs->deleteLater();

    delete ui;
}

Settings* Settings::getInstance()
{
    if ( instance == nullptr )
        instance = new Settings( nullptr );

    return instance;
}

void Settings::setInstance(Settings* value)
{
    instance = value;
}

void Settings::updateTabBar(Server* server)
{
    qint32 index{ tabWidget->currentIndex() };

    tabWidget->clear();

    getInstance()->setWindowTitle( "[ " % server->getServerName() % " ] Settings:");
    tabWidget->insertTab( 0, SettingsWidget::getInstance(), "Settings" );
    tabWidget->insertTab( 1, RulesWidget::getInstance( server ), "Rules" );
    tabWidget->insertTab( 2, MOTDWidget::getInstance( server ), "MotD" );

    tabWidget->setCurrentIndex( index );
}

void Settings::copyServerSettings(Server* server, const QString& newName)
{
    QString oldName{ server->getServerName() };
    if ( oldName != newName )
    {
        prefs->sync();

        //Copy Server Rules.
        for ( const SSubKeys& key : serverRules )
        {
            QVariant val = getSetting( SKeys::Rules, key, oldName );
            setSetting( val, SKeys::Rules, key, newName );
        }

        //Copy Server Settings.
        for ( const SSubKeys& key : serverSettings )
        {
            QVariant val = getSetting( SKeys::Setting, key, oldName );
            setSetting( val, SKeys::Setting, key, newName );
        }

        prefs->remove( oldName );
        prefs->sync();
    }
}

//Static-Free Functions.
void Settings::setSettingFromPath(const QString& path, const QVariant& value)
{
    QMutexLocker<QMutex> locker( &mutex );

    prefs->sync();
    if ( !canRemoveSetting( value ) )
        prefs->setValue( path, value );
    else
        removeSetting( path );

    prefs->sync();
}

QVariant Settings::getSettingFromPath(const QString& path)
{
    QMutexLocker<QMutex> locker( &mutex );
    prefs->sync();
    return prefs->value( path );
}

QString Settings::makeSettingPath(const SKeys& key, const SSubKeys& subKey, const QVariant& childSubKey)
{
    QMutexLocker<QMutex> locker( &mutex );
    QString path{ "%1/%2/%3" };
            path = path.arg( childSubKey.toString() )
                       .arg( Settings::pKeys[ static_cast<int>( key ) ] )
                       .arg( sKeys[ static_cast<int>( subKey ) ] );
    return path;
}

QString Settings::makeSettingPath(const SKeys& key, const SSubKeys& subKey)
{
    QMutexLocker<QMutex> locker( &mutex );
    QString path{ "%1/%2" };
            path = path.arg( Settings::pKeys[ static_cast<int>( key ) ] )
                       .arg( sKeys[ static_cast<int>( subKey ) ] );
    return path;
}

QString Settings::makeSettingPath(const SKeys& key, const QVariant& subKey)
{
    QMutexLocker<QMutex> locker( &mutex );
    QString path{ "%1/%2" };
            path = path.arg( Settings::pKeys[ static_cast<int>( key ) ] )
                       .arg( subKey.toString() );
    return path;
}

bool Settings::canRemoveSetting(const QVariant& value)
{
    bool remove{ false };
    if ( !value.toBool()
      && ( value.toInt() == 0 )
      && ( Helper::cmpStrings( "false", value.toString() )
        || Helper::cmpStrings( "0", value.toString() )
        || value.toString().isEmpty() ) )
    {
        remove = true;  //Keys with a 'disabled' value will be removed from storage.
    }
    return remove;
}

void Settings::removeSetting(const QString& path)
{
    prefs->sync();
    prefs->remove( path );
    prefs->sync();
}

void Settings::setSetting(const QVariant& value, const SKeys& key, const SSubKeys& subKey, const QVariant& childSubKey)
{
    setSettingFromPath( makeSettingPath( key, subKey, childSubKey ), value );
}

void Settings::setSetting(const QVariant& value, const SKeys& key, const SSubKeys& subKey)
{
    setSettingFromPath( makeSettingPath( key, subKey ), value );
}

void Settings::setSetting(const QVariant& value, const SKeys& key, const QVariant& subKey)
{
    setSettingFromPath( makeSettingPath( key, subKey ), value );
}

QVariant Settings::getSetting(const SKeys& key, const SSubKeys& subKey, const QVariant& childSubKey)
{
    return getSettingFromPath( makeSettingPath( key, subKey, childSubKey ) );
}

QVariant Settings::getSetting(const SKeys& key, const SSubKeys& subKey)
{
    return getSettingFromPath( makeSettingPath( key, subKey ) );
}

QVariant Settings::getSetting(const SKeys& key, const QString& subKey)
{
    return getSettingFromPath( makeSettingPath( key, subKey ) );
}

//Retain function for ease of use.
QString Settings::getServerID(const QString& svrID)
{
    qint32 id{ getSetting( SKeys::Setting, SSubKeys::Extension, svrID ).toInt() };
    if ( id <= 0 )
    {
        RandDev* randDev{ RandDev::getDevice() };
        if ( randDev != nullptr )
            id = randDev->genRandNum( 1, 0x7FFFFFFE );

        setSetting( id, SKeys::Setting, SSubKeys::Extension, svrID );
        randDev = nullptr;
    }
    return Helper::intToStr( id, static_cast<int>( IntBase::HEX ), 8 );
}

QString Settings::getRuleSet(const QString& svrID)
{
    QMutexLocker<QMutex> locker( &mutex );
    prefs->sync();
    prefs->beginGroup( svrID % "/" % pKeys[ static_cast<int>( SKeys::Rules ) ] );

    QStringList ruleList{ prefs->allKeys() };
    QString rules{ "" };

    QVariant value;
    for ( const QString& rule : ruleList )
    {
        value = prefs->value( rule );

        bool valIsBool{ false };
        if ( Helper::cmpStrings( value.toString(), "true" ) )
            valIsBool = true;

        if ( !Helper::cmpStrings( rule, "svrPassword" ) )
        {
            rules += rule % "=";
            if ( valIsBool )
                rules = rules.append( QString::number( value.toBool() ) );
            else
                rules = rules.append( value.toString() );

            rules.append( ", " );
        }
    }

    prefs->endGroup();
    return rules;
}

//Retain function for ease of use.
bool Settings::cmpServerPassword(const QString& serverName, const QString& value)
{
    QString hash{ value };
    QVariant val{ getSetting( SKeys::Rules, SSubKeys::SvrPassword, serverName ) };
    if ( !hash.isEmpty() )
        return ( val.toString() == Helper::hashPassword( hash ) );

    return false;
}

void Settings::insertBioHash(const QHostAddress& addr, const QByteArray& value)
{
    QMutexLocker<QMutex> locker( &mutex );
    bioHash.insert( addr, value );
}

QByteArray Settings::getBioHashValue(const QHostAddress& addr)
{
    QMutexLocker<QMutex> locker( &mutex );
    return bioHash.value( addr );
}

QHostAddress Settings::getBioHashKey(const QByteArray& bio)
{
    QMutexLocker<QMutex> locker( &mutex );
    return bioHash.key( bio );
}
