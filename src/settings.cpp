
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

//Qt Includes.
#include <QNetworkAccessManager>
#include <QTabWidget>
#include <QUdpSocket>
#include <QString>
#include <QObject>
#include <QtCore>

//Initialize Settings keys/        subKeys lists
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
    "serverPwd",
    "autoBanish",
    "discIdle",
    "reqSerNums",
    "dupeOK",
    "supportsSSV",
    "banishDupeIP",
    "reqServerPwd",
    "MOTD",
    "reqAdminAuth",
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

    if ( this->getSaveWindowPositions() )
    {
        QByteArray geometry{ Settings::getWindowPositions( this->metaObject()->className() ) };
        if ( !geometry.isEmpty() )
            this->restoreGeometry( Settings::getWindowPositions( this->metaObject()->className() ) );
    }
}

Settings::~Settings()
{
    if ( this->getSaveWindowPositions() )
        this->setWindowPositions( this->saveGeometry(), this->metaObject()->className() );

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
    QMutexLocker locker( &mutex );
    QString oldName{ server->getServerName() };
    if ( oldName != newName )
    {
        //Copy Rules.
        Rules::setNoEavesdropping( Rules::getNoEavesdropping( oldName ), newName );
        Rules::setReportLadder( Rules::getReportLadder( oldName ), newName );
        Rules::setNoMigrating( Rules::getNoMigrating( oldName ), newName );
        Rules::setURLAddress( Rules::getURLAddress( oldName ), newName );
        Rules::setMinVersion( Rules::getMinVersion( oldName ), newName );
        Rules::setNoCheating( Rules::getNoCheating( oldName ), newName );
        Rules::setArenaPKing( Rules::getArenaPKing( oldName ), newName );
        Rules::setMaxPlayers( Rules::getMaxPlayers( oldName ), newName );
        Rules::setNoCursing( Rules::getNoCursing( oldName ), newName );
        Rules::setNoModding( Rules::getNoModding( oldName ), newName );
        Rules::setWorldName( Rules::getWorldName( oldName ),newName );
        Rules::setAllPKing( Rules::getAllPKing( oldName ), newName );
        Rules::setNoPKing( Rules::getNoPKing( oldName ), newName );
        Rules::setMaxAFK( Rules::getMaxAFK( oldName ), newName );
        Rules::setNoPets( Rules::getNoPets( oldName ), newName );

        //Copy other Settings.
        setServerRunning( getServerRunning( oldName ), newName );
        setMOTDMessage( getMOTDMessage( oldName ), newName );
        setPortNumber( getPortNumber( oldName ).toUShort(), newName );
        setServerID( getServerID( oldName ).toInt(), newName );
        setIsPublic( getIsPublic( oldName ), newName );
        setGameName( getGameName( oldName ), newName );

        prefs->remove( oldName );
        prefs->sync();
    }
}

//Static-Free Functions.
void Settings::setSetting(const QString& key, const QString& subKey, const QVariant& value)
{
    QMutexLocker locker( &mutex );
    prefs->setValue( key % "/" % subKey, value );
    prefs->sync();
}

QVariant Settings::getSetting(const QString& key, const QString& subKey)
{
    QMutexLocker locker( &mutex );
    return prefs->value( key % "/" % subKey );
}

void Settings::setServerSetting(const QString& key, const QString& subKey, const QVariant& value, const QString& svrID)
{
    QMutexLocker locker( &mutex );
    prefs->setValue( svrID % "/" % key % "/" % subKey, value );
    prefs->sync();
}

QVariant Settings::getServerSetting(const QString& key, const QString& subKey, const QString& svrID)
{
    QMutexLocker locker( &mutex );
    return prefs->value( svrID % "/" % key % "/" % subKey );
}

void Settings::setReqAdminAuth(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::ReqAdminAuth ], value );
}

bool Settings::getReqAdminAuth()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::ReqAdminAuth ] )
              .toBool();
}

