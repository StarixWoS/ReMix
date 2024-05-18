
//Class includes.
#include "cmdtableoverride.hpp"
#include "ui_cmdtableoverride.h"

//Required ReMix Widget includes.
#include "widgets/cmdtabledelegate.hpp"

//ReMix includes.
#include "settings.hpp"
#include "helper.hpp"
#include "remix.hpp"

//Qt Includes.
#include <QSettings>
#include <QtCore>

const QMap<GMCmds, QString> CmdTableOverride::commands =
{
    // Commands
    { GMCmds::Help,         "help"         },
    { GMCmds::List,         "list"         },
    { GMCmds::MotD,         "motd"         },
    { GMCmds::Info,         "info"         },
    { GMCmds::Ban,          "ban"          },
    { GMCmds::UnBan,        "unban"        },
    { GMCmds::Kick,         "kick"         },
    { GMCmds::Mute,         "mute"         },
    { GMCmds::UnMute,       "unmute"       },
    { GMCmds::Quarantine,   "quarantine"   },
    { GMCmds::UnQuarantine, "unquarantine" },
    { GMCmds::Message,      "message"      },
    { GMCmds::LogIn,        "login"        },
    { GMCmds::LogOut,       "logout"       },
    { GMCmds::Register,     "register"     },
    { GMCmds::ShutDown,     "shutdown"     },
    { GMCmds::ReStart,      "restart"      },
    { GMCmds::Vanish,       "vanish"       },
    { GMCmds::Version,      "version"      },
    { GMCmds::Ping,         "ping"         },
    { GMCmds::Camp,         "camp"         },
};
const QMap<GMSubCmds, QString> CmdTableOverride::subCommands =
{
    // SubCommands
    { GMSubCmds::MotDChange,       "motd/change"        },
    { GMSubCmds::MotDRemove,       "motd/remove"        },
    { GMSubCmds::InfoServer,       "info/server"        },
    { GMSubCmds::InfoQuarantined,  "into/quarantined"   },
    { GMSubCmds::InfoMuted,        "into/muted"         },
    { GMSubCmds::InfoIP,           "info/ip"            },
    { GMSubCmds::InfoSoul,         "info/soul"          },
    { GMSubCmds::BanIP,            "ban/ip"             },
    { GMSubCmds::BanSoul,          "ban/soul"           },
    { GMSubCmds::BanAll,           "ban/all"            },
    { GMSubCmds::UnBanIP,          "unban/ip"           },
    { GMSubCmds::UnBanSoul,        "unban/soul"         },
    { GMSubCmds::UnBanAll,         "unban/all"          },
    { GMSubCmds::KickIP,           "kick/ip"            },
    { GMSubCmds::KickSoul,         "kick/soul"          },
    { GMSubCmds::KickAll,          "kick/all"           },
    { GMSubCmds::MuteIP,           "mute/ip"            },
    { GMSubCmds::MuteSoul,         "mute/soul"          },
    { GMSubCmds::MuteAll,          "mute/all"           },
    { GMSubCmds::UnMuteIP,         "unmute/ip"          },
    { GMSubCmds::UnMuteSoul,       "unmute/soul"        },
    { GMSubCmds::UnMuteAll,        "unmute/all"         },
    { GMSubCmds::QuarantineIP,     "quarantine/ip"      },
    { GMSubCmds::QuarantineSoul,   "quarantine/soul"    },
    { GMSubCmds::UnQuarantineIP,   "unquarantine/ip"    },
    { GMSubCmds::UnQuarantineSoul, "unquarantine/soul"  },
    { GMSubCmds::MessageIP,        "message/ip"         },
    { GMSubCmds::MessageSoul,      "message/soul"       },
    { GMSubCmds::MessageAll,       "message/all"        },
    { GMSubCmds::ShutDownStop,     "shutdown/stop"      },
    { GMSubCmds::ReStartStop,      "restart/stop"       },
    { GMSubCmds::VanishHide,       "vanish/hide"        },
    { GMSubCmds::VanishShow,       "vanish/show"        },
    { GMSubCmds::VanishStatus,     "vanish/status"      },
    { GMSubCmds::PingIP,           "ping/ip"            },
    { GMSubCmds::PingSoul,         "ping/soul"          },
    { GMSubCmds::CampLock,         "camp/lock"          },
    { GMSubCmds::CampUnLock,       "camp/unlock"        },
    { GMSubCmds::CampAllowAll,     "camp/allowall"      },
    { GMSubCmds::CampAllowCurrent, "camp/allowcurrent"  },
    { GMSubCmds::CampAllow,        "camp/allow"         },
    { GMSubCmds::CampRemove,       "camp/remove"        },
    { GMSubCmds::CampStatus,       "camp/status"        },
    { GMSubCmds::CampSoul,         "camp/soul"          },
};

