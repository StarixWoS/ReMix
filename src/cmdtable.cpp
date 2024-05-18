
#include "cmdtable.hpp"

//Required ReMix Includes.
#include "cmdtableoverride.hpp"
#include "helper.hpp"
#include "player.hpp"

//Required Qt includes
#include <QVector>
#include <QtCore>

CmdTable* CmdTable::instance{ nullptr };
const QVector<CmdTable::CmdStructure> CmdTable::cmdTable =
{
    {   //Command Implemented.
        { "help" },
        1,
        {
            {
            }
        },
        0,
        "Command Description: Shows command information and syntax.",
        "Command Usage: /help *Command *<SubCommand>",
        GMRanks::User,
        true,
        GMCmds::Help,
    },
    {   //Command Implemented.
        { "list", "commands", "cmd" },
        3,
        {
            {
            }
        },
        0,
        "Command Description: Prints all commands the User has access to.",
        "Command Usage: /list", //List does not have a Syntax.
        GMRanks::User,
        true,
        GMCmds::List,
    },
    {   //Command Implemented.
        { "motd" },
        1,
        {
            {
                "change",
                "Command Description: Allows a Remote Administrator to change the Server's Message of the Day.",
                "Command Usage: /motd change *<Message>",
                GMRanks::CoAdmin,
                GMSubCmds::MotDChange,
            },
            {
                "remove",
                "Command Description: Allows a Remote Administrator to remove the Server's Message of the Day.",
                "Command Usage: /motd remove",
                GMRanks::CoAdmin,
                GMSubCmds::MotDRemove,
            }
        },
        2,
        "Command Description: Allows a Remote Administrator to change the Server's Message of the Day.",
        "Command Usage: /motd *<Message>",
        GMRanks::CoAdmin,
        true,
        GMCmds::MotD,
    },
    {   //Server UpTime, Connected Users, Connected Admins.
        { "info" },
        1,
        {
            {
                "server",
                "Command Description: Provides the Server's Ping and User Information.",
                "Command Usage: /info server",
                GMRanks::GMaster,
                GMSubCmds::InfoServer,
            },
            {
                "ip",
                "Command Description: Provides Information on a specific User via IP Address.",
                "Command Usage: /info ip 127.0.0.1",
                GMRanks::Admin,
                GMSubCmds::InfoSoul,
            },
            {
                "soul",
                "Command Description: Provides Information on a specific User via SoulID..",
                "Command Usage: /info soul 4000",
                GMRanks::Admin,
                GMSubCmds::InfoSoul,
            },
            {
                "muted",
                "Command Description: Provides a list of Muted Users.",
                "Command Usage: /info muted",
                GMRanks::GMaster,
                GMSubCmds::InfoMuted,
            },
            {
                "quarantined",
                "Command Description: Provides a list of Quarantined Users.",
                "Command Usage: /info quarantined",
                GMRanks::GMaster,
                GMSubCmds::InfoQuarantined,
            }
        },
        5,
        "Command Description: Provides Server Information for Remote Admins to use.",
        "Command Usage: /info *<SubCommand>",
        GMRanks::GMaster,
        true,
        GMCmds::Info,
    },
    {   //Command Implemented.
        { "ban" },
        1,
        {
            {
                "ip",
                "Command Description: Banish a User via IP Address.",
                "Command Usage: /ban ip 127.0.0.1 *<Duration( <#>s(Seconds) | m(Minutes) | h(Hours) | d(Days) ) *<Reason(Optional)>",
                GMRanks::Admin,
                GMSubCmds::BanIP,
            },
            {
                "soul",
                "Command Description: Banish a User via SoulID.",
                "Command Usage: /ban soul 4000 *<Duration( <#>s(Seconds) | m(Minutes) | h(Hours) | d(Days) ) *<Reason(Optional)>",
                GMRanks::CoAdmin,
                GMSubCmds::BanSoul,
            },
            {
                "all",
                "Command Description: Banish *ALL* connected Users.",
                "Command Usage: /ban all *<Duration( <#>s(Seconds) | m(Minutes) | h(Hours) | d(Days) ) *<Reason(Optional)>",
                GMRanks::Owner,
                GMSubCmds::BanAll,
            }
        },
        3,
        "Command Description: Banish a User from the Server.",
        "Command Usage: /ban *<SubCommand> *<Duration( Optional) > *<Reason(Optional)>",
        GMRanks::CoAdmin,
        true,
        GMCmds::Ban,
    },
    {   //Command Implemented.
        { "unban" },
        1,
        {
            {
                "ip",
                "Command Description: UnBanish a User via IP Address.",
                "Command Usage: /unban ip 127.0.0.1 *<Reason(Optional)>",
                GMRanks::Admin,
                GMSubCmds::UnBanIP,
            },
            {
                "soul",
                "Command Description: UnBanish a User via SoulID.",
                "Command Usage: /unban soul 4000 *<Reason(Optional)>",
                GMRanks::CoAdmin,
                GMSubCmds::BanSoul,
            },
            {
                "all",
                "Command Description: UnBanish *ALL* Users.",
                "Command Usage: /unban all",
                GMRanks::Owner,
                GMSubCmds::UnBanAll,
            }
        },
        3,
        "Command Description: Removes a ban from the selected user and reallows them to connect.",
        "Command Usage: /unban *<SubCommand> *<Reason(Optional)>",
        GMRanks::CoAdmin,
        true,
        GMCmds::UnBan,
    },
    {   //Command Implemented.
        { "kick" },
        1,
        {
            {
                "ip",
                "Command Description: Disconnects a User via IP Address.",
                "Command Usage: /kick ip 127.0.0.1 *<Reason(Optional)>",
                GMRanks::CoAdmin,
                GMSubCmds::MuteIP,
            },
            {
                "soul",
                "Command Description: Disconnects a User via SoulID.",
                "Command Usage: /kick soul 4000 *<Reason(Optional)>",
                GMRanks::GMaster,
                GMSubCmds::MuteSoul,
            },
            {
                "all",
                "Command Description: Disconnects *ALL* Users.",
                "Command Usage: /kick all *<Reason(Optional)>",
                GMRanks::Owner,
                GMSubCmds::MuteAll,
            }
        },
        3,
        "Command Description: Disconnects the selected user from the server.",
        "Command Usage: /kick *<SubCommand> *<Reason(Optional)>",
        GMRanks::GMaster,
        true,
        GMCmds::Kick,
    },
    {   //Command Implemented.
        { "mute" },
        1,
        {
            {
                "ip",
                "Command Description: Adds a network mute to the selected User via IP Address.",
                "Command Usage: /mute ip 127.0.0.1 *<Duration( <#>s(Seconds) | m(Minutes) | h(Hours) | d(Days) ) *<Reason(Optional)>",
                GMRanks::Admin,
                GMSubCmds::MuteIP,
            },
            {
                "soul",
                "Command Description: Adds a network mute to the selected User via SoulID.",
                "Command Usage: /mute soul 4000 *<Duration( <#>s(Seconds) | m(Minutes) | h(Hours) | d(Days) ) *<Reason(Optional)>",
                GMRanks::GMaster,
                GMSubCmds::MuteSoul,
            },
            {
                "all",
                "Command Description: Adds a network mute to *ALL* Users.",
                "Command Usage: /mute all *<Duration( <#>s(Seconds) | m(Minutes) | h(Hours) | d(Days) ) *<Reason(Optional)>",
                GMRanks::Owner,
                GMSubCmds::MuteAll,
            }
        },
        3,
        "Command Description: Adds a network mute to the selected User.",
        "Command Usage: /mute *<SubCommand> *<Duration(Optional)> *<Reason(Optional)>",
        GMRanks::GMaster,
        true,
        GMCmds::Mute,
    },
    {   //Command Implemented.
        { "unmute" },
        1,
        {
            {
                "ip",
                "Command Description: Removes a network mute imposed on the selected User via IP Address.",
                "Command Usage: /unmute ip 127.0.0.1 *<Reason(Optional)>",
                GMRanks::Admin,
                GMSubCmds::UnMuteIP,
            },
            {
                "soul",
                "Command Description: Removes a network mute imposed on the selected User via SoulID.",
                "Command Usage: /unmute soul 4000 *<Reason(Optional)>",
                GMRanks::GMaster,
                GMSubCmds::UnMuteSoul,
            },
            {
                "all",
                "Command Description: Removes a network mute imposed on *ALL* Users.",
                "Command Usage: /unmute all *<Reason(Optional)>",
                GMRanks::Owner,
                GMSubCmds::UnMuteAll,
            }
        },
        3,
        "Command Description: Removes a network mute imposed on the selected User.",
        "Command Usage: /unmute *<SubCommand> *<Reason(Optional)>",
        GMRanks::GMaster,
        true,
        GMCmds::UnMute,
    },
    {   //Command Implemented.
        { "quarantine" },
        1,
        {
            {
                "ip",
                "Command Description: Adds a network quarantine imposed on the selected User via IP Address.",
                "Command Usage: /quarantine ip 127.0.0.1 *<Reason(Optional)>",
                GMRanks::Admin,
                GMSubCmds::QuarantineIP,
            },
            {
                "soul",
                "Command Description: Adds a network quarantine imposed on the selected User via SoulID.",
                "Command Usage: /quarantine soul 4000 *<Reason(Optional)>",
                GMRanks::GMaster,
                GMSubCmds::QuarantineSoul,
            }
        },
        2,
        "Command Description: Adds a network quarantine to the selected User. Note: Quarantined Users can only interact with other Quarantined Users.",
        "Command Usage: /quarantine *<SubCommand> *<Reason(Optional)>",
        GMRanks::GMaster,
        true,
        GMCmds::Quarantine,
    },
    {   //Command Implemented.
        { "unquarantine" },
        1,
        {
            {
                "ip",
                "Command Description: Removes a network quarantine imposed on the selected User via IP Address.",
                "Command Usage: /unquarantine ip 127.0.0.1 *<Reason(Optional)>",
                GMRanks::Admin,
                GMSubCmds::UnQuarantineIP,
            },
            {
                "soul",
                "Command Description: Removes a network quarantine imposed on the selected User via SoulID.",
                "Command Usage: /unquarantine soul 4000 *<Reason(Optional)>",
                GMRanks::GMaster,
                GMSubCmds::UnQuarantineSoul,
            }
        },
        2,
        "Command Description: Removes a network quarantine from the selected User.",
        "Command Usage: /unquarantine *<SubCommand> *<Reason(Optional)>",
        GMRanks::GMaster,
        true,
        GMCmds::UnQuarantine,
    },
    {   //Command Implemented.
        { "msg", "message" },
        2,
        {
            {
                "ip",
                "Command Description: Messages the selected User via IP Address.",
                "Command Usage: /msg ip 127.0.0.1 *<Message>",
                GMRanks::Admin,
                GMSubCmds::MessageIP,
            },
            {
                "soul",
                "Command Description: Messages the selected User via SoulID.",
                "Command Usage: /msg soul 4000 *<Message>",
                GMRanks::GMaster,
                GMSubCmds::MessageSoul,
            },
            {
                "all",
                "Command Description: Messages *ALL* Users.",
                "Command Usage: /msg all *<Message>",
                GMRanks::GMaster,
                GMSubCmds::MessageAll,
            }
        },
        3,
        "Command Description: Sends a message to the selected User.",
        "Command Usage: /msg *<SubCommand> *<Message>",
        GMRanks::GMaster,
        true,
        GMCmds::Message,
    },
    {   //Command Implemented.
        { "login" },
        1,
        {
            {
            }
        },
        0,
        "Command Description: Allows a User to authenticate with the server.",
        "Command Usage: /login *<Password>",
        GMRanks::User,
        true,
        GMCmds::LogIn,
    },
    {   //Command Implemented.
        { "logout" },
        1,
        {
            {
            }
        },
        0,
        "Command Description: Allows a User to de-authenticate.",
        "Command Usage: /logout",
        GMRanks::GMaster,
        true,
        GMCmds::LogOut,
    },
    {   //Command Implemented.
        { "register" },
        1,
        {
            {
            }
        },
        0,
        "Command Description: Allows User to register with the server as a Remote Administrator.",
        "Command Usage: /register *<Password>",
        GMRanks::User,
        true,
        GMCmds::Register,
    },
    {   //Command Implemented.
        { "shutdown" },
        1,
        {
            {
                "stop",
                "Command Description: Allows a Remote Admin to halt a Server Shutdown.",
                "Command Usage: /shutdown stop *<Reason(Optional)>",
                GMRanks::Owner,
                GMSubCmds::ShutDownStop,
            }
        },
        1,
        "Command Description: Initiates server shutdown in <n>s(Seconds) | m(Minutes) | h(Hours) | d(Days) (30 Seconds if duration is not provided).",
        "Command Usage: /shutdown *<SubCommand> *<Duration( <#>s(Seconds) | m(Minutes) | h(Hours) | d(Days) ) *<Reason(Optional)>",
        GMRanks::Owner,
        true,
        GMCmds::ShutDown,
    },
    {   //Command Implemented.
        { "restart" },
        1,
        {
            {
                "stop",
                "Command Description: Allows a Remote Admin to halt a Server Restart.",
                "Command Usage: /restart stop *<Reason(Optional)>",
                GMRanks::Admin,
                GMSubCmds::ReStartStop,
            }
        },
        1,
        "Command Description: Initiates server restart in <n>s(Seconds) | m(Minutes) | h(Hours) | d(Days) (30 Seconds if duration is not provided).",
        "Command Usage: /restart *<SubCommand> *<Duration( <#>s(Seconds) | m(Minutes) | h(Hours) | d(Days) ) *<Reason(Optional)>",
        GMRanks::Admin,
        true,
        GMCmds::ReStart,
    },
    {   //Command Unimplemented. Would require a restart or for currently connected Users to reconnect.
        { "chrules" },
        1,
        {
            {
            }
        },
        0,
        "Command Description: Modifies the Server's rule set.",
        "",
        GMRanks::Admin,
        false,
        GMCmds::Invalid,
    },
    {   //Command Unimplemented.
        { "chsettings" },
        1,
        {
            {
            }
        },
        0,
        "Command Description: Modifies the Server's settings.",
        "",
        GMRanks::Admin,
        false,
        GMCmds::Invalid,
    },
    {   //Command Implemented.
        { "vanish" },
        1,
        {
            {
                "hide",
                "Command Description: The Remote Admin will become invisible to other Users.",
                "Command Usage: /vanish hide",
                GMRanks::GMaster,
                GMSubCmds::VanishHide,
            },
            {
                "show",
                "Command Description: The Remote Admin will become visible to other Users.",
                "Command Usage: /vanish show",
                GMRanks::GMaster,
                GMSubCmds::VanishShow,
            },
            {
                "status",
                "Command Description: Will inform the Remote Admin of their vanish status.",
                "Command Usage: /vanish status",
                GMRanks::GMaster,
                GMSubCmds::VanishStatus,
            }
        },
        3,
        "Command Description: Allows a Remote Admin to become invisible to others.",
        "Command Usage: /vanish *<SubCommand(Optional)>",
        GMRanks::GMaster,
        true,
        GMCmds::Vanish,
    },
    {   //Command Implemented.
        { "version" },
        1,
        {
            {
            }
        },
        0,
        "Command Description: Shows the Servers Version Information.",
        "Command Usage: /version",
        GMRanks::User,
        true,
        GMCmds::Version,
    },
    {   //Command Implemented.
        { "ping" },
        1,
        {
            {
                "ip",
                "Command Description: Shows the Ping Information the selected User via IP Address.",
                "Command Usage: /ping ip 127.0.0.1",
                GMRanks::Admin,
                GMSubCmds::PingIP,
            },
            {
                "soul",
                "Command Description: Shows the Ping Information the selected User via SoulID.",
                "Command Usage: /ping soul 4000",
                GMRanks::GMaster,
                GMSubCmds::PingSoul,
            },
        },
        2,
        "Command Description: Shows your Ping Information.",
        "Command Usage: /ping",
        GMRanks::User,
        true,
        GMCmds::Ping,
    },
    {
        { "camp" },
        1,
        {
            {
                "lock",
                "Command Description: The User's Scene will be locked to all not *Allowed*.",
                "Command Usage: /camp lock",
                GMRanks::User,
                GMSubCmds::CampLock,
            },
            {
                "unlock",
                "Command Description: The User's Scene will be unlocked.",
                "Command Usage: /camp unlock",
                GMRanks::User,
                GMSubCmds::CampUnLock,
            },
            {
                "allowcurrent",
                "Command Description: The User's Scene will be unlocked only for Users currently connected.",
                "Command Usage: /camp allowcurrent",
                GMRanks::User,
                GMSubCmds::CampAllowCurrent,
            },
            {
                "allowall",
                "Command Description: The User's Scene will be unlocked for all current and future Users.",
                "Command Usage: /camp allowall",
                GMRanks::User,
                GMSubCmds::CampAllowAll,
            },
            {
                "allow",
                "Command Description: The User may allow another User to bypass any restrictions.",
                "Command Usage: /camp allow soul 4000",
                GMRanks::User,
                GMSubCmds::CampAllow,
            },
            {
                "remove",
                "Command Description: The User may revoke another User's bypass permissions.",
                "Command Usage: /camp remove soul 4000",
                GMRanks::User,
                GMSubCmds::CampRemove,
            },
            {
                "status",
                "Command Description: Provides the status of the User's Camp.",
                "Command Usage: /camp status soul 4000",
                GMRanks::User,
                GMSubCmds::CampStatus,
            },
            {
                "soul",
                "Command Description: Remote Admins may override a User's limitations.",
                "Command Usage: /camp *<SubCommand> soul 4000",
                GMRanks::Admin,
                GMSubCmds::CampSoul,
            },
        },
        8,
        "Command Description: Allows a User to prevent others from entering a hosted Scene.",
        "Command Usage: /camp *<SubCommand>",
        GMRanks::User,
        true,
        GMCmds::Camp,
    },
    {
        { "guild" },
        1,
        {
            {
            },
            {
            },
            {
            },
            {
            }
        },
        4,
        "Command Description: ",
        "Command Usage: /guild [ *<Message> | create | join | kick | leave ]. e.g. (/guild *chat), (/guild create *Name) ",
        GMRanks::User,
        false,
        GMCmds::Guild,
    },
};

