
#include "cmdtable.hpp"

//Required ReMix Includes.
#include "helper.hpp"

//Required Qt includes.
#include <QtCore>

CmdTable* CmdTable::instance{ nullptr };
const CmdTable::CmdStruct CmdTable::cmdTable[ GM_COMMAND_COUNT ] =
{
    {   //Command Implemented.
        "help",
        { "format", "", "", "" },
        1,
        "Help Description: Shows command information and syntax. "
        "e.g. (/help help) and (/help help format)",
        "Help Usage: /help *Command *format. "
        "e.g. (/help help) will show the command description, "
        "and (/help help format) will show the command format.",
        GMRanks::User,
        true,
    },
    {   //Command Implemented.
        "list",
        { "", "", "", "" },
        0,
        "List Description: Prints all commands the User has access to.",
        "List Usage: /list", //List does not have a Syntax.
        GMRanks::User,
        true,
    },
    {   //Command Implemented.
        "motd",
        { "change", "remove","", "" },
        2,
        "MotD Description: Sets the Server's Message of the Day.",
        "MotD Usage: /motd change|remove *Message. "
        "e.g. (/motd change No cheating!) or (/motd remove remove) to disable the MotD.",
        GMRanks::CoAdmin,
        true,
    },
    {   //Command Unimplemented.
        //Server UpTime, Connected Users, Connected Admins.
        "info",
        { "", "", "", "" },
        0,
        "Info Description: Shows the Server Information.",
        "",
        GMRanks::Admin,
        false,
    },
    {   //Command Unimplemented.
        //Server Network information. IP, Ping, Bandwidth Used.
        "netstatus",
        { "", "", "", "" },
        0,
        "NetStatus Description: Shows the Server's network status.",
        "",
        GMRanks::Admin,
        false,
    },
    {   //Command Implemented.
        //TODO: Allow the Admin to select a duration for the ban.
        "ban",
        { "soul", "ip", "all", "" },
        3,
        "Ban Description: Bans the selected user and prevents their future connection to the server.",
        "Ban Usage: /ban Soul|IP|All(Permission Required) *Reason (Optional). "
        "e.g. (/ban soul 4000 Bad behavior) or (/ban ip 10.0.0.1 Bad behavior.)",
        GMRanks::CoAdmin,
        true,
    },
    {   //Command Implemented.
        "unban",
        { "soul", "ip", "all", "" },
        3,
        "UnBan Description: Removes a ban from the selected user and reallows them to connect.",
        "Unban Usage: /unban Soul|IP|All(Permission Required) *Reason (Optional). "
        "e.g. (/unban soul 4000 Good behavior) or (/unban ip 10.0.0.1 Good behavior.)",
        GMRanks::CoAdmin,
        true,
    },
    {   //Command Implemented.
        "kick",
        { "soul", "ip", "all", "" },
        3,
        "Kick Description: Disconnects the selected user from the server.",
        "Kick Usage: /kick Soul|IP|All(Permission Required) *Reason (Optional). "
        "e.g. (/kick soul 4000 Booted!) or (/kick ip 10.0.0.1 Booted!.)",
        GMRanks::GMaster,
        true,
    },
    {   //Command Implemented.
        //TODO: Allow the Admin to select a duration for the Mute.
        "mute",
        { "soul", "ip", "all", "" },
        3,
        "Mute Description: Adds a network mute to the selected User.",
        "Mute Usage: /mute Soul|IP|All(Permission Required) *Reason (Optional). "
        "e.g. (/mute soul 4000 Bad behavior) or (/mute ip 10.0.0.1 Bad behavior.)",
        GMRanks::GMaster,
        true,
    },
    {   //Command Implemented.
        "unmute",
        { "soul", "ip", "all", "" },
        3,
        "UnMute Description: Removes a network mute imposed on the selected User.",
        "Unmute Usage: /unmute Soul|IP|All(Permission Required) *Reason (Optional). "
        "e.g. (/unmute soul 4000 Good behavior) or (/unmute ip 10.0.0.1 Good behavior.)",
        GMRanks::GMaster,
        true,
    },
    {   //Command Implemented.
        //TODO: Add alternate initializer word "message".
        "msg",
        { "soul", "ip", "all", "" },
        3,
        "Msg Description: Sends a message to the selected User.",
        "Message Usage: /msg Soul|IP|All(Permission Required) *Message. "
        "e.g. (/msg soul 4000 Hello.) or (/msg ip 10.0.0.1 Hello.)",
        GMRanks::GMaster,
        true,
    },
    {   //Command Implemented.
        "login",
        { "", "", "", "" },
        0,
        "Login Description: Input command from the User to authenticate with the server.",
        "Login Usage: /login *Password",
        GMRanks::User,
        true,
    },
    {   //Command Implemented.
        "register",
        { "", "", "", "" },
        0,
        "Register Description: Input command from the User to register with the server as a Remote Administrator.",
        "Register Usage: /register *Password",
        GMRanks::User,
        true,
    },
    {   //Command Implemented.
        "shutdown",
        { "stop", "seconds", "minutes", "hours" },
        4,
        "Shutdown Description: Initiates server shutdown in Seconds|Minutes|Hours <x> (30 Seconds by default).",
        "Shutdown Usage: /shutdown Stop|Seconds|Minutes|Hours <n>. e.g. (/shutdown), (/shutdown Seconds 30) "
        "will cause the server to shutdown in 30 seconds, (/shutdown stop) will cease an inprogress shutdown.",
        GMRanks::Admin,
        true,
    },
    {   //Command Implemented.
        "restart",
        { "stop", "seconds", "minutes", "hours" },
        4,
        "Restart Description: Initiates server restart in Stop|Seconds|Minutes|Hours <x> (30 Seconds by default).",
        "Restart Usage: /restart Stop|Seconds|Minutes|Hours <n>. e.g. (/restart), (/restart Seconds 30) "
        "will cause the server to shutdown in 30 seconds, (/restart stop) will cease an inprogress restart.",
        GMRanks::Admin,
        true,
    },
    {   //Command Unimplemented.
        "mkadmin",
        { "", "", "", "" },
        0,
        "MKAdmin Description: Allows the selected User to authenticate with the server as a Remote Administrator.",
        "",
        GMRanks::Owner,
        false,
    },
    {   //Command Unimplemented.
        "rmadmin",
        { "", "", "", "" },
        0,
        "RMAdmin Description: Disallows the selected User to authenticate with the server as a Remote Administrator.",
        "",
        GMRanks::Owner,
        false,
    },
    {   //Command Unimplemented.
        "chadmin",
        { "", "", "", "" },
        0,
        "CHAdmin Description: Changes the Remote Administrator rank of the selected User.",
        "",
        GMRanks::Owner,
        false,
    },
    {   //Command Unimplemented.
        "chrules",
        { "", "", "", "" },
        0,
        "CHRules Description: Modifies the Server's rule set",
        "",
        GMRanks::Admin,
        false,
    },
    {   //Command Unimplemented.
        "chsettings",
        { "", "", "", "" },
        0,
        "CHSettings Description: Modifies the Server's settings.",
        "",
        GMRanks::Admin,
        false,
    },
    {   //Command Implemented.
        "vanish",
        { "hide", "show", "status", "" },
        3,
        "Vanish Description: Makes the Admin invisible to others. Poof!",
        "Vanish Usage: /vanish hide|show|status. When no sub-command is "
        "entered the command acts as an on|off toggle.",
        GMRanks::GMaster,
        true,
    },
    {   //Command Implemented.
        "version",
        { "", "", "", "" },
        0,
        "Version Description: Shows the Servers Version Information.",
        "Version Usage: /version *Message (Optional comment to the Server Host.)",
        GMRanks::User,
        true,
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
    return cmdTable[ static_cast<int>( index ) ].commandIsActive;
}

bool CmdTable::isSubCommand(const GMCmds& index, const QString& cmd)
{
    qint32 idx{ static_cast<int>( index ) };
    if ( cmdTable[ idx ].subCommandCount > 0 )
    {
        for ( int i = 0; i < cmdTable[ idx ].subCommandCount; ++i )
        {
            if ( Helper::cmpStrings( cmdTable[ idx ].subCommands[ i ], cmd ) )
            {
                return true;
            }
        }
    }
    return false;
}

bool CmdTable::getCmdHasSubCmd(const GMCmds& index)
{
    return cmdTable[ static_cast<qint32>( index ) ].subCommandCount >= 1;
}

QString CmdTable::getCmdName(const GMCmds& index)
{
    return cmdTable[ static_cast<qint32>( index ) ].command;
}

GMCmds CmdTable::getCmdIndex(const QString& cmd)
{
    GMCmds index{ GMCmds::Invalid };
    for ( int i = 0; i < GM_COMMAND_COUNT; ++i )
    {
        //Check the current Object if it contains our command information,
        if ( Helper::cmpStrings( cmdTable[ i ].command, cmd ) )
        {
            //Make Certain that the command is Activated.
            if ( cmdTable[ i ].commandIsActive == true )
                index = static_cast<GMCmds>( i );
        }
    }
    return index;
}

GMRanks CmdTable::getCmdRank(const GMCmds& index)
{
    if ( this->cmdIsActive( index ) )
    {
        return cmdTable[ static_cast<int>( index ) ].commandRank;
    }

    //The command is inactive. Return Rank Invalid.
    return GMRanks::Invalid;
}

QString CmdTable::collateCmdList(const GMRanks& rank)
{
    QString list{ "Available Command list: " };
    for ( int i = 0; i < GM_COMMAND_COUNT; ++i )
    {
        //Check the current Object if it contains our command information,
        if ( cmdTable[ i ].commandRank <= rank )
        {
            //Make Certain that the command is Activated.
            if ( cmdTable[ i ].commandIsActive == true )
                list.append( cmdTable[ i ].command % ", " );
        }
    }
    return list;
}

QString CmdTable::getCommandInfo(const GMCmds& index, const bool& syntax)
{
    qint32 idx{ static_cast<int>( index ) };
    if ( syntax )
    {
        return cmdTable[ idx ].commandSyntax;
    }
    return cmdTable[ idx ].commandInfo;
}
