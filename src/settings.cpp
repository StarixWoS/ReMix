
//Class includes.
#include "settings.hpp"
#include "ui_settings.h"

//ReMix Widget includes.
#include "widgets/settingswidget.hpp"
#include "widgets/ruleswidget.hpp"
#include "widgets/colorwidget.hpp"
#include "widgets/motdwidget.hpp"

//ReMix includes.
#include "randdev.hpp"
#include "helper.hpp"
#include "server.hpp"
#include "remix.hpp"

//Qt Includes.
#include <QNetworkAccessManager>
#include <QTabWidget>
#include <QUdpSocket>
#include <QString>
#include <QObject>
#include <QtCore>

//Initialize Settings keys/subKeys lists
const QMap<SKeys, QString> Settings::pKeys =
{
    { SKeys::Setting,   "Settings"  },
    { SKeys::Messages,  "Messages"  },
    { SKeys::Positions, "Positions" },
    { SKeys::Rules,     "Rules"     },
    { SKeys::Logger,    "Logger"    },
    { SKeys::Colors,    "Colors"    },
};

const QMap<SSubKeys, QString> Settings::sKeys =
{
    //Settings.
    { SSubKeys::ServerID,               "serverID"            },
    { SSubKeys::AutoBan,                "autoBanish"          },
    { SSubKeys::AllowIdle,              "discIdle"            },
    { SSubKeys::ReqSerNum,              "reqSerNums"          },
    { SSubKeys::AllowDupe,              "dupeOK"              },
    { SSubKeys::AllowSSV,               "supportsSSV"         },
    { SSubKeys::BanDupes,               "banishDupeIP"        },
    { SSubKeys::CensorIPInfo,           "censorUIIPInfo"      },
    { SSubKeys::MOTD,                   "MOTD"                },
    { SSubKeys::LogComments,            "logComments"         },
    { SSubKeys::FwdComments,            "fwdComments"         },
    { SSubKeys::InformAdminLogin,       "informAdminLogin"    },
    { SSubKeys::EchoComments,           "echoComments"        },
    { SSubKeys::MinimizeToTray,         "minimizeToTray"      },
    { SSubKeys::SaveWindowPositions,    "saveWindowPositions" },
    { SSubKeys::IsRunning,              "isRunning"           },
    { SSubKeys::WorldDir,               "worldDir"            },
    { SSubKeys::PortNumber,             "portNumber"          },
    { SSubKeys::IsPublic,               "isPublic"            },
    { SSubKeys::GameName,               "gameName"            },
    { SSubKeys::LogFiles,               "logFiles"            },
    { SSubKeys::DarkMode,               "darkMode"            },
    { SSubKeys::UseUPNP,                "useUPNP"             },
    { SSubKeys::CheckForUpdates,        "checkForUpdates"     },
    { SSubKeys::DCBlueCodedSerNums,     "dcBlueCodedSerNums"  },
    { SSubKeys::LoggerAutoScroll,       "loggerAutoScroll"    },
    { SSubKeys::OverrideMasterIP,       "overrideMasterIP"    },
    { SSubKeys::LoggerAutoClear,        "loggerAutoClear"     },
    { SSubKeys::OverrideMasterHost,     "overrideMasterHost"  },
    { SSubKeys::ChatAutoScroll,         "chatAutoScroll"      },
    { SSubKeys::ChatTimeStamp,          "chatTimeStampTxt"    },
    { SSubKeys::HidePlayerView,         "hidePlayerView"      },
    { SSubKeys::HideChatView,           "hideChatView"        },
    { SSubKeys::NetInterface,           "netInterface"        },
    { SSubKeys::ServerButtonState,      "serverButtonState"   },
    { SSubKeys::ServerPlayerChatSize,   "serverPlayerChatSize"},

    //Rules.
    { SSubKeys::StrictRules,            "enforceRules"   },
    { SSubKeys::HasSvrPassword,         "hasSvrPassword" },
    { SSubKeys::SvrPassword,            "svrPassword"    },
    { SSubKeys::WorldName,              "world"          },
    { SSubKeys::ToYName,                "toy"            },
    { SSubKeys::SvrUrl,                 "url"            },
    { SSubKeys::AllPK,                  "allPK"          },
    { SSubKeys::MaxPlayers,             "maxP"           },
    { SSubKeys::MaxIdle,                "maxIdle"        },
    { SSubKeys::MinVersion,             "minV"           },
    { SSubKeys::PKLadder,               "ladder"         },
    { SSubKeys::NoBleep,                "noBleep"        },
    { SSubKeys::NoCheat,                "noCheat"        },
    { SSubKeys::NoEavesdrop,            "noEavesdrop"    },
    { SSubKeys::NoMigrate,              "noMigrate"      },
    { SSubKeys::NoModding,              "noMod"          },
    { SSubKeys::NoPets,                 "noPets"         },
    { SSubKeys::NoPK,                   "noPK"           },
    { SSubKeys::ArenaPK,                "arenaPK"        },
    { SSubKeys::AutoRestart,            "autoRestart"    },
};

