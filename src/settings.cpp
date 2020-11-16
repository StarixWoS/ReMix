
//Class includes.
#include "settings.hpp"
#include "ui_settings.h"

//ReMix Widget includes.
#include "widgets/settingswidget.hpp"
#include "widgets/ruleswidget.hpp"
#include "widgets/motdwidget.hpp"

//ReMix includes.
#include "serverinfo.hpp"
#include "randdev.hpp"
#include "helper.hpp"

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

void Settings::updateTabBar(ServerInfo* server)
{
    qint32 index{ tabWidget->currentIndex() };

    tabWidget->clear();

    getInstance()->setWindowTitle( "[ " % server->getServerName() % " ] Settings:");
    tabWidget->insertTab( 0, SettingsWidget::getInstance(), "Settings" );
    tabWidget->insertTab( 1, RulesWidget::getWidget( server ), "Rules" );
    tabWidget->insertTab( 2, MOTDWidget::getWidget( server ), "MotD" );

    tabWidget->setCurrentIndex( index );
}

void Settings::copyServerSettings(ServerInfo* server, const QString& newName)
{
    //QMutexLocker locker( &mutex );
    QString oldName{ server->getServerName() };
    QVariant val;
    if ( oldName != newName )
    {
        //Copy Rules.
        val = getSetting( SKeys::Rules, SSubKeys::HasSvrPassword, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::HasSvrPassword, newName );
        val = getSetting( SKeys::Rules, SSubKeys::NoEavesdrop, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::NoEavesdrop, newName );
        val = getSetting( SKeys::Rules, SSubKeys::SvrPassword, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::SvrPassword, newName );
        val = getSetting( SKeys::Rules, SSubKeys::AutoRestart, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::AutoRestart, newName );
        val = getSetting( SKeys::Rules, SSubKeys::MinVersion, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::MinVersion, newName );
        val = getSetting( SKeys::Rules, SSubKeys::MaxPlayers, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::MaxPlayers, newName );
        val = getSetting( SKeys::Rules, SSubKeys::NoMigrate, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::NoMigrate, newName );
        val = getSetting( SKeys::Rules, SSubKeys::NoModding, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::NoModding, newName );
        val = getSetting( SKeys::Rules, SSubKeys::PKLadder, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::PKLadder, newName );
        val = getSetting( SKeys::Rules, SSubKeys::ArenaPK, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::ArenaPK, newName );
        val = getSetting( SKeys::Rules, SSubKeys::NoCheat, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::NoCheat, newName );
        val = getSetting( SKeys::Rules, SSubKeys::NoBleep, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::NoBleep, newName );
        val = getSetting( SKeys::Rules, SSubKeys::SvrUrl, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::SvrUrl, newName );
        val = getSetting( SKeys::Rules, SSubKeys::NoPets, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::NoPets, newName );
        val = getSetting( SKeys::Rules, SSubKeys::MaxIdle, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::MaxIdle, newName );
        val = getSetting( SKeys::Rules, SSubKeys::World, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::World, newName );
        val = getSetting( SKeys::Rules, SSubKeys::AllPK, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::AllPK, newName );
        val = getSetting( SKeys::Rules, SSubKeys::NoPK, oldName );
              setSetting( val, SKeys::Rules, SSubKeys::NoPK, newName );

        //Copy Settings.
        val = getSetting( SKeys::Setting, SSubKeys::PortNumber, oldName );
              setSetting( val, SKeys::Setting, SSubKeys::PortNumber, newName );
        val = getSetting( SKeys::Setting, SSubKeys::IsRunning, oldName );
              setSetting( val, SKeys::Setting, SSubKeys::IsRunning, newName );
        val = getServerID( oldName );
              setSetting( val, SKeys::Setting, SSubKeys::Extension, newName );
        val = getSetting( SKeys::Setting, SSubKeys::IsPublic, oldName );
              setSetting( val, SKeys::Setting, SSubKeys::IsPublic, newName );
        val = getSetting( SKeys::Setting, SSubKeys::GameName, oldName );
              setSetting( val, SKeys::Setting, SSubKeys::GameName, newName );
        val = getSetting( SKeys::Setting, SSubKeys::UseUPNP, oldName );
              setSetting( val, SKeys::Setting, SSubKeys::UseUPNP, newName );
        val = getSetting( SKeys::Setting, SSubKeys::MOTD, oldName );
              setSetting( val, SKeys::Setting, SSubKeys::MOTD, oldName );

        prefs->remove( oldName );
        prefs->sync();
    }
}

//Static-Free Functions.
void Settings::setSettingFromPath(const QString& path, const QVariant& value)
{
    QMutexLocker locker( &mutex );

    prefs->sync();
    if ( !canRemoveSetting( value ) )
        prefs->setValue( path, value );
    else
        removeSetting( path );

    prefs->sync();
}

QVariant Settings::getSettingFromPath(const QString& path)
{
    QMutexLocker locker( &mutex );
    prefs->sync();
    return prefs->value( path );
}

QString Settings::makeSettingPath(const SKeys& key, const SSubKeys& subKey, const QVariant& childSubKey)
{
    QMutexLocker locker( &mutex );
    QString path{ "%1/%2/%3" };
            path = path.arg( childSubKey.toString() )
                       .arg( Settings::pKeys[ static_cast<int>( key ) ] )
                       .arg( sKeys[ static_cast<int>( subKey ) ] );
    return path;
}

QString Settings::makeSettingPath(const SKeys& key, const SSubKeys& subKey)
{
    QMutexLocker locker( &mutex );
    QString path{ "%1/%2" };
            path = path.arg( Settings::pKeys[ static_cast<int>( key ) ] )
                       .arg( sKeys[ static_cast<int>( subKey ) ] );
    return path;
}

QString Settings::makeSettingPath(const SKeys& key, const QVariant& subKey)
{
    QMutexLocker locker( &mutex );
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
    QMutexLocker locker( &mutex );
    prefs->sync();
    prefs->beginGroup( svrID % "/" % pKeys[ static_cast<int>( SKeys::Rules ) ] );

    QStringList ruleList{ prefs->allKeys() };
    QString rules{ "" };

    bool valIsBool{ false };

    QVariant value;
    for ( const QString& rule : ruleList )
    {
        value = prefs->value( rule );

        valIsBool = false;

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
    QMutexLocker locker( &mutex );
    bioHash.insert( addr, value );
}

QByteArray Settings::getBioHashValue(const QHostAddress& addr)
{
    QMutexLocker locker( &mutex );
    return bioHash.value( addr );
}

QHostAddress Settings::getBioHashKey(const QByteArray& bio)
{
    QMutexLocker locker( &mutex );
    return bioHash.key( bio );
}
