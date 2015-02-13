
#include "includes.hpp"
#include "settings.hpp"
#include "ui_settings.h"

//Initialize Settings keys/        subKeys lists
const QString Settings::keys[ SETTINGS_KEY_COUNT ] =
{
    "options",
    "wrongIPs",
    "General"
};

const QString Settings::subKeys[ SETTINGS_SUBKEY_COUNT ] =
{
    "extension",
    "myPassword",
    "autoBanish",
    "discIdle",
    "requireSernum",
    "dupeOK",
    "serverSupportsVariables",
    "banishDupes",
    "requirePassword",
    "MOTD",
    "BANISHED",
    "RULES",
    "requireAdminAuth",
    "logComments",
    "FwdComments",
    "InformAdminLogin",
    "EchoComments"
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

    //Load Settings from file.
    this->setCheckedState( Toggles::REQPWD,
                           this->getRequirePassword() );

    this->setCheckedState( Toggles::REQADMINPWD,
                           this->getReqAdminAuth() );

    this->setCheckedState( Toggles::ALLOWDUPEDIP,
                           this->getAllowDupedIP() );

    this->setCheckedState( Toggles::BANDUPEDIP,
                           this->getBanDupedIP() );

    this->setCheckedState( Toggles::BANHACKERS,
                           this->getBanHackers() );

    this->setCheckedState( Toggles::REQSERNUM,
                           this->getReqSernums() );

    this->setCheckedState( Toggles::DISCONNECTIDLES,
                           this->getDisconnectIdles() );

    this->setCheckedState( Toggles::ALLOWSSV,
                           this->getAllowSSV() );

    this->setCheckedState( Toggles::LOGCOMMENTS,
                           this->getLogComments() );

    this->setCheckedState( Toggles::FWDCOMMENTS,
                           this->getFwdComments() );

    this->setCheckedState( Toggles::INFORMADMINLOGIN,
                           this->getInformAdminLogin() );

    this->setCheckedState( Toggles::ECHOCOMMENTS,
                           this->getEchoComments() );
}

Settings::~Settings()
{
    delete ui;
}

void Settings::setCheckedState(Toggles option, bool val)
{
    Qt::CheckState state;
    if ( val )
        state = Qt::Checked;
    else
        state = Qt::Unchecked;

    ui->settingsView->item( option, 0 )->setCheckState( state );
}

void Settings::on_settingsView_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    if ( row < 0 )
        return;

    Qt::CheckState val = ui->settingsView->item( row, 0 )->checkState();
    ui->settingsView->item( row, 0 )->setCheckState(
                val == Qt::Checked ? Qt::Unchecked : Qt::Checked );

    val = ui->settingsView->item( row, 0 )->checkState();

    QVariant state = val == Qt::Checked;

    QString title{ "" };
    QString prompt{ "" };

    switch ( row )
    {
        case Toggles::REQPWD:
            {
                QString txt{ "" };
                bool ok;

                this->setRequirePassword( state );
                if ( this->getPassword().isEmpty()
                  && this->getRequirePassword() )
                {
                    title = "Server Password:";
                    prompt = "Password:";

                    txt = Helper::getTextResponse( this, title,
                                                   prompt, &ok, 0 );
                    if ( ok && !txt.isEmpty() )
                    {
                        this->setPassword( txt );
                    }
                    else
                    {   //Invalid dialog state or no input Password.
                        //Reset the Object's state.
                        ui->settingsView->item( row, 0 )->setCheckState(
                                    Qt::Unchecked );

                        state = false;
                        this->setRequirePassword( state );
                    }
                }
                else if ( !this->getRequirePassword() )
                {
                    title = "Remove Password:";
                    prompt = "Do you wish to erase the stored Password hash?";

                    if ( Helper::confirmAction( this, title, prompt ) )
                        this->setPassword( txt );
                }
            }
        break;
        case Toggles::REQADMINPWD:
            this->setReqAdminAuth( state );
        break;
        case Toggles::ALLOWDUPEDIP:
            this->setAllowDupedIP( state );
        break;
        case Toggles::BANDUPEDIP:
            this->setBanDupedIP( state );
        break;
        case Toggles::BANHACKERS:
            this->setBanHackers( state );
        break;
        case Toggles::REQSERNUM:
            this->setReqSernums( state );
        break;
        case Toggles::DISCONNECTIDLES:
            this->setDisconnectIdles( state );
        break;
        case Toggles::ALLOWSSV:
            this->setAllowSSV( state );
        break;
        case Toggles::LOGCOMMENTS:
            this->setLogComments( state );
        break;
        case Toggles::FWDCOMMENTS:
            this->setFwdComments( state );
        break;
        case Toggles::ECHOCOMMENTS:
            this->setEchoComments( state );
        break;
        case Toggles::INFORMADMINLOGIN:
            this->setInformAdminLogin( state );
        break;
        default:
            qDebug() << "Unknown Option, doing nothing!";
        break;
    }
}

//Static-Free Functions.
void Settings::setSetting(const QString& key, const QString& subKey,
                        QVariant& value)
{
    QSettings setting( "preferences.ini", QSettings::IniFormat );

    if ( key == QLatin1String( "General" ) )
        setting.setValue( subKey, value );
    else
        setting.setValue( key % "/" % subKey, value );
}