CmdTable* CmdTable::getInstance()
{
    if ( instance == nullptr )
        instance = new CmdTable();

    return instance;
}

bool CmdTable::cmdIsActive(const GMCmds& index)
{
    return cmdTable.at( *index ).cmdIsActive;
}

bool CmdTable::isSubCommand(const GMCmds& index, const QString& cmd, const bool& time)
{
    if ( index == GMCmds::Invalid )
        return false;

    const CmdTable::CmdStructure& cmdAt{ cmdTable.at( *index ) };
    if ( !time )
    {
        for ( const auto& item : cmdAt.subCmd )
        {
            if ( Helper::cmpStrings( item.subCommand, cmd ) )
                return true;
        }
    }
    return false;
}

bool CmdTable::getCmdHasSubCmd(const GMCmds& index)
{
    if ( index == GMCmds::Invalid )
        return false;

    return cmdTable.at( *index ).subCmdCount >= 1;
}

QString CmdTable::getCmdNames(const GMCmds& index)
{
    QString retList;
    for ( qint32 i = 0; i < cmdTable.at( *index ).cmdActivatorCount; ++i )
    {
        retList.append( cmdTable.at( *index ).cmdActivators.at( i ) );
        if ( cmdTable.at( *index ).cmdActivatorCount >= 2 )
        {
            if ( i != cmdTable.at( *index ).cmdActivatorCount )
                retList.append( ", " );
        }
    }
    return retList;
}