void Settings::setPassword(const QString& value)
{
    QString hash{ value };

    //Convert the password to a SHA3_512 hash.
    if ( !value.isEmpty() )
    {
        hash = Helper::hashPassword( hash );
        setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::Password ], hash );
    }
}

QString Settings::getPassword()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::Password ] )
              .toString();
}

void Settings::setRequirePassword(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::ReqPassword ], value );
}

bool Settings::getRequirePassword()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::ReqPassword ] )
              .toBool();
}

bool Settings::cmpServerPassword(const QString& value)
{
    QString hash{ value };
    if ( !hash.isEmpty() )
        return ( getPassword() == Helper::hashPassword( hash ) );

    return false;
}

void Settings::setAllowDupedIP(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::AllowDupe ], value );
}

bool Settings::getAllowDupedIP()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::AllowDupe ] )
              .toBool();
}

void Settings::setBanDupedIP(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::BanDupes ], value );
}

bool Settings::getBanDupedIP()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::BanDupes ] )
              .toBool();
}

void Settings::setBanHackers(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::AutoBan ], value );
}

bool Settings::getBanDeviants()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::AutoBan ] )
              .toBool();
}

void Settings::setReqSernums(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::ReqSerNum ], value );
}

bool Settings::getReqSernums()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::ReqSerNum ] )
              .toBool();
}

void Settings::setDisconnectIdles(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::AllowIdle ], value );
}

bool Settings::getDisconnectIdles()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::AllowIdle ] )
              .toBool();
}

void Settings::setAllowSSV(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::AllowSSV ], value );
}

bool Settings::getAllowSSV()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::AllowSSV ] )
              .toBool();
}

void Settings::setLogComments(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::LogComments ], value );
}

bool Settings::getLogComments()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::LogComments ] )
              .toBool();
}

void Settings::setLogFiles(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::LogFiles ], value );
}

bool Settings::getLogFiles()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::LogFiles ] )
            .toBool();
}

void Settings::setDarkMode(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::DarkMode ], value );
}

bool Settings::getDarkMode()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::DarkMode ] )
            .toBool();
}

void Settings::setFwdComments(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::FwdComments ], value );
}

bool Settings::getFwdComments()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::FwdComments ] )
              .toBool();
}

void Settings::setInformAdminLogin(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::InformAdminLogin ], value );
}

bool Settings::getInformAdminLogin()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::InformAdminLogin ] )
              .toBool();
}

void Settings::setEchoComments(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::EchoComments ], value );
}

bool Settings::getEchoComments()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::EchoComments ] )
              .toBool();
}

void Settings::setMinimizeToTray(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::MinimizeToTray ], value );
}

bool Settings::getMinimizeToTray()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::MinimizeToTray ] )
            .toBool();
}

void Settings::setSaveWindowPositions(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::SaveWindowPositions ], value );
}

bool Settings::getSaveWindowPositions()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::SaveWindowPositions ] )
              .toBool();
}

void Settings::setCheckForUpdates(const bool& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::CheckForUpdates ], value );
}

bool Settings::getCheckForUpdates()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::CheckForUpdates ] )
              .toBool();
}

void Settings::setDCBlueCodedSerNums(const bool& value)
{

    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::DCBlueCodedSerNums ], value );
}

bool Settings::getDCBlueCodedSerNums()
{

    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::DCBlueCodedSerNums ] )
              .toBool();
}

void Settings::setWindowPositions(const QByteArray& geometry, const char* dialog)
{
    setSetting( keys[ Keys::Positions ], dialog, geometry );
}

QByteArray Settings::getWindowPositions(const char* dialog)
{
    return getSetting( keys[ Keys::Positions ], dialog )
              .toByteArray();
}

void Settings::setIsInvalidIPAddress(const QString& value)
{
    setSetting( keys[ Keys::WrongIP ], value, true );
}

bool Settings::getIsInvalidIPAddress(const QString& value)
{
    return getSetting( keys[ Keys::WrongIP ], value )
              .toBool();
}