const QMap<Colors, QString> Settings::cKeys =
{
    //Color Roles.
    { Colors::GossipTxt,        "colorGossipTxt"      },
    { Colors::ShoutTxt,         "colorShoutTxt"       },
    { Colors::EmoteTxt,         "colorEmoteTxt"       },
    { Colors::DiceAndLevel,     "colorDiceAndLevel"   },
    { Colors::SpellTxt,         "colorSpellTxt"       },
    { Colors::DeathTxt,         "colorDeathTxt"       },
    { Colors::PlayerTxt,        "colorPlayerTxt"      },
    { Colors::AdminTxt,         "colorAdminTxt"       },
    { Colors::AdminMessage,     "colorAdminMsg"       },
    { Colors::OwnerTxt,         "colorOwnerTxt"       },
    { Colors::CommentTxt,       "colorCommentTxt"     },
    { Colors::GoldenSoul,       "colorGoldenSoul"     },
    { Colors::WhiteSoul,        "colorWhiteSoul"      },
    { Colors::PlayerName,       "colorPlayerName"     },
    { Colors::AdminName,        "colorPlayerName"     },
    { Colors::OwnerName,        "colorOwnerName"      },
    { Colors::TimeStamp,        "colorTimestamp"      },
    { Colors::AdminValid,       "colorAdminValid"     },
    { Colors::AdminInvalid,     "colorAdminInvalid"   },
    { Colors::IPValid,          "colorIPValid"        },
    { Colors::IPInvalid,        "colorIPInvalid"      },
    { Colors::IPVanished,       "colorIPVanished"     },
    { Colors::PartyJoin,        "colorPartyJoin"      },
    { Colors::PKChallenge,      "colorPKChallenge"    },
    { Colors::SoulIncarnated,   "colorSoulIncarnated" },
    { Colors::SoulLeftWorld,    "colorSoulLeftWorld"  },
};

QVector<SSubKeys> Settings::serverSettings //Settings Specific to a Server Instance.
{
    SSubKeys::ServerPlayerChatSize,
    SSubKeys::ServerButtonState,
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
    SSubKeys::StrictRules,
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
    SSubKeys::WorldName,
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
        this->restoreGeometry( Settings::getSetting( SKeys::Positions, "Settings" ).toByteArray() );
}

Settings::~Settings()
{
    if ( getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        setSetting( this->saveGeometry(), SKeys::Positions, "Settings" );

    ColorWidget::deleteInstance();
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
        instance = new Settings( ReMix::getInstance() );

    return instance;
}

void Settings::setInstance(Settings* value)
{
    instance = value;
}

void Settings::updateTabBar(QSharedPointer<Server> server)
{
    qint32 index{ tabWidget->currentIndex() };

    tabWidget->clear();

    getInstance()->setWindowTitle( "[ " % server->getServerName() % " ] Settings:");
    tabWidget->insertTab( 0, SettingsWidget::getInstance(), "Settings" );
    tabWidget->insertTab( 1, RulesWidget::getInstance( server ), "Rules" );
    tabWidget->insertTab( 2, ColorWidget::getInstance(), "Colors" );
    tabWidget->insertTab( 3, MOTDWidget::getInstance( server ), "MotD" );

    tabWidget->setCurrentIndex( index );
}

void Settings::copyServerSettings(QSharedPointer<Server> server, const QString& newName)
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
                       .arg( Settings::pKeys.value( key ) )
                       .arg( Settings::sKeys.value( subKey ) );
    return path;
}

QString Settings::makeSettingPath(const SKeys& key, const SSubKeys& subKey)
{
    QMutexLocker<QMutex> locker( &mutex );
    QString path{ "%1/%2" };
            path = path.arg( Settings::pKeys.value( key ) )
                       .arg( Settings::sKeys.value( subKey ) );
    return path;
}

QString Settings::makeSettingPath(const SKeys& key, const Themes& theme, const Colors& subKey)
{
    QMutexLocker<QMutex> locker( &mutex );
    QString themeStr{ "Light" };
    if ( theme == Themes::Dark )
        themeStr = "Dark";

    QString path{ "%1/%2/%3" };
            path = path.arg( Settings::pKeys.value( key ) )
                       .arg( themeStr )
                       .arg( Settings::cKeys.value( subKey ) );
    return path;
}

QString Settings::makeSettingPath(const SKeys& key, const QVariant& subKey)
{
    QMutexLocker<QMutex> locker( &mutex );
    QString path{ "%1/%2" };
            path = path.arg( Settings::pKeys.value( key ) )
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

void Settings::setSetting(const QVariant& value, const SKeys& key, const Themes& theme, const Colors& subKey)
{
    setSettingFromPath( makeSettingPath( key, theme, subKey ), value );
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

QVariant Settings::getSetting(const SKeys& key, const Themes& theme, const Colors& subKey)
{
    return getSettingFromPath( makeSettingPath( key, theme, subKey ) );
}

QVariant Settings::getSetting(const SKeys& key, const QString& subKey)
{
    return getSettingFromPath( makeSettingPath( key, subKey ) );
}

//Retain function for ease of use.
QString Settings::getServerID(const QString& svrID)
{
    qint32 id{ getSetting( SKeys::Setting, SSubKeys::ServerID, svrID ).toInt() };
    if ( id <= 0 )
    {
        id = RandDev::getInstance().getGen( 1, 0x7FFFFFFE );
        setSetting( id, SKeys::Setting, SSubKeys::ServerID, svrID );
    }
    return Helper::intToStr( id, IntBase::HEX, IntFills::QuadWord );
}

QString Settings::getRuleSet(const QString& svrID)
{
    QMutexLocker<QMutex> locker( &mutex );
    prefs->sync();
    prefs->beginGroup( svrID % "/" % Settings::pKeys.value( SKeys::Rules ) );

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

bool Settings::removeBioHash(const QHostAddress& addr)
{
    QMutexLocker<QMutex> locker( &mutex );
    return static_cast<bool>( bioHash.remove( addr ) );
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