GMCmds CmdTable::getCmdIndex(const QString& cmd)
{
    GMCmds index{ GMCmds::Invalid };
    qint32 idx{ -1 };
    for ( const CmdTable::CmdStructure& el : cmdTable )
    {
        ++idx;
        //Check the current Object if it contains our command information,
        for ( const auto& item : el.cmdActivators )
        {
            if ( Helper::cmpStrings( item, cmd ) )
            {
                //Make Certain that the command is Activated.
                if ( el.cmdIsActive )
                    index = static_cast<GMCmds>( idx );
            }
        }
    }
    return index;
}

GMSubCmdIndexes CmdTable::getSubCmdIndex(const GMCmds& cmdIndex, const QString& subCmd, const bool& time)
{
    qint32 index{ *GMSubCmdIndexes::Invalid };
    if ( cmdIndex == GMCmds::Invalid )
        return GMSubCmdIndexes::Invalid;

    const CmdTable::CmdStructure& cmdAt{ cmdTable.at( *cmdIndex ) };
    if ( !time )
    {
        qint32 sCmdIdx{ -1 };
        for ( const auto& el : cmdAt.subCmd )
        {
            ++sCmdIdx;
            if ( Helper::cmpStrings( el.subCommand, subCmd ) )
                index = sCmdIdx;
        }
    }
    return static_cast<GMSubCmdIndexes>( index );
}