QVariant Settings::getSetting(const QString& key, const QString& subKey)
{
    QSettings setting( "preferences.ini", QSettings::IniFormat );

    if ( key == QLatin1String( "General" ) )
        return setting.value( subKey );

    return setting.value( key % "/" % subKey );
}

void Settings::setReqAdminAuth(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::ReqAdminAuth ], value );
}

bool Settings::getReqAdminAuth()
{
    return getSetting( keys[ Keys::Options ],
                       subKeys[ SubKeys::ReqAdminAuth ] )
              .toBool();
}

void Settings::setMOTDMessage(QVariant& value)
{
    setSetting( keys[ Keys::General ],
                subKeys[ SubKeys::MOTD ], value );
}

QString Settings::getMOTDMessage()
{
    return getSetting( keys[ Keys::General ],
                       subKeys[ SubKeys::MOTD ] )
              .toString();
}

void Settings::setBanishMesage(QVariant& value)
{
    setSetting( keys[ Keys::General ],
                subKeys[ SubKeys::BanishMsg ], value );
}

QString Settings::getBanishMesage()
{
    return getSetting( keys[ Keys::General ],
                       subKeys[ SubKeys::BanishMsg ] )
              .toString();
}

void Settings::setPassword(QString& value)
{
    QVariant pwd{ value };

    //Convert the password to a SHA3_512 hash.
    if ( !value.isEmpty() )
    {
        pwd = Helper::hashPassword( pwd );
    }
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::Password ], pwd );
}

QString Settings::getPassword()
{
    return getSetting( keys[ Keys::Options ],
                       subKeys[ SubKeys::Password ] )
              .toString();
}

void Settings::setRequirePassword(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::ReqPassword ], value );
}

bool Settings::getRequirePassword()
{
    return getSetting( keys[ Keys::Options ],
                       subKeys[ SubKeys::ReqPassword ] )
              .toBool();
}

bool Settings::cmpServerPassword(QVariant& value)
{
    if ( !value.toString().isEmpty() )
        value = Helper::hashPassword( value );

    return ( getPassword() == value.toString() );
}

void Settings::setServerRules(QVariant& value)
{
    setSetting( keys[ Keys::General ],
                subKeys[ SubKeys::Rules ], value );
}

QString Settings::getServerRules()
{
    QVariant pending = getSetting( keys[ Keys::General ],
                                   subKeys[ SubKeys::Rules ] );

    QString rules;
    if ( pending.type() == QVariant::StringList )
    {
        QStringList ruleList = pending.toStringList();
        for ( int i = 0; i < ruleList.count(); ++i )
        {
            if ( i > 0 )
                rules.append( ", " );

            rules.append( ruleList.at( i ) );
        }
    }
    else if ( pending.type() == QVariant::String )
        rules = pending.toString();

    return rules;
}

void Settings::setAllowDupedIP(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::AllowDupe ], value );
}

bool Settings::getAllowDupedIP()
{
    return getSetting( keys[ Keys::Options ],
                       subKeys[ SubKeys::AllowDupe ] )
              .toBool();
}

void Settings::setBanDupedIP(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::BanDupes ], value );
}

bool Settings::getBanDupedIP()
{
    return getSetting( keys[ Keys::Options ],
                       subKeys[ SubKeys::BanDupes ] )
              .toBool();
}

void Settings::setBanHackers(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::AutoBan ], value );
}

bool Settings::getBanHackers()
{
    return getSetting( keys[ Keys::Options ],
                       subKeys[ SubKeys::AutoBan ] )
              .toBool();
}

void Settings::setReqSernums(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::ReqSerNum ], value );
}

bool Settings::getReqSernums()
{
    return getSetting( keys[ Keys::Options ],
                       subKeys[ SubKeys::ReqSerNum ] )
              .toBool();
}

void Settings::setDisconnectIdles(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::AllowIdle ], value );
}

bool Settings::getDisconnectIdles()
{
    return getSetting( keys[ Keys::Options ],
                       subKeys[ SubKeys::AllowIdle ] )
              .toBool();
}

void Settings::setAllowSSV(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::AllowSSV ], value );
}

bool Settings::getAllowSSV()
{
    return getSetting( keys[ Keys::Options ],
                       subKeys[ SubKeys::AllowSSV ] )
              .toBool();
}

void Settings::setLogComments(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::LogComments ], value );
}

bool Settings::getLogComments()
{
    return getSetting( keys[ Keys::Options ],
                       subKeys[ SubKeys::LogComments ] )
              .toBool();
}

void Settings::setFwdComments(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::FwdComments ], value );
}

bool Settings::getFwdComments()
{
    return getSetting( keys[ Keys::Options ],
                       subKeys[ SubKeys::FwdComments ] )
              .toBool();
}

void Settings::setInformAdminLogin(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::InformAdminLogin ], value );
}

bool Settings::getInformAdminLogin()
{
    return getSetting( keys[ Keys::Options ],
                       subKeys[ SubKeys::InformAdminLogin ] )
              .toBool();
}

void Settings::setEchoComments(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::EchoComments ], value );
}

bool Settings::getEchoComments()
{
    return getSetting( keys[ Keys::Options ],
                       subKeys[ SubKeys::EchoComments ] )
              .toBool();
}

void Settings::setServerID(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::Extension ], value );
}

int Settings::getServerID()
{
    qint32 id = getSetting( keys[ Keys::Options ],
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