void Settings::setMOTDMessage(const QString& value, const QString& svrID)
{
    setServerSetting( keys[ Keys::Messages ], subKeys[ SubKeys::MOTD ], value, svrID );
}

QString Settings::getMOTDMessage(const QString& svrID)
{
    return getServerSetting( keys[ Keys::Messages ], subKeys[ SubKeys::MOTD ], svrID )
                    .toString();
}

void Settings::setServerID(const qint32& value, const QString& svrID)
{
    setServerSetting( keys[ Keys::Setting ], subKeys[ SubKeys::Extension ], value, svrID );
}

QString Settings::getServerID(const QString& svrID)
{
    qint32 id = getServerSetting( keys[ Keys::Setting ], subKeys[ SubKeys::Extension ], svrID )
                         .toInt();
    if ( id <= 0 )
    {
        RandDev* randDev{ RandDev::getDevice() };
        if ( randDev != nullptr )
            id = randDev->genRandNum( 1, 0x7FFFFFFE );

        setServerID( id, svrID );

        delete randDev;
    }
    return Helper::intToStr( id, 16, 8 );
}

void Settings::setServerRunning(const bool& value, const QString& svrID)
{
    QMutexLocker locker( &mutex );
    prefs->setValue( svrID % "/" % subKeys[ SubKeys::IsRunning ], value );
    prefs->sync();
}

bool Settings::getServerRunning(const QString& svrID)
{
    QMutexLocker locker( &mutex );
    return prefs->value( svrID % "/" % subKeys[ SubKeys::IsRunning ] )
                    .toBool();
}

void Settings::setWorldDir(const QString& value)
{
    setSetting( keys[ Keys::Setting ], subKeys[ SubKeys::WorldDir ], value );
}

QString Settings::getWorldDir()
{
    return getSetting( keys[ Keys::Setting ], subKeys[ SubKeys::WorldDir ] )
              .toString();
}

void Settings::setPortNumber(const quint16& value, const QString& svrID)
{
    setServerSetting( keys[ Keys::Setting ], subKeys[ SubKeys::PortNumber ], value, svrID );
}

QString Settings::getPortNumber(const QString& svrID)
{
    return getServerSetting( keys[ Keys::Setting ], subKeys[ SubKeys::PortNumber ], svrID )
                    .toString();
}

void Settings::setIsPublic(const bool& value, const QString& svrID)
{
    setServerSetting( keys[ Keys::Setting ], subKeys[ SubKeys::IsPublic ], value, svrID );
}

bool Settings::getIsPublic(const QString& svrID)
{
    return getServerSetting( keys[ Keys::Setting ], subKeys[ SubKeys::IsPublic ], svrID )
            .toBool();
}

void Settings::setUseUPNP(const bool& value, const QString& svrID)
{
    setServerSetting( keys[ Keys::Setting ], subKeys[ SubKeys::UseUPNP ], value, svrID );
}

bool Settings::getUseUPNP(const QString& svrID)
{
    return getServerSetting( keys[ Keys::Setting ], subKeys[ SubKeys::UseUPNP ], svrID )
                    .toBool();
}

void Settings::setGameName(const QString& value, const QString& svrID)
{
    setServerSetting( keys[ Keys::Setting ], subKeys[ SubKeys::GameName ], value, svrID );
}

QString Settings::getGameName(const QString& svrID)
{
    return getServerSetting( keys[ Keys::Setting ], subKeys[ SubKeys::GameName ], svrID )
            .toString();
}

void Settings::setLoggerAutoScroll(const bool& value)
{
    setSetting( keys[ Keys::Logger ], subKeys[ SubKeys::LoggerAutoScroll ], value );
}

bool Settings::getLoggerAutoScroll()
{
    return getSetting( keys[ Keys::Logger ], subKeys[ SubKeys::LoggerAutoScroll ] )
              .toBool();
}