CmdTableOverride* CmdTableOverride::instance{ nullptr };
bool CmdTableOverride::usingOverrides{ false };

CmdTableOverride::CmdTableOverride(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CmdTableOverride)
{
    ui->setupUi(this);

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        this->restoreGeometry( Settings::getSetting( SKeys::Positions, "AdminCmdOverride" ).toByteArray() );

    ui->overridWidget->setItemDelegate( new CmdTableDelegate( this ) );

    for ( int i = 0; i < 4; ++i )
    {
        ui->overridWidget->resizeColumnToContents( i );
    }

    //Setup our QSettings Object.
    usingOverrides = this->getOverrideFromPath( "CommandRanks/usingOverrides" ).toBool();
    this->handleValues( ui->overridWidget, !usingOverrides ); //If ture, load. Else create default values.

    QObject::connect( ui->overridWidget, &QTreeWidget::itemChanged, this, &CmdTableOverride::itemChangedSlot );
}

CmdTableOverride::~CmdTableOverride()
{
    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        Settings::setSetting( this->saveGeometry(), SKeys::Positions, "AdminCmdOverride" );

    delete ui;
}

CmdTableOverride* CmdTableOverride::getInstance()
{
    if ( instance == nullptr )
        instance = new CmdTableOverride( ReMix::getInstance() );

    return instance;
}

void CmdTableOverride::setInstance(CmdTableOverride* overrides)
{
    instance = overrides;
}

bool CmdTableOverride::getUsingOverrides()
{
    return usingOverrides;
}

void CmdTableOverride::handleValues(QTreeWidget* tree, const bool& save)
{
    if ( tree == nullptr )
        return;

    const QString path{ "CommandRanks/%1/%2/Rank" };
    QString tPath{ "" };

    for ( qint32 i = 0; i < tree->topLevelItemCount(); i++ )
    {
        QTreeWidgetItem* item{ tree->topLevelItem( i ) };
        if ( item == nullptr )
            continue;

        QString topName{ item->text( 0 ) };

        tPath = "CommandRanks/" % topName % "/Rank";
        if ( save )
            this->setOverride( tPath, item->data( 1, Qt::DisplayRole ) );
        else
            item->setData( 1, Qt::DisplayRole, this->getOverrideFromPath( tPath ) );

        for ( qint32 j = 0; j < item->childCount(); ++j )
        {
            const QTreeWidgetItem* child{ item->child( j ) };
            if ( child != nullptr )
            {
                tPath = path.arg( topName )
                            .arg( child->text( 0 ) );

                if ( save )
                    this->setOverride( tPath, child->data( 1, Qt::DisplayRole ) );
                else
                    item->child( j )->setData( 1, Qt::DisplayRole, this->getOverrideFromPath( tPath ) );
            }
        }
    }
}

void CmdTableOverride::setOverride(const QString& path, const QVariant& value)
{
    Settings::setSettingFromPath( path, value );
}

QVariant CmdTableOverride::getOverrideFromPath(const QString& path)
{
    return Settings::getSettingFromPath( path );
}

GMRanks CmdTableOverride::getOverride(const GMCmds& cmd, const GMSubCmds& subCmd)
{
    QString path{ "" };
    if ( GMSubCmds::Invalid == subCmd )
        path = "CommandRanks/" % commands.value( cmd ) % "/Rank";
    else
        path = "CommandRanks/" % subCommands.value( subCmd ) % "/Rank";

    if ( path.isEmpty() )
        return GMRanks::Invalid;

    return static_cast<GMRanks>( getOverrideFromPath( path ).toInt() );
}

void CmdTableOverride::itemChangedSlot(QTreeWidgetItem* item, int column)
{
    QString tPath{ "" };
    QString topName{ "" };

    if ( item != nullptr
      && column == 1 )
    {
        QTreeWidgetItem* topTree{ item->parent() };
        if ( topTree == nullptr )
        {
            topName = item->text( 0 );
            tPath = "CommandRanks/" % topName % "/Rank";
        }
        else
            tPath = "CommandRanks/" % topTree->text( 0 ) % "/" % item->text(0) % "/Rank";

        if ( !tPath.isEmpty() )
        {
            this->setOverride( tPath, item->data( 1, Qt::DisplayRole ).toInt() );
            if ( !usingOverrides )
            {
                usingOverrides = true;
                this->setOverride( "CommandRanks/usingOverrides", true );
            }
        }
    }
}
