
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

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

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
        settings = new SettingsWidget( this );
        if ( settings != nullptr )
            tabWidget->addTab( settings, "Settings" );

        rules = new RulesWidget( this );
        if ( rules != nullptr )
            tabWidget->addTab( rules, "Rules" );

        messages = new MessagesWidget( this );
        if ( messages != nullptr )
            tabWidget->addTab( messages, "Messages" );

        ui->widget->setLayout( new QGridLayout( ui->widget ) );
        ui->widget->layout()->setContentsMargins( 5, 5, 5, 5 );
        ui->widget->layout()->addWidget( tabWidget );
    }

    if ( this->getSaveWindowPositions() )
    {
        QByteArray geometry{ Settings::getWindowPositions(
                                    this->metaObject()->className() ) };
        if ( !geometry.isEmpty() )
        {
            this->restoreGeometry( Settings::getWindowPositions(
                                       this->metaObject()->className() ) );
        }
    }
}

Settings::~Settings()
{
    if ( this->getSaveWindowPositions() )
    {
        this->setWindowPositions( this->saveGeometry(),
                                  this->metaObject()->className() );
    }

    tabWidget->deleteLater();
    settings->deleteLater();
    messages->deleteLater();
    rules->deleteLater();

    delete ui;
}

//Static-Free Functions.
void Settings::setSetting(const QString& key, const QString& subKey,
                          QVariant& value)
{
    QSettings setting( "preferences.ini", QSettings::IniFormat );
    setting.setValue( key % "/" % subKey, value );
}

QVariant Settings::getSetting(const QString& key, const QString& subKey)
{
    QSettings setting( "preferences.ini", QSettings::IniFormat );
    return setting.value( key % "/" % subKey );
}

void Settings::setReqAdminAuth(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::ReqAdminAuth ], value );
}

bool Settings::getReqAdminAuth()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::ReqAdminAuth ] )
              .toBool();
}

void Settings::setMOTDMessage(QVariant& value)
{
    setSetting( keys[ Keys::Messages ],
                subKeys[ SubKeys::MOTD ], value );
}

QString Settings::getMOTDMessage()
{
    return getSetting( keys[ Keys::Messages ],
                       subKeys[ SubKeys::MOTD ] )
              .toString();
}

void Settings::setBanishMesage(QVariant& value)
{
    setSetting( keys[ Keys::Messages ],
                subKeys[ SubKeys::BanishMsg ], value );
}

QString Settings::getBanishMesage()
{
    return getSetting( keys[ Keys::Messages ],
                       subKeys[ SubKeys::BanishMsg ] )
              .toString();
}

void Settings::setPassword(QString& value)
{
    QVariant pwd{ value };

    //Convert the password to a SHA3_512 hash.
    if ( !value.isEmpty() )
        pwd = Helper::hashPassword( value );

    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::Password ], pwd );
}

QString Settings::getPassword()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::Password ] )
              .toString();
}

void Settings::setRequirePassword(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::ReqPassword ], value );
}

bool Settings::getRequirePassword()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::ReqPassword ] )
              .toBool();
}

bool Settings::cmpServerPassword(QString& value)
{
    if ( !value.isEmpty() )
        value = Helper::hashPassword( value );

    return ( getPassword() == value );
}

void Settings::setAllowDupedIP(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::AllowDupe ], value );
}

bool Settings::getAllowDupedIP()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::AllowDupe ] )
              .toBool();
}

void Settings::setBanDupedIP(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::BanDupes ], value );
}

bool Settings::getBanDupedIP()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::BanDupes ] )
              .toBool();
}

void Settings::setBanHackers(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::AutoBan ], value );
}

bool Settings::getBanHackers()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::AutoBan ] )
              .toBool();
}

void Settings::setReqSernums(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::ReqSerNum ], value );
}

bool Settings::getReqSernums()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::ReqSerNum ] )
              .toBool();
}

void Settings::setDisconnectIdles(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::AllowIdle ], value );
}

bool Settings::getDisconnectIdles()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::AllowIdle ] )
              .toBool();
}

void Settings::setAllowSSV(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::AllowSSV ], value );
}

bool Settings::getAllowSSV()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::AllowSSV ] )
              .toBool();
}

void Settings::setLogComments(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::LogComments ], value );
}

bool Settings::getLogComments()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::LogComments ] )
              .toBool();
}

void Settings::setFwdComments(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::FwdComments ], value );
}

bool Settings::getFwdComments()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::FwdComments ] )
              .toBool();
}

void Settings::setInformAdminLogin(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::InformAdminLogin ], value );
}

bool Settings::getInformAdminLogin()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::InformAdminLogin ] )
              .toBool();
}

void Settings::setEchoComments(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::EchoComments ], value );
}

bool Settings::getEchoComments()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::EchoComments ] )
              .toBool();
}

void Settings::setMinimizeToTray(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::MinimizeToTray ], value );
}

bool Settings::getMinimizeToTray()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::MinimizeToTray ] )
              .toBool();
}

void Settings::setSaveWindowPositions(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::SaveWindowPositions ], value );
}

bool Settings::getSaveWindowPositions()
{
    return getSetting( keys[ Keys::Setting ],
                       subKeys[ SubKeys::SaveWindowPositions ] )
              .toBool();
}

void Settings::setWindowPositions(QByteArray geometry, const char* dialog)
{
    QString key{ dialog };
    QVariant value{ geometry };

    setSetting( keys[ Keys::Positions ], key, value );
}

QByteArray Settings::getWindowPositions(const char* dialog)
{
    QString key{ dialog };
    return getSetting( keys[ Keys::Positions ], key )
              .toByteArray();
}

void Settings::setServerID(QVariant& value)
{
    setSetting( keys[ Keys::Setting ],
                subKeys[ SubKeys::Extension ], value );
}

int Settings::getServerID()
{
    qint32 id = getSetting( keys[ Keys::Setting ],
                            subKeys[ SubKeys::Extension ] )
                   .toInt();
    if ( id <= 0 )
    {
        RandDev* randDev = new RandDev();
        if ( randDev != nullptr )
            id = randDev->genRandNum( 1, 0x7FFFFFFE );

        QVariant var{ id };
        setServerID( var );

        delete randDev;
    }
    return id;
}

bool Settings::getIsInvalidIPAddress(const QString& value)
{
    return getSetting( keys[ Keys::WrongIP ], value )
              .toBool();
}
