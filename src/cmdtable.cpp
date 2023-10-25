
#include "cmdtable.hpp"

//Required ReMix Includes.
#include "helper.hpp"
#include "player.hpp"

//Required Qt includes.
#include <QtCore>

CmdTable* CmdTable::instance{ nullptr };
const QVector<CmdTable::CmdStruct> CmdTable::cmdTable =
{
    {   //Command Implemented.
        { "help" },
        1,
        { },
        0,
        "Command Description: Shows command information and syntax.",
        "Command Usage: /help *Command"
        "e.g. (/help help) will show the command description and format!",
        GMRanks::User,
        true,
        GMCmds::Help,
    },
    {   //Command Implemented.
        { "list" },
        1,
        { },
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
        { "change", "remove", },
        2,
        "Command Description: Sets the Server's Message of the Day.",
        "Command Usage: /motd [ change | remove ] *Message. "
        "e.g. (/motd change No cheating!) or (/motd remove remove) to disable the MotD.",
        GMRanks::CoAdmin,
        true,
        GMCmds::MotD,
    },
    {   //Server UpTime, Connected Users, Connected Admins.
        { "info" },
        1,
        { "server", "soul", "muted", "quarantined" },
        4,
        "Command Description: Shows the Server or User Information.",
        "Command Usage: /info [ server | soul | muted | quarantined ] e.g. \"/info soul 4000\".",
        GMRanks::GMaster,
        true,
        GMCmds::Info,
    },
    {   //Command Implemented.
        { "ban" },
        1,
        { "soul", "ip", "all"  },
        3,
        "Command Description: Bans the selected user and prevents their future connection to the server.",
        "Command Usage: /ban [ Soul | IP | All (Permission Required) ] <#>s(Seconds) | m(Minutes) | h(Hours) | d(Days) *<Reason(Optional)>. "
        "(Duration is default to 30 Days if not provided). e.g. (/ban soul 4000 Bad Soul!) or (/ban soul 4000 30m Bad Soul!)",
        GMRanks::CoAdmin,
        true,
        GMCmds::Ban,
    },
    {   //Command Implemented.
        { "unban" },
        1,
        { "soul", "ip", "all" },
        3,
        "Command Description: Removes a ban from the selected user and reallows them to connect.",
        "Command Usage: /unban [ Soul | IP | All (Permission Required) ]*Reason (Optional). "
        "e.g. (/unban soul 4000 Good behavior) or (/unban ip 10.0.0.1 Good behavior.)",
        GMRanks::CoAdmin,
        true,
        GMCmds::UnBan,
    },
    {   //Command Implemented.
        { "kick" },
        1,
        { "soul", "ip", "all" },
        3,
        "Command Description: Disconnects the selected user from the server.",
        "Command Usage: /kick [ Soul | IP | All (Permission Required) ]*Reason (Optional). "
        "e.g. (/kick soul 4000 Booted!) or (/kick ip 10.0.0.1 Booted!.)",
        GMRanks::GMaster,
        true,
        GMCmds::Kick,
    },
    {   //Command Implemented.
        { "mute" },
        1,
        { "soul", "ip", "all" },
        3,
        "Command Description: Adds a network mute to the selected User.",
        "Command Usage: /mute [ Soul | IP | All (Permission Required) ] <#>s(Seconds) | m(Minutes) | h(Hours) | d(Days) *<Reason(Optional)>. "
        "(Duration is default 10 Minutes if not provided). e.g. (/mute soul 4000 Bad Soul!) or (/mute soul 4000 30m Bad Soul!)",
        GMRanks::GMaster,
        true,
        GMCmds::Mute,
    },
    {   //Command Implemented.
        { "unmute" },
        1,
        { "soul", "ip", "all" },
        3,
        "Command Description: Removes a network mute imposed on the selected User.",
        "Command Usage: /unmute [ Soul | IP | All ] (Permission Required) *Reason (Optional). "
        "e.g. (/unmute soul 4000 Good behavior) or (/unmute ip 10.0.0.1 Good behavior.)",
        GMRanks::GMaster,
        true,
        GMCmds::UnMute,
    },
    {   //Command Implemented.
        { "quarantine" },
        1,
        { "soul", "ip" },
        2,
        "Command Description: Adds a network quarantine to the selected User. Quarantined Users can only interact with other Quarantined Users.",
        "Command Usage: /quarantine [ Soul | IP ] *Reason (Optional). "
        "e.g. (/quarantine soul 4000 Good behavior) or (/quarantine ip 10.0.0.1 Good behavior.)",
        GMRanks::GMaster,
        true,
        GMCmds::Quarantine,
    },
    {   //Command Implemented.
        { "unquarantine" },
        1,
        { "soul", "ip" },
        2,
        "Command Description: Removes a network quarantine to the selected User. Quarantined Users can only interact with other Quarantined Users.",
        "Command Usage: /unquarantine [ Soul | IP ] *Reason (Optional). "
        "e.g. (/unquarantine soul 4000 Good behavior) or (/unquarantine ip 10.0.0.1 Good behavior.)",
        GMRanks::GMaster,
        true,
        GMCmds::UnQuarantine,
    },
    {   //Command Implemented.
        { "msg", "message" },
        2,
        { "soul", "ip", "all" },
        3,
        "Command Description: Sends a message to the selected User.",
        "Command Usage: /msg [ Soul | IP | All ] (Permission Required) *Message. "
        "e.g. (/msg soul 4000 Hello.) or (/msg ip 10.0.0.1 Hello.)",
        GMRanks::GMaster,
        true,
        GMCmds::Message,
    },
    {   //Command Implemented.
        { "login" },
        1,
        { },
        0,
        "Command Description: Input command from the User to authenticate with the server.",
        "Command Usage: /login *Password",
        GMRanks::User,
        true,
        GMCmds::LogIn,
    },
    {   //Command Implemented.
        { "logout" },
        1,
        { },
        0,
        "Command Description: De-authenticates the User that issues the command. The User will not be able to use Admin Commands.",
        "Command Usage: /logout",
        GMRanks::GMaster,
        true,
        GMCmds::LogOut,
    },
    {   //Command Implemented.
        { "register" },
        1,
        { },
        0,
        "Command Description: Input command from the User to register with the server as a Remote Administrator.",
        "Command Usage: /register *Password",
        GMRanks::User,
        true,
        GMCmds::Register,
    },
    {   //Command Implemented.
        { "shutdown" },
        1,
        { "stop" },
        1,
        "Command Description: Initiates server shutdown in <n>s(Seconds) | m(Minutes) | h(Hours) | d(Days) (30 Seconds if duration is not provided).",
        "Command Usage: /shutdown [ <n>s(Seconds) | m(Minutes) | h(Hours) | d(Days) *<Reason(Optional)> ]. e.g. (/shutdown), (/shutdown Seconds 30) "
        "will cause the server to shutdown in 30 seconds, (/shutdown stop) will cease an inprogress shutdown.",
        GMRanks::Owner,
        true,
        GMCmds::ShutDown,
    },
    {   //Command Implemented.
        { "restart" },
        1,
        { "stop" },
        1,
        "Command Description: Initiates server restart in <n>s(Seconds) | m(Minutes) | h(Hours) | d(Days) (30 Seconds if duration is not provided).",
        "Command Usage: /restart [ <n>s(Seconds) | m(Minutes) | h(Hours) | d(Days) *<Reason(Optional)> ]. e.g. (/restart), (/restart 30s) "
        "will cause the server to shutdown in 30 seconds, (/restart stop) will cease an inprogress restart.",
        GMRanks::Admin,
        true,
        GMCmds::ReStart,
    },
    {   //Command Unimplemented. Would require a restart or for currently connected Users to reconnect.
        { "chrules" },
        1,
        { },
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
        { },
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
        { "hide", "show", "status" },
        3,
        "Command Description: Makes the Admin invisible to others. Poof!",
        "Command Usage: /vanish [ hide | show | status ]. When no sub-command is entered the command acts as an on|off toggle.",
        GMRanks::GMaster,
        true,
        GMCmds::Vanish,
    },
    {   //Command Implemented.
        { "version" },
        1,
        { },
        0,
        "Command Description: Shows the Servers Version Information.",
        "Command Usage: /version",
        GMRanks::User,
        true,
        GMCmds::Version,
    },
    {
        { "camp" },
        1,
        { "lock", "unlock", "allowcurrent", "allowall", "allow", "remove" },
        6,
        "Command Description: Using \"lock\" prevents other Players from entering a scene hosted by you and \"unlock\" reverses that limitation. "
        "If enabling \"allowcurrent\" then only Players online when the Scene was created can enter and the command \"allowall\" reverses that limitation. "
        "Using the command syntax \"/camp allow soul 4000\" or \"/camp remove soul 4000\" you may exempt specific Players from any limitations.",
        "Command Usage: /camp [ lock | unlock | allowcurrent | allowall | allow ]. Remote Administrators may append \"soul *PlayerSerNum\" to override a "
        "Player's status. e.g. \"/camp lock soul 4000\".",
        GMRanks::User,
        true,
        GMCmds::Camp,
    },
    {
        { "guild" },
        1,
        { "create", "join", "kick", "leave" },
        3,
        "Command Description: ",
        "Command Usage: /guild [ *message | create | join | kick | leave ]. e.g. (/guild *chat), (/guild create *Name) ",
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
    qint32 idx{ *index };
    const CmdTable::CmdStruct& cmdAt{ cmdTable.at( idx ) };

    if ( !time )
    {
        for ( const auto& item : cmdAt.subCmd )
        {
            if ( Helper::cmpStrings( item, cmd ) )
                return true;
        }
    }
    return false;
}

bool CmdTable::getCmdHasSubCmd(const GMCmds& index)
{
    return cmdTable.at( *index ).subCmdCount >= 1;
}

QStringList CmdTable::getCmdName(const GMCmds& index)
{
    return cmdTable.at( *index ).cmdActivators;
}

GMCmds CmdTable::getCmdIndex(const QString& cmd)
{
    GMCmds index{ GMCmds::Invalid };
    qint32 idx{ -1 };
    for ( const CmdTable::CmdStruct& el : cmdTable )
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

GMSubCmds CmdTable::getSubCmdIndex(const GMCmds& cmdIndex, const QString& subCmd, const bool& time)
{
    qint32 index{ *GMSubCmds::Invalid };
    qint32 cmdIdx{ *cmdIndex };
    const CmdTable::CmdStruct& cmdAt{ cmdTable.at( cmdIdx ) };
    if ( !time )
    {
        qint32 sCmdIdx{ -1 };
        for ( const QString& el : cmdAt.subCmd )
        {
            ++sCmdIdx;
            if ( Helper::cmpStrings( el, subCmd ) )
                index = sCmdIdx;
        }
    }
    return static_cast<GMSubCmds>( index );
}

GMRanks CmdTable::getCmdRank(const GMCmds& index)
{
    if ( index == GMCmds::Invalid )
        return GMRanks::Invalid;

    if ( this->cmdIsActive( index ) )
        return cmdTable.at( *index ).cmdRank;

    //The command is inactive. Return Rank Invalid.
    return GMRanks::Invalid;
}

QString CmdTable::getCmdString(const CmdTable::CmdStruct& cmdStruct, const GMRanks rank, const bool isAuth)
{
    QString result{ "" };
    if ( isAuth
      && cmdStruct.cmdRank > GMRanks::User ) //Unauthenticated should only get a basic list.
    {
        return result;
    }

    if ( cmdStruct.cmdRank <= rank
      && cmdStruct.cmdIsActive )
    {
        for ( const auto& item : cmdStruct.cmdActivators )
        {
            result.append( item );
            if ( cmdStruct.subCmdCount > 0 )
            {
                result.append( "[ " );
                for ( const QString& sEl : cmdStruct.subCmd )
                {
                    result.append( sEl % ", " );
                }
                result.append( " ]" );
            }
            result.append( ", " );
        }
    }
    return result;
}

QString CmdTable::collateCmdStrings(const QSharedPointer<Player> admin)
{
    QString list{ "Available Command list: " };
    GMRanks rank{ admin->getAdminRank() };
    bool isAuth{ admin->getAdminPwdReceived() };

    for ( const CmdTable::CmdStruct& el : cmdTable )
    {
        list.append( getCmdString( el, rank, isAuth ) );
    }
    return list;
}

QString CmdTable::getCommandInfo(const GMCmds& index, const bool& syntax)
{
    qint32 idx{ *index };
    if ( idx < 0 )
        idx = *GMCmds::Help;

    if ( syntax )
        return cmdTable.at( idx ).cmdSyntax;

    return cmdTable.at( idx ).cmdInfo;
}
