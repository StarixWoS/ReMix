
//Class includes.
#include "settings.hpp"
#include "ui_settings.h"

//ReMix Widget includes.
#include "widgets/settingswidget.hpp"
#include "widgets/motdwidget.hpp"
#include "widgets/ruleswidget.hpp"

//ReMix includes.
#include "serverinfo.hpp"
#include "settings.hpp"
#include "randdev.hpp"
#include "helper.hpp"
#include "rules.hpp"
#include "settings.hpp"

//Qt Includes.
#include <QNetworkAccessManager>
#include <QTabWidget>
#include <QUdpSocket>
#include <QString>
#include <QObject>
#include <QtCore>

//Initialize Settings keys/subKeys lists
const QStringList Settings::keys =
{
    "Settings",
    "WrongIPs",
    "Messages",
    "Positions",
    "Rules",
    "Logger"
};

const QStringList Settings::subKeys =
{
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
    "loggerAutoScroll"
};

//Initialize our QSettings Object globally to make things more responsive.
QSettings* Settings::prefs{ new QSettings( "preferences.ini", QSettings::IniFormat ) };
SettingsWidget* Settings::settings;
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
        settings = new SettingsWidget( this );
        if ( settings != nullptr )
            tabWidget->insertTab( 0, settings, "Settings" );

        ui->widget->setLayout( new QGridLayout( ui->widget ) );
        ui->widget->layout()->setContentsMargins( 5, 5, 5, 5 );
        ui->widget->layout()->addWidget( tabWidget );
    }

    if ( getSetting( SettingKeys::Setting, SettingSubKeys::SaveWindowPositions ).toBool() )
    {
        QByteArray geometry{ getWindowPositions( this->metaObject()->className() ) };
        if ( !geometry.isEmpty() )
            this->restoreGeometry( getWindowPositions( this->metaObject()->className() ) );
    }
}

Settings::~Settings()
{
    if ( getSetting( SettingKeys::Setting, SettingSubKeys::SaveWindowPositions ).toBool() )
        setWindowPositions( this->saveGeometry(), this->metaObject()->className() );

    tabWidget->deleteLater();
    settings->deleteLater();

    instance->close();
    instance->deleteLater();

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
    if ( settings == nullptr )
        settings = new SettingsWidget( nullptr );

    getInstance()->setWindowTitle( "[ " % server->getServerName() % " ] Settings:");
    tabWidget->insertTab( 0, settings, "Settings" );
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
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::NoEavesdrop ), RuleKeys::NoEavesdrop );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::SvrPassword ), RuleKeys::SvrPassword );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::NoMigrate ), RuleKeys::NoMigrate );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::ArenaPK ), RuleKeys::ArenaPK );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::NoCheat ), RuleKeys::NoCheat );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::NoBleep ), RuleKeys::NoBleep );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::PKLadder ), RuleKeys::PKLadder );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::NoPets ), RuleKeys::NoPets );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::MaxAFK ), RuleKeys::MaxAFK );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::NoModding ), RuleKeys::NoModding );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::World ), RuleKeys::World );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::AllPK ), RuleKeys::AllPK );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::NoPK ), RuleKeys::NoPK );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::MinVersion ), RuleKeys::MinVersion );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::MaxPlayers ), RuleKeys::MaxPlayers );
        Rules::setRule( newName, Rules::getRule( oldName, RuleKeys::SvrUrl ), RuleKeys::SvrUrl );

        //Copy Settings.
        val = getSetting( SettingKeys::Setting, SettingSubKeys::IsRunning, oldName );
              setSetting( val, SettingKeys::Setting, SettingSubKeys::IsRunning, oldName );
        val = getSetting( SettingKeys::Setting, SettingSubKeys::MOTD, oldName );
              setSetting( val, SettingKeys::Setting, SettingSubKeys::MOTD, oldName );
        val = getSetting( SettingKeys::Setting, SettingSubKeys::PortNumber, oldName );
              setSetting( val, SettingKeys::Setting, SettingSubKeys::PortNumber, oldName );
        val = getSetting( SettingKeys::Setting, SettingSubKeys::IsPublic, oldName );
              setSetting( val, SettingKeys::Setting, SettingSubKeys::IsPublic, oldName );
        val = getSetting( SettingKeys::Setting, SettingSubKeys::GameName, oldName );
              setSetting( val, SettingKeys::Setting, SettingSubKeys::GameName, oldName );
        val = getSetting( SettingKeys::Setting, SettingSubKeys::UseUPNP, oldName );
              setSetting( val, SettingKeys::Setting, SettingSubKeys::UseUPNP, oldName );
        val = getServerID( oldName );
              setSetting( val, SettingKeys::Setting, SettingSubKeys::Extension, newName );

        prefs->remove( oldName );
        prefs->sync();
    }
}