GMRanks CmdTable::getCmdRank(const GMCmds& index)
{
    if ( index == GMCmds::Invalid )
        return GMRanks::Invalid;

    if ( this->cmdIsActive( index ) )
    {
        if ( CmdTableOverride::getUsingOverrides() )
            return CmdTableOverride::getOverride( index, GMSubCmds::Invalid );
        else
            return cmdTable.at( *index ).cmdRank;
    }

    //The command is inactive. Return Rank Invalid.
    return GMRanks::Invalid;
}

GMRanks CmdTable::getSubCmdRank(const GMCmds& index, const GMSubCmdIndexes& subIndex)
{
    if ( index == GMCmds::Invalid )
        return GMRanks::Invalid;

    if ( !this->cmdIsActive( index ) )
        return GMRanks::Invalid;

    if ( CmdTableOverride::getUsingOverrides() )
        return CmdTableOverride::getOverride( index, cmdTable.at( *index ).subCmd.at( *subIndex ).index );
    else
        return cmdTable.at( *index ).subCmd.at( *subIndex ).subRank;

    return GMRanks::Invalid;
}

QString CmdTable::getCmdString(const CmdTable::CmdStructure& cmdStruct, const GMRanks rank, const bool isAuth)
{
    QString result{ "" };
    if ( !isAuth
      && cmdStruct.cmdRank > GMRanks::User ) //Unauthenticated should only get a basic list.
    {
        return result;
    }

    if ( this->getCmdRank( cmdStruct.index ) <= rank
      && cmdStruct.cmdIsActive )
    {
        GMSubCmdIndexes subCmdIndex{ GMSubCmdIndexes::Invalid };
        GMRanks tSubCmdRank{ GMRanks::Invalid };

        qint32 activatorCount{ cmdStruct.cmdActivatorCount };
        qint32 subCmdCount{ cmdStruct.subCmdCount };

        result.append( "< " );
        for ( const auto& item : cmdStruct.cmdActivators )
        {
            result.append( item );
            if ( --activatorCount > 0 )
                result.append( " | " );
        }

        result.append( " >" );
        if ( cmdStruct.subCmdCount == 0 )
            result.append( ", " );

        activatorCount = cmdStruct.cmdActivatorCount;
        if ( cmdStruct.subCmdCount >= 1 )
        {
            result.append( "[ " );
            for ( const auto& sEl : cmdStruct.subCmd )
            {
                if ( CmdTableOverride::getUsingOverrides() )
                {
                    subCmdIndex = this->getSubCmdIndex( cmdStruct.index, sEl.subCommand );
                    tSubCmdRank = this->getSubCmdRank( cmdStruct.index, subCmdIndex );
                }
                else
                    tSubCmdRank = sEl.subRank;

                if ( tSubCmdRank <= rank )
                {
                    result.append( sEl.subCommand );
                    if ( --subCmdCount > 0 )
                        result.append( ", " );
                }
            }
            result.append( " ], " );
        }
    }
    return result;
}

QString CmdTable::collateCmdStrings(const QSharedPointer<Player> admin)
{
    QString list{ "Available Command list: " };
    GMRanks rank{ admin->getAdminRank() };
    bool isAuth{ admin->getAdminPwdReceived() };

    for ( const CmdTable::CmdStructure& el : cmdTable )
    {
        list.append( getCmdString( el, rank, isAuth ) );
    }
    return list;
}

QString CmdTable::getCommandInfo(const GMCmds& index, const GMSubCmdIndexes& subIndex, const bool& syntax)
{
    qint32 idx{ *index };
    if ( idx < 0 )
        idx = *GMCmds::Help;

    if ( subIndex == GMSubCmdIndexes::Invalid )
    {
        if ( syntax )
            return cmdTable.at( idx ).cmdSyntax;

        return cmdTable.at( idx ).cmdInfo;
    }

    if ( syntax )
        return cmdTable.at( idx ).subCmd.at( *subIndex ).subSyntax;

    return cmdTable.at( idx ).subCmd.at( *subIndex ).subInfo;
}
