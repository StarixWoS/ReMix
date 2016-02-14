
#include "includes.hpp"
#include "settings.hpp"
#include "ui_settings.h"

//Initialize Settings keys/        subKeys lists
const QString Settings::keys[ SETTINGS_KEY_COUNT ] =
{
    "Settings",
    "WrongIPs",
    "Messages",
    "Positions",
    "Rules"
};

const QString Settings::subKeys[ SETTINGS_SUBKEY_COUNT ] =
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
    "BANISHED",
    "reqAdminAuth",
    "logComments",
    "fwdComments",
    "informAdminLogin",
    "echoComments",
    "minimizeToTray",
    "saveWindowPositions"
};

//Initialize our QSettings Object globally to make things more responsive.
QSettings* Settings::prefs{ new QSettings( "preferences.ini",
                                           QSettings::IniFormat ) };

Settings::Settings(QWidget *parent, QString svrID) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    serverID = svrID;

    //Remove the "Help" button from the window title bars.
    {
        QIcon icon = this->windowIcon();
        Qt::WindowFlags flags = this->windowFlags();
        flags &= ~Qt::WindowContextHelpButtonHint;

        this->setWindowFlags( flags );
        this->setWindowIcon( icon );

        //this->setWindowModality( Qt::WindowModal );
    }

    //Setup Objects.
    tabWidget = new QTabWidget( this );
    if ( tabWidget != nullptr )
    {
        settings = new SettingsWidget( this, serverID );
        if ( settings != nullptr )
            tabWidget->addTab( settings, "Settings" );

        rules = new RulesWidget( this, serverID );
        if ( rules != nullptr )
            tabWidget->addTab( rules, "Rules" );

        messages = new MessagesWidget( this, serverID );
        if ( messages != nullptr )
            tabWidget->addTab( messages, "Messages" );

        ui->widget->setLayout( new QGridLayout( ui->widget ) );
        ui->widget->layout()->setContentsMargins( 5, 5, 5, 5 );
        ui->widget->layout()->addWidget( tabWidget );
    }

    if ( this->getSaveWindowPositions( serverID ) )
    {
        QByteArray geometry{ Settings::getWindowPositions(
                             this->metaObject()->className(), serverID ) };
        if ( !geometry.isEmpty() )
        {
            this->restoreGeometry( Settings::getWindowPositions(
                                       this->metaObject()->className(),
                                       serverID ) );
        }
    }
}

Settings::~Settings()
{
    if ( this->getSaveWindowPositions( serverID ) )
    {
        this->setWindowPositions( this->saveGeometry(),
                                  this->metaObject()->className(), serverID );
    }

    tabWidget->deleteLater();
    settings->deleteLater();
    messages->deleteLater();
    rules->deleteLater();

    delete ui;
}

//Static-Free Functions.
void Settings::setSetting(const QString& key, const QString& subKey,
                          QVariant& value, QString& svrID)
{
    prefs->setValue( svrID % "/" % key % "/" % subKey, value );
    prefs->sync();
}

QVariant Settings::getSetting(const QString& key, const QString& subKey,
                              QString& svrID)
{
    return prefs->value( svrID % "/" % key % "/" % subKey );
}

void Settings::setReqAdminAuth(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::ReqAdminAuth ], value, svrID );
}

bool Settings::getReqAdminAuth(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::ReqAdminAuth ], svrID )
              .toBool();
}

void Settings::setMOTDMessage(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Messages ],
                subKeys[ SubKeys::MOTD ], value, svrID );
}

QString Settings::getMOTDMessage(QString& svrID)
{
    return getSetting( keys[ Keys::Messages ],
                       subKeys[ SubKeys::MOTD ], svrID )
              .toString();
}

void Settings::setBanishMesage(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Messages ],
                subKeys[ SubKeys::BanishMsg ], value, svrID );
}

QString Settings::getBanishMesage(QString& svrID)
{
    return getSetting( keys[ Keys::Messages ],
                       subKeys[ SubKeys::BanishMsg ], svrID )
              .toString();
}