//Static-Free Functions.
QString Settings::makeSettingPath(const SettingKeys& key, const SettingSubKeys& subKey)
{
    QString path{ "%1/%2" };
            path = path.arg( Settings::keys[ static_cast<int>( key ) ] )
                       .arg( subKeys[ static_cast<int>( subKey ) ] );
            return path;
}

QString Settings::makeSettingPath(const SettingKeys& key, const SettingSubKeys& subKey, const QVariant& childSubKey)
{
    QString path{ "%1/%2/%3" };
            path = path.arg( childSubKey.toString() )
                       .arg( Settings::keys[ static_cast<int>( key ) ] )
                       .arg( subKeys[ static_cast<int>( subKey ) ] );
            return path;
}

void Settings::removeSetting(const QString& path)
{
    prefs->sync();
    prefs->remove( path );
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

void Settings::setSetting(const QVariant& value, const SettingKeys& key, const SettingSubKeys& subKey)
{
    QMutexLocker locker( &mutex );
    QVariant val{ value };

    QString path{ makeSettingPath( key, subKey ) };
    if ( !canRemoveSetting( value ) )
        prefs->setValue( path, val );
    else
        removeSetting( path );

    prefs->sync();
}

void Settings::setSetting(const QVariant& value, const SettingKeys& key, const SettingSubKeys& subKey, const QVariant& childSubKey)
{
    QMutexLocker locker( &mutex );
    QVariant val{ value };

    QString path{ makeSettingPath( key, subKey, childSubKey ) };
    if ( !canRemoveSetting( value ) )
        prefs->setValue( path, val );
    else
        removeSetting( path );

    prefs->sync();
}

QVariant Settings::getSetting(const SettingKeys& key, const SettingSubKeys& subKey)
{
    QMutexLocker locker( &mutex );
    prefs->sync();
    return prefs->value( makeSettingPath( key, subKey ) );
}

QVariant Settings::getSetting(const SettingKeys& key, const SettingSubKeys& subKey, const QVariant& childSubKey)
{
    QMutexLocker locker( &mutex );
    prefs->sync();
    return prefs->value( makeSettingPath( key, subKey, childSubKey ) );
}

void Settings::setSetting(const QString& key, const QString& subKey, const QVariant& value)
{
    QMutexLocker locker( &mutex );
    prefs->setValue( key % "/" % subKey, value );
    prefs->sync();
}

QVariant Settings::getSetting(const QString& key, const QString& subKey)
{
    QMutexLocker locker( &mutex );
    prefs->sync();
    return prefs->value( key % "/" % subKey );
}

void Settings::setWindowPositions(const QByteArray& geometry, const char* dialog)
{
    setSetting( keys[ static_cast<int>( SettingKeys::Positions ) ], dialog, geometry );
}

QByteArray Settings::getWindowPositions(const char* dialog)
{
    return getSetting( keys[ static_cast<int>( SettingKeys::Positions ) ], dialog )
              .toByteArray();
}

void Settings::setIsInvalidIPAddress(const QString& value)
{
    setSetting( keys[ static_cast<int>( SettingKeys::WrongIP ) ], value, true );
}

bool Settings::getIsInvalidIPAddress(const QString& value)
{
    return getSetting( keys[ static_cast<int>( SettingKeys::WrongIP ) ], value )
              .toBool();
}

QString Settings::getServerID(const QString& svrID)
{
    qint32 id = getSetting( SettingKeys::Setting, SettingSubKeys::Extension, svrID ).toInt();
    if ( id <= 0 )
    {
        RandDev* randDev{ RandDev::getDevice() };
        if ( randDev != nullptr )
            id = randDev->genRandNum( 1, 0x7FFFFFFE );

        setSetting( id, SettingKeys::Setting, SettingSubKeys::Extension, svrID );

        delete randDev;
    }
    return Helper::intToStr( id, 16, 8 );
}