void Settings::setPassword(QString& value, QString& svrID)
{
    QVariant pwd{ value };

    //Convert the password to a SHA3_512 hash.
    if ( !value.isEmpty() )
        pwd = Helper::hashPassword( value );

    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::Password ], pwd, svrID );
}

QString Settings::getPassword(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::Password ], svrID )
              .toString();
}

void Settings::setRequirePassword(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::ReqPassword ], value, svrID );
}

bool Settings::getRequirePassword(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::ReqPassword ], svrID )
              .toBool();
}

bool Settings::cmpServerPassword(QString& value, QString& svrID)
{
    if ( !value.isEmpty() )
        value = Helper::hashPassword( value );

    return ( getPassword( svrID ) == value );
}

void Settings::setAllowDupedIP(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::AllowDupe ], value, svrID );
}

bool Settings::getAllowDupedIP(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::AllowDupe ], svrID )
              .toBool();
}

void Settings::setBanDupedIP(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::BanDupes ], value, svrID );
}

bool Settings::getBanDupedIP(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::BanDupes ], svrID )
              .toBool();
}

void Settings::setBanHackers(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::AutoBan ], value, svrID );
}

bool Settings::getBanHackers(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::AutoBan ], svrID )
              .toBool();
}

void Settings::setReqSernums(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::ReqSerNum ], value, svrID );
}

bool Settings::getReqSernums(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::ReqSerNum ], svrID )
              .toBool();
}

void Settings::setDisconnectIdles(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::AllowIdle ], value, svrID );
}

bool Settings::getDisconnectIdles(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::AllowIdle ], svrID )
              .toBool();
}

void Settings::setAllowSSV(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::AllowSSV ], value, svrID );
}

bool Settings::getAllowSSV(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::AllowSSV ], svrID )
              .toBool();
}

void Settings::setLogComments(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::LogComments ], value, svrID );
}

bool Settings::getLogComments(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::LogComments ], svrID )
              .toBool();
}

void Settings::setFwdComments(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::FwdComments ], value, svrID );
}

bool Settings::getFwdComments(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::FwdComments ], svrID )
              .toBool();
}

void Settings::setInformAdminLogin(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::InformAdminLogin ], value, svrID );
}

bool Settings::getInformAdminLogin(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::InformAdminLogin ], svrID )
              .toBool();
}

void Settings::setEchoComments(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::EchoComments ], value, svrID );
}

bool Settings::getEchoComments(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::EchoComments ], svrID )
              .toBool();
}

void Settings::setMinimizeToTray(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::MinimizeToTray ], value, svrID );
}

bool Settings::getMinimizeToTray(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::MinimizeToTray ], svrID )
              .toBool();
}

void Settings::setSaveWindowPositions(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::SaveWindowPositions ], value, svrID );
}

bool Settings::getSaveWindowPositions(QString& svrID)
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::SaveWindowPositions ], svrID )
              .toBool();
}

void Settings::setWindowPositions(QByteArray geometry, const char* dialog,
                                  QString& svrID)
{
    QString key{ dialog };
    QVariant value{ geometry };

    setSetting( keys[ Keys::Positions ], key, value, svrID );
}

QByteArray Settings::getWindowPositions(const char* dialog, QString& svrID)
{
    QString key{ dialog };
    return getSetting( keys[ Keys::Positions ], key, svrID )
              .toByteArray();
}

void Settings::setServerID(QVariant& value, QString& svrID)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::Extension ], value, svrID );
}

int Settings::getServerID(QString& svrID)
{
    qint32 id = getSetting( keys[ Keys::Setting ],
                            subKeys[ SubKeys::Extension ], svrID )
                   .toInt();
    if ( id <= 0 )
    {
        RandDev* randDev = new RandDev();
        if ( randDev != nullptr )
            id = randDev->genRandNum( 1, 0x7FFFFFFE );

        QVariant var{ id };
        setServerID( var, svrID );

        delete randDev;
    }
    return id;
}

bool Settings::getIsInvalidIPAddress(const QString& value)
{
    return prefs->value( keys[ Keys::WrongIP ] % "/" % value )
              .toBool();
}
