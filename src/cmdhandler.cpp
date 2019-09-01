
//Class includes.
#include "cmdhandler.hpp"

//ReMix includes.
#include "remixtabwidget.hpp"
#include "motdwidget.hpp"
#include "serverinfo.hpp"
#include "cmdtable.hpp"
#include "settings.hpp"
#include "randdev.hpp"
#include "player.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "user.hpp"

//Qt includes.
#include <QtCore>

CmdHandler::CmdHandler(QObject* parent, ServerInfo* svr)
    : QObject(parent)
{
    cmdTable = CmdTable::getInstance();
    server = svr;
}

CmdHandler::~CmdHandler(){}

bool CmdHandler::canUseAdminCommands(Player* plr, const GMRanks rank,
                                     const QString& cmdStr)
{
    bool retn{ false };
    QString unauth{ "While your SerNum is registered as a Remote Admin, "
                    "you are not Authenticated and are unable to use these "
                    "commands. Please reply to this message with "
                    "(/login *PASS) and the server will "
                    "authenticate you." };

//    QString invalid{ "Your SerNum is not registered as a Remote Admin. "
//                     "Please refrain from attempting to use Remote Admin "
//                     "commands as you will be banned after [ %1 ] "
//                     "attempts." };

    QString invalid{ "Error: You do not have access to the command [ %1 ]. "
                     "Please refrain from attempting to use Remote Admin "
                     "commands as you will automatically be "
                     "banned after [ %2 ] attempts" };

    GMRanks plrRank{ this->getAdminRank( plr ) };
    if ( plr->getIsAdmin() )
    {
        retn = false;
        if ( plr->getAdminPwdReceived() )
        {
            retn = true;
        }
        else
        {
            if ( rank <= GMRanks::User )
                retn = true;
            else
                plr->sendMessage( unauth, false );
        }
    }
    else
    {
        if ( plrRank >= rank )
            return true;

        plr->setCmdAttempts( plr->getCmdAttempts() + 1 );
        invalid = invalid.arg( cmdStr )
                         .arg( MAX_CMD_ATTEMPTS - plr->getCmdAttempts() );

        if ( plr->getCmdAttempts() >= MAX_CMD_ATTEMPTS )
        {
            QString reason = "Auto-Banish; <Unregistered Remote Admin[ %1 ]: "
                             "[ %2 ] command attempts>";

            reason = reason.arg (plr->getSernum_s() )
                           .arg( plr->getCmdAttempts() );
            plr->sendMessage( reason, false );

            //Append IP:Port and BIO data to the reason for the Ban log.
            QString append{ " [ %1:%2 ], %3" };
                    append = append.arg( plr->getPublicIP() )
                                   .arg( plr->getPublicPort() )
                                   .arg( plr->getBioData() );
            reason.append( append );

            User::addBan( nullptr, plr, reason, false,
                          PunishDurations::THIRTY_DAYS );

            Logger::getInstance()->insertLog( server->getName(), reason,
                                              LogTypes::BAN, true, true );

            plr->setDisconnected( true, DCTypes::IPDC );
        }
        else
            plr->sendMessage( invalid, false );

        retn = false;
    }
    return retn;
}

void CmdHandler::parseMix5Command(Player* plr, const QString& packet)
{
    if ( packet.isEmpty()
      || plr == nullptr )
    {
        return;
    }

    qint32 colIndex{ Helper::getStrIndex( packet, ": " ) };
    QString alias = packet.left( colIndex ).mid( 10 );

    QString msg{ packet.mid( colIndex + 2 ) };
            msg = msg.left( msg.length() - 2 );

    if ( !alias.isEmpty()
      && !msg.isEmpty() )
    {
        if ( Helper::strStartsWithStr( msg, "/" ) )
        {
            if ( Helper::strStartsWithStr( msg, "/cmd " ) )
            {
                msg = msg.mid( Helper::getStrIndex( msg, "/cmd " ) + 4 );
            }
            else
                msg = msg.mid( Helper::getStrIndex( msg, "/" ) + 1 );

            if ( !msg.isEmpty() )
                this->parseCommandImpl( plr, msg );
        }
        else
        {
            if ( !msg.isEmpty() )
            {
                //Echo the chat back to the User.
                if ( Settings::getEchoComments() )
                    plr->sendMessage( "Echo: " % msg, false );

                if ( Settings::getFwdComments() )
                {
                    Player* tmpPlr{ nullptr };
                    QString message{ "Server comment from %1 [ %2 ]: %3" };
                    QString user{ "User" };
                    if ( this->getAdminRank( plr ) >= GMRanks::GMaster )
                        user = "Admin";

                    message = message.arg( user )
                                     .arg( plr->getSernum_s() )
                                     .arg( msg );
                    for ( int i = 0; i < MAX_PLAYERS; ++i )
                    {
                        tmpPlr = server->getPlayer( i );
                        if ( tmpPlr != nullptr )
                        {
                            if ( this->getAdminRank( tmpPlr ) >= GMRanks::GMaster
                              && tmpPlr->getAdminPwdReceived() )
                            {
                                tmpPlr->sendMessage( message, false );
                            }
                        }
                    }
                }
                QString sernum{ plr->getSernum_s() };
                emit newUserCommentSignal( sernum, alias, msg );
            }
        }
    }
}

void CmdHandler::parseMix6Command(Player* plr, const QString& packet)
{
    QString cmd{ packet };

    if ( plr != nullptr
      && !packet.isEmpty() )
    {
        qint32 colIndex{ Helper::getStrIndex( packet, ": /cmd " ) };
        if ( colIndex >= 0 )
            cmd = cmd.mid( colIndex + 7 );
        else
            cmd = cmd.mid( 10 );

        cmd = cmd.left( cmd.length() - 2 );
        if ( !cmd.isEmpty() )
            this->parseCommandImpl( plr, cmd );
    }
}

bool CmdHandler::parseCommandImpl(Player* plr, QString& packet)
{
    bool logMsg{ true };
    bool retn{ false };
    bool all{ false };

    QTextStream stream( &packet );
    QString cmd;
    QString subCmd;
    QString arg1;
    QString arg2;
    QString arg3;

    stream >> cmd >> subCmd >> arg1 >> arg2 >> arg3;

    GMCmds argIndex{ cmdTable->getCmdIndex( cmd ) };

    //The User entered a nonexistant command. Return gracefully.
    if ( argIndex == GMCmds::Invalid )
        return false;

    GMRanks cmdRank{ cmdTable->getCmdRank( argIndex ) };
    if ( !this->canUseAdminCommands( plr, cmdRank, cmd ) )
        return false;

    QString message{ "" };
    bool subCommand{ false };
    if ( cmdTable->isSubCommand( argIndex, subCmd ) )
    {
        subCommand = true;
        if ( Helper::cmpStrings( subCmd, "all" ) )
        {
            //Correctly handle "all" command reason/message.
            message = packet.mid( Helper::getStrIndex( packet, arg1 ) );
            if ( this->getAdminRank( plr ) >= GMRanks::Admin
              || argIndex == GMCmds::Message )
            {
                all = true;
            }
            else    //Invalid Rank. Give generic response.
                return false;
        }
        else if ( Helper::cmpStrings( subCmd, "SOUL" ) )
        {
            if ( !( arg1.toInt( nullptr, 16 ) & MIN_HEX_SERNUM ) )
                arg1.prepend( "SOUL " );
        }
        else if ( Helper::cmpStrings( subCmd, "change" ) )
        {
            message = packet.mid( Helper::getStrIndex( packet, arg1 ) );
        }

        //Command Parameter is a Sub-Command not 'All'.
        if ( message.isEmpty() )
            message = packet.mid( Helper::getStrIndex( packet, arg2 ) );
    }
    else
    {
        //Command parameter is not a Sub-Command.
        message = packet.mid( Helper::getStrIndex( packet, cmd )
                            + cmd.length() ).simplified();
    }

    bool canUseCommands{ false };
    bool sendCommandInfo{ false };
    bool getCommandSyntax{ false };
    GMCmds index{ argIndex };

    switch ( argIndex )
    {
        case GMCmds::Help:
            {
                if ( !subCmd.isEmpty() )
                {
                    index = cmdTable->getCmdIndex( subCmd );
                    if ( index != GMCmds::Invalid )
                    {
                        if ( !arg1.isEmpty() )
                        {
                            getCommandSyntax = cmdTable->isSubCommand( argIndex,
                                                                       arg1 );
                        }
                    }
                }
                sendCommandInfo = true;
            }
        break;
        case GMCmds::List:
            {
                plr->sendMessage(
                            cmdTable->collateCmdList(
                                this->getAdminRank( plr ) ), false );
            }
        break;
        case GMCmds::MotD:
            {
                if ( this->validateAdmin( plr, cmdRank, cmd ) )
                    this->motdHandler( plr, subCmd, arg1, message );
            }
        break;
//        case GMCmds::Info:
//            {
//            }
//        break;
//        case GMCmds::NetStatus:
//            {
//            }
//        break;
        case GMCmds::Ban:
            {
                if ( this->validateAdmin( plr, cmdRank, cmd )
                  && ( !arg1.isEmpty()
                    && !subCmd.isEmpty() ) )
                {
                    this->banhandler( plr, arg1, arg2, arg3,
                                      argIndex, message, all );
                }
                retn = true;
            }
        break;
        case GMCmds::UnBan:
            {
                if ( this->validateAdmin( plr, cmdRank, cmd )
                  && ( !arg1.isEmpty()
                    && !subCmd.isEmpty() ) )
                {
                    this->unBanhandler( subCmd, arg1 );
                }
                retn = true;
            }
        break;
        case GMCmds::Kick:
            {
                if ( this->validateAdmin( plr, cmdRank, cmd )
                  && !arg1.isEmpty() )
                {
                    this->kickHandler( plr, arg1, argIndex, message, all );
                }
                retn = true;
            }
        break;
        case GMCmds::Mute:
        case GMCmds::UnMute:
            {
                if ( this->validateAdmin( plr, cmdRank, cmd )
                  && !arg1.isEmpty() )
                {
                    this->muteHandler( plr, arg1, argIndex, message, all );
                }
                retn = true;
            }
        break;
        case GMCmds::Message:
            {
                QString tmpMsg{ "" };
                if ( !message.isEmpty() )
                {
                    tmpMsg = message;
                    tmpMsg.prepend( "Admin [ %1 ] to [ %2 ]: " );
                    tmpMsg = tmpMsg.arg( plr->getSernum_s() )
                                   .arg( all ? "Everyone" : arg1 );
                }

                if ( this->validateAdmin( plr, cmdRank, cmd )
                  && !arg1.isEmpty() )
                {
                    this->msgHandler( arg1, tmpMsg, all );
                }
                retn = true;
            }
        break;
        case GMCmds::Login:
            {
                if ( !subCmd.isEmpty() )
                {
                    if ( ( plr->getAdminPwdRequested()
                      || ( plr->getSvrPwdRequested()
                        && !plr->getSvrPwdReceived() ) ) )
                    {
                        this->loginHandler( plr, subCmd );
                    }
                }
                retn = false;
                logMsg = false;
            }
        break;
        case GMCmds::Register:
            {
                if ( !subCmd.isEmpty()
                  && plr->getNewAdminPwdRequested() )
                {
                    this->registerHandler( plr, subCmd );
                }
                retn = false;
                logMsg = false;
            }
        break;
        case GMCmds::ShutDown:
        case GMCmds::ReStart:
            {
                bool restart{ false };
                if ( argIndex == GMCmds::ReStart )
                    restart = true;

                if ( Helper::strContainsStr( subCmd, "stop" ) )
                {
                    message = packet.mid( Helper::getStrIndex( packet,
                                                               subCmd )
                                        + subCmd.length() ).simplified();
                }
                else if ( !subCommand )
                {
                    if ( message.isEmpty() )
                        message = "No reason provided.";
                }
                else
                {
                    message = packet.mid( Helper::getStrIndex( packet, subCmd )
                                        + subCmd.length() ).simplified();
                }

                if ( this->validateAdmin( plr, cmdRank, cmd ) )
                {
                    this->shutDownHandler( plr, index, subCmd, arg1,
                                           message, restart );
                    retn = true;
                }
            }
        break;
//        case GMCmds::MKAdmin:
//            {
//                this->mkAdminHandler( plr, arg1, arg2 );
//            }
//        break;
//        case GMCmds::RMAdmin:
//            {
//                this->rmAdminHandler( plr, arg1 );
//            }
//        break;
//        case GMCmds::CHAdmin:
//            {
//                this->chAdminHandler( plr, arg1, arg2 );
//            }
//        break;
//        case GMCmds::CHRules:
//            {
//                if ( this->validateAdmin( plr, GMRanks::CoAdmin ) )
//                    this->chRulesHandler( plr, arg1, QVariant() );
//            }
//        break;
//        case GMCmds::CHSettings:
//            {
//                this->chSettingsHandler( plr, arg1, arg2 );
//            }
//        break;
        case GMCmds::Vanish:
            {
                if ( this->validateAdmin( plr, cmdRank, cmd ) )
                    this->vanishHandler( plr, subCmd );
            }
        break;
        case GMCmds::Version:
            {
                QString ver{ "ReMix Version: [ %1 ]" };
                ver = ver.arg( QString( REMIX_VERSION ) );
                if ( plr != nullptr )
                    plr->sendMessage( ver, false );

                //Version can be used by any User.
                //Do not log usage to file.
                logMsg = false;
            }
        break;
        case GMCmds::Invalid:
        default:
        break;
    }

    if ( sendCommandInfo )
    {
        //Command lacked proper arguments. Send the command description.
        plr->sendMessage( cmdTable->getCommandInfo( index, getCommandSyntax ),
                          false );
    }

    QString msg{ "Remote-Admin: [ %1 ] issued the command [ %2 ] with "
                 "ArgType [ %3 ], Arg1 [ %4 ], Arg2 [ %5 ] and Message "
                 "[ %6 ]." };

    msg = msg.arg( plr->getSernum_s() )
             .arg( cmd )
             .arg( subCmd )
             .arg( arg1 )
             .arg( arg2 )
             .arg( message );

    //The command was a Message, do not send command information to
    //online Users.
    if ( argIndex != GMCmds::Message )
    {
        if ( retn && canUseCommands )
            plr->sendMessage( msg, false );
    }

    if ( logMsg )
    {
        Logger::getInstance()->insertLog( server->getName(), msg,
                                          LogTypes::ADMIN, true, true );
    }

    return retn;
}

bool CmdHandler::canIssueAction(Player* admin, Player* target,
                                const QString& arg1, const GMCmds& argIndex,
                                const bool& all)
{
    //Remote commands cannot affect the issuer.
    if ( admin == target )
    {
        this->cannotIssueAction( admin, arg1, argIndex );
        return false;
    }

    //Remote commands cannot affect other remote administrators.
    if ( target != nullptr )
    {
        //The target matches the Remote-Administrators conditions.
        QString sernum = Helper::serNumToHexStr( arg1 );
        if ( target->getPublicIP() == arg1
          || target->getSernumHex_s() == sernum
          || all )
        {
            return true;
        }

        if ( target->getIsAdmin() )
        {
            //If the Admin has a higher rank then the command will succeed.
            if ( admin->getAdminRank() <= target->getAdminRank() )
            {
                //If the Admin's rank was less or equal the command will fail.
                this->cannotIssueAction( admin, arg1, argIndex );
                return false;
            }
        }

    }

    //Fallthrough, command cannot be issued.
    return false;
}

void CmdHandler::cannotIssueAction(Player* admin, const QString& arg1,
                                   const GMCmds& argIndex)
{
    if ( admin == nullptr )
        return;

    QString message{ "Admin [ %1 ]: Unable to issue the command [ %2 ] on the "
                     "User [ %3 ]. The User may be offline or "
                     "another Remote Administrator." };
            message = message.arg( admin->getSernum_s() )
                             .arg( cmdTable->getCmdName( argIndex ) )
                             .arg( arg1 );

    admin->sendMessage( message, false );
}

bool CmdHandler::validateAdmin(Player* plr, GMRanks& rank,
                               const QString& cmdStr)
{
    return  ( ( this->getAdminRank( plr ) >= rank )
             && this->canUseAdminCommands( plr, rank, cmdStr ) );
}

GMRanks CmdHandler::getAdminRank(Player* plr)
{
    return static_cast<GMRanks>( plr->getAdminRank() );
}

void CmdHandler::motdHandler(Player* plr, const QString& subCmd,
                             const QString& arg1,
                             const QString& msg)
{
    if ( !subCmd.isEmpty()
      && cmdTable->isSubCommand( GMCmds::MotD, subCmd ) )
    {
        MOTDWidget* motdWidget = MOTDWidget::getWidget( server );
        if ( motdWidget != nullptr )
        {
            QString message{ "Admin [ %1 ] has changed the Message of the Day "
                             "to: [ %2 ]" };
            bool erase{ false };
            if ( Helper::cmpStrings( subCmd, "change" ) )
            {
                message = message.arg( plr->getSernum_s() )
                                 .arg( msg );
            }
            else if ( Helper::cmpStrings( subCmd, "remove" ) )
            {
                message.clear();
                if ( Helper::cmpStrings( arg1, "remove" ) )
                {
                    message = "Admin [ %1 ] has removed the "
                              "Message of the Day!";
                    message = message.arg( plr->getSernum_s() );

                    erase = true;
                }
            }

            if ( !erase )
                motdWidget->changeMotD( msg );
            else
                motdWidget->changeMotD( "" );

            server->sendMasterMessage( message, nullptr, true );
        }
    }
    else
    {
        //Invalid argument listing. Send the command syntax.
        plr->sendMessage(
                    cmdTable->getCommandInfo( GMCmds::MotD,
                                              true ) );
    }
}

void CmdHandler::banhandler(Player* plr, const QString& arg1,
                            const QString& arg2, const QString& arg3,
                            const GMCmds& argIndex, const QString& message,
                            const bool& all)
{
    QString reason{ "Remote-Admin [ %1 ] has [ Banned ] you until [ %2 ]. "
                    "Reason: [ %3 ]." };
    QString msg{ message };
    QString str{ "" };

    qint32 timeMul{ 0 };
    qint32 time{ 0 };

    bool ban{ false };
    bool ok{ false };

    Player* tmpPlr{ nullptr };

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr && tmpPlr != plr )
        {
            //Check target validity.
            if ( this->canIssueAction( plr, tmpPlr, arg1, argIndex, all ) )
            {
                ban = true;
            }
            else
                ban = false;

            if ( ban )
            {
                QString dateString{ "" };
                quint64 date{ QDateTime::currentDateTime().toTime_t() };
                qint32 banDuration{ static_cast<qint32>(
                                         PunishDurations::THIRTY_DAYS ) };

                if ( !arg2.isEmpty() && !arg3.isEmpty() )
                {
                    qint32 subCmdIdx{ cmdTable->getSubCmdIndex( argIndex,
                                                                arg2, true ) };
                    if ( subCmdIdx != static_cast<qint32>(
                                            GMSubCmds::Invalid ) )
                    {

                        time = this->getTimeFromCommand( arg3, subCmdIdx,
                                                         str, msg, timeMul, ok );

                        time *= timeMul;
                        banDuration = time;

                    }
                }

                date += banDuration;
                dateString = Helper::getTimeAsString( date );

                if ( msg.isEmpty() )
                    msg = "No Reason!";

                reason = reason.arg( plr->getSernum_s() )
                               .arg( dateString )
                               .arg( msg );

                if ( !reason.isEmpty() )
                    tmpPlr->sendMessage( reason, false );

                msg = msg.prepend( "Remote-Banish; " );
                User::addBan( plr, tmpPlr, msg, true,
                              static_cast<PunishDurations>( banDuration ) );

                msg = msg.append( ": [ %1 ], [ %2 ]" );
                msg = msg.arg( plr->getSernum_s() )
                         .arg( plr->getBioData() );

                Logger::getInstance()->insertLog( server->getName(), msg,
                                                  LogTypes::BAN, true, true );

                tmpPlr->setDisconnected( true, DCTypes::IPDC );
            }
        }
        ban = false;
    }
}

void CmdHandler::unBanhandler(const QString& subCmd, const QString& arg1)
{
    QString sernum = Helper::serNumToHexStr( arg1 );
    if ( Helper::cmpStrings( subCmd, "ip" ) )
        User::removeBan( arg1, static_cast<int>( BanTypes::IP ) );
    else
        User::removeBan( sernum, static_cast<int>( BanTypes::SerNum ) );
}

void CmdHandler::kickHandler(Player* plr, const QString& arg1,
                             const GMCmds& argIndex, const QString& message,
                             const bool& all)
{
    QString reason{ "Remote-Admin [ %1 ] has [ Kicked ] you. "
                    "Reason: [ %2 ]." };

    QString msg = message;
    if ( msg.isEmpty() )
        msg = "No Reason!";

    reason = reason.arg( plr->getSernum_s() )
                   .arg( message );

    Player* tmpPlr{ nullptr };
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr && tmpPlr != plr )
        {
            //Check target validity.
            if ( this->canIssueAction( plr, tmpPlr, arg1, argIndex, all ) )
            {
                tmpPlr->sendMessage( reason, false );

                reason = "Remote-Kick; %1: [ %2 ], [ %3 ]";
                reason = reason.arg( msg )
                               .arg( tmpPlr->getSernum_s() )
                               .arg( tmpPlr->getBioData() );

                Logger::getInstance()->insertLog( server->getName(), reason,
                                                  LogTypes::DC, true, true );

                tmpPlr->setDisconnected( true, DCTypes::IPDC );
            }
        }
    }
}

void CmdHandler::muteHandler(Player* plr, const QString& arg1,
                             const GMCmds& argIndex, const QString& message,
                             const bool& all)
{
    QString msg{ "Remote-Admin [ %1 ] %2 [ %3 ]'s Network. "
                 "Reason: [ %4 ]." };

    Player* tmpPlr{ nullptr };
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr && tmpPlr != plr )
        {
            //Check target validity.
            if ( this->canIssueAction( plr, tmpPlr, arg1, argIndex, all ) )
            {
                msg = msg.arg( plr->getSernum_s() )
                         .arg( argIndex == GMCmds::Mute ? "Muted" : "Un-Muted" )
                         .arg( arg1 )
                         .arg( message.isEmpty() ? "No Reason!" : message );

                if ( argIndex == GMCmds::Mute )
                    tmpPlr->setNetworkMuted( true, msg );
                else
                    tmpPlr->setNetworkMuted( false, msg );
            }
        }
    }
}

void CmdHandler::msgHandler(const QString& arg1, const QString& message,
                            const bool& all)
{
    if ( !message.isEmpty() )
    {
        if ( !all )
        {
            Player* tmpPlr{ nullptr };
            for ( int i = 0; i < MAX_PLAYERS; ++i )
            {
                tmpPlr = server->getPlayer( i );
                if ( tmpPlr != nullptr )
                {
                    if ( tmpPlr->getPublicIP() == arg1
                      || tmpPlr->getSernum_s() == arg1 )
                    {
                        tmpPlr->sendMessage( message, false );
                    }
                }
            }
        }
        else
            server->sendMasterMessage( message, nullptr, all );
    }
}

void CmdHandler::loginHandler(Player* plr, const QString& subCmd)
{
    QString response{ "%1 %2 Password." };
    QString invalid{ "Incorrect" };
    QString valid{ "Correct" };

    QString pwdTypes[ 2 ]{ "Server", "Admin" };

    bool disconnect{ false };
    PwdTypes pwdType{ PwdTypes::Invalid };

    QString pwd{ subCmd };
    if ( plr->getSvrPwdRequested()
      && !plr->getSvrPwdReceived() )
    {
        pwdType = PwdTypes::Server;
        if ( Settings::cmpServerPassword( pwd ) )
        {
            response = response.arg( valid );

            plr->setSvrPwdRequested( false );
            plr->setSvrPwdReceived( true );
        }
        else
        {
            response = response.arg( invalid ).append( " Goodbye." );
            disconnect = true;
        }
        response = response.arg( pwdTypes[ static_cast<int>( pwdType ) ] );
    }
    else if ( !plr->getAdminPwdReceived()
           || plr->getAdminPwdRequested() )
    {
        pwdType = PwdTypes::Admin;

        QString sernum{ plr->getSernumHex_s() };
        if ( !pwd.isEmpty()
          && User::cmpAdminPwd( sernum, pwd ) )
        {
            response = response.arg( valid ).append( " Welcome!" );

            plr->setAdminPwdRequested( false );
            plr->setAdminPwdReceived( true );

            //Inform Other Users of this Remote-Admin's login if enabled.
            if ( Settings::getInformAdminLogin() )
            {
                QString message{ "Remote Admin [ %1 ] has Authenticated with "
                                 "the server." };
                        message = message.arg( plr->getSernum_s() ) ;

                Player* tmpPlr{ nullptr };
                for ( int i = 0; i < MAX_PLAYERS; ++i )
                {
                    tmpPlr = server->getPlayer( i );
                    if ( tmpPlr != nullptr )
                    {
                        if ( this->getAdminRank( tmpPlr ) >= GMRanks::GMaster
                          && tmpPlr->getAdminPwdReceived() )
                        {
                            //Do not Inform our own Admin.. --Redundant..
                            if ( tmpPlr != plr )
                                tmpPlr->sendMessage( message, false );
                        }
                    }
                }
            }
        }
        else
        {
            response = response.arg( invalid ).append( " Goodbye." );
            disconnect = true;
        }
        response = response.arg( pwdTypes[ static_cast<int>( pwdType ) ] );
    }

    if ( !response.isEmpty() )
        plr->sendMessage( response, false );

    if ( disconnect )
    {
        if ( pwdType != PwdTypes::Invalid )
        {
            QString reason{ "Auto-Disconnect; Invalid %1 password: "
                            "[ %2 ], [ %3 ]" };
            reason = reason.arg( pwdTypes[ static_cast<int>( pwdType ) ],
                                 plr->getSernum_s(),
                                 plr->getBioData() );

            Logger::getInstance()->insertLog( server->getName(), reason,
                                              LogTypes::DC, true, true );
        }
        plr->setDisconnected( true, DCTypes::IPDC );
    }
}

void CmdHandler::registerHandler(Player* plr, const QString& subCmd)
{
    QString success{ "You are now registered as an Admin with the "
                     "Server. Congrats!" };

    QString fail{ "You were not registered as an Admin with the "
                  "Server. It seems something has gone wrong or "
                  "you were already registered as an Admin." };
    QString response{ "" };

    QString sernum{ plr->getSernumHex_s() };
    bool registered{ false };

    if ( !plr->getNewAdminPwdReceived() )
    {
        if (( plr->getNewAdminPwdRequested()
           || plr->getIsAdmin() )
          && !User::getHasPassword( sernum ) )
        {
            response = success;
            if ( User::makeAdmin( sernum, subCmd ) )
            {
                registered = true;

                plr->setNewAdminPwdRequested( false );
                plr->setNewAdminPwdReceived( true );

                plr->setAdminPwdRequested( false );
                plr->setAdminPwdReceived( true );
            }
            else
            {
                response = fail;

                plr->setNewAdminPwdRequested( false );
                plr->setNewAdminPwdReceived( false );
            }
        }
    }

    //Inform Other Users of this Remote-Admin's login if enabled.
    if ( registered
      && Settings::getInformAdminLogin() )
    {
        QString message{ "User [ %1 ] has Registered as a Remote "
                         "Administrator with the server." };
                message = message.arg( plr->getSernum_s() );

        Player* tmpPlr{ nullptr };
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            tmpPlr = server->getPlayer( i );
            if ( tmpPlr != nullptr )
            {
                if ( this->getAdminRank( tmpPlr ) >= GMRanks::GMaster
                  && tmpPlr->getAdminPwdReceived() )
                {
                    //Do not Inform our own Admin.. --Redundant..
                    if ( tmpPlr != plr )
                        tmpPlr->sendMessage( message, false );
                }
            }
        }
    }

    if ( !response.isEmpty() )
        plr->sendMessage( response, false );
}

void CmdHandler::shutDownHandler(Player* plr, const GMCmds& index,
                                 const QString& subCmd, const QString& arg1,
                                 const QString& msg, const bool& restart)
{
    QString message{ "Admin [ %1 ]: The Server will %2 in %3 %4..." };
    QString reason{ msg };

    //Default to seconds.
    bool stop{ false };
    qint32 timeMul{ 30 };
    qint32 time{ 1 };

    QString type{ "shut down" };
    if ( restart )
        type = "restart";

    QString timeText{ "seconds" };
    qint32 subCmdIdx{ static_cast<qint32>( GMSubCmds::Invalid ) };
    if ( cmdTable->isSubCommand( index, subCmd, true ) )
    {
        subCmdIdx = cmdTable->getSubCmdIndex( index, subCmd, true );
        timeMul = this->getTimeFromCommand( arg1, subCmdIdx, timeText,
                                            reason, time, stop );
    }
    else
    {
        if ( cmdTable->isSubCommand( index, subCmd, false ) )
        {
            subCmdIdx = cmdTable->getSubCmdIndex( index, subCmd, false );
            if ( subCmdIdx == static_cast<qint32>( GMSubCmds::One ) )
                stop = true;
        }
        else
        {
            //The subcommand for timing information is invalid.
            //Send the Command Syntax to the Admin.
            if ( !subCmd.isEmpty() && !arg1.isEmpty() )
            {
                if ( !Helper::strContainsStr( reason, subCmd ) )
                {
                    plr->sendMessage( cmdTable->getCommandInfo( index, true ),
                                      false );
                    return;
                }
            }
        }
    }

    if ( !stop )
    {
        ServerInfo* plrServer = plr->getServerInfo();
        if ( shutdownTimer == nullptr )
            shutdownTimer = new QTimer();

        if ( plrServer != nullptr )
        {
            QObject::connect( shutdownTimer, &QTimer::timeout, shutdownTimer,
                              [=]()
            {
                ReMixTabWidget::remoteCloseServer( plrServer, restart );

                shutdownTimer->disconnect();
                shutdownTimer->deleteLater();

                shutdownTimer = nullptr;
            });
        }

        message = message.arg( plr->getSernum_s() )
                         .arg( type )
                         .arg( timeMul )
                         .arg( timeText );

        shutdownTimer->start( time * timeMul * 1000 );
    }
    else
    {
        shutdownTimer->stop();
        shutdownTimer->disconnect();

        message = "Admin [ %1 ]: Has canceled the Server %2...";
        message = message.arg( plr->getSernum_s() )
                         .arg( type );
    }

    if ( !message.isEmpty() )
    {
        if ( !reason.isEmpty() )
        {
            message.append( " Reason: < " % reason % " >" );
        }
        server->sendMasterMessage( message, nullptr, true );
    }
}

void CmdHandler::vanishHandler(Player* plr, const QString& subCmd)
{
    QString message{ "Admin [ %1 ]: You are now %2 to other Players..." };
    QString state{ "visible" };

    bool isVisible{ plr->getIsVisible() };
    if ( !isVisible )
        state = "invisible";

    if ( subCmd.isEmpty() )
    {
        if ( isVisible )
            state = "invisible";

        plr->setIsVisible( !isVisible );
    }
    else
    {
        if ( cmdTable->isSubCommand( GMCmds::Vanish, subCmd ) )
        {
            if ( Helper::cmpStrings( subCmd, "hide" )
              && isVisible == true )
            {
                state = "invisible";
                plr->setIsVisible( false );
            }
            else if ( Helper::cmpStrings( subCmd, "show" )
                   && isVisible == false )
            {
                state = "visible";
                plr->setIsVisible( true );
            }
            else if ( Helper::cmpStrings( subCmd, "status" ) )
            {
                message = "Admin [ %1 ]: You are currently %2 to "
                          "other Players...";
            }
        }
    }

    message = message.arg( plr->getSernum_s() )
                     .arg( state );
    plr->sendMessage( message, false );
}

//void CmdHandler::mkAdminHandler(Player* plr, QString& sernum, QString& arg)
//{
//    QString msg{ "The Remote Admin [ %1 ] is attempting to register you as an "
//                 "Admin with the server. Please reply to this message with "
//                 "(/register *YOURPASS). Note: Neither the Server Host or "
//                 "other Admins will have access to this information." };

//    qint32 rank{ arg.toInt( 0, 10 ) };

//    Player* tmpPlr{ nullptr };
//    if ( plr != nullptr )
//    {
//        if ( !sernum.isEmpty() )
//        {
//            tmpPlr = server->getPlayer( server->getSernumSlot( sernum ) );
//            if ( tmpPlr != nullptr )
//            {
//                msg = msg.arg( plr->getSernum_s() );

//                if ( !msg.isEmpty() )
//                    server->sendMasterMessage( msg, tmpPlr, false );
//            }
//        }
//    }
//}

//void CmdHandler::rmAdminHandler(Player* plr, QString& sernum)
//{
//    QString msg{ "Your Remote Administrator privileges have been REVOKED "
//                 "by the Remote Admin [ %1 ]. Please contact the Server Host "
//                 "if you believe this was in error." };

//    Player* tmpPlr{ nullptr };
//    if ( plr != nullptr )
//    {
//        if ( !sernum.isEmpty() )
//        {
//            if ( Admin::remoteRemoveAdmin( sernum ) )
//            {
//                tmpPlr = server->getPlayer( server->getSernumSlot( sernum ) );
//                if ( tmpPlr != nullptr )
//                {
//                    msg = msg.arg( plr->getSernum_s() );
//                    if ( !msg.isEmpty() )
//                        server->sendMasterMessage( msg, tmpPlr, false );
//                }
//            }
//        }
//    }
//}

//void CmdHandler::chAdminHandler(Player* plr, QString& sernum, QString& arg)
//{
//    QString msg{ "The Remote Admin [ %1 ] has changed your rank from [ %1 ] "
//                 "to [ %2 ]. If you feel this was in error, please contact the "
//                 "Server Host." };

//    qint32 rank{ arg.toInt( 0, 10 ) };

//    Player* tmpPlr{ nullptr };
//    if ( plr != nullptr )
//    {
//        if ( !sernum.isEmpty() )
//        {
//            if ( Admin::remoteChangeRank( sernum, rank ) )
//            {
//                tmpPlr = server->getPlayer( server->getSernumSlot( sernum ) );
//                if ( tmpPlr != nullptr )
//                {
//                    qint32 prevRank{ tmpPlr->getAdminRank() };
//                    if ( Admin::remoteChangeRank( sernum, rank ) )
//                    {
//                        msg = msg.arg( plr->getSernum_s() )
//                              .arg( prevRank )
//                              .arg( rank );

//                        if ( !msg.isEmpty() )
//                            server->sendMasterMessage( msg, tmpPlr, false );
//                    }
//                }
//            }
//        }
//    }
//}

//void CmdHandler::chRulesHandler(Player* plr, const QString& rule,
//                                const QVariant& value)
//{
//    //The rule is empty, return.
//    if ( rule.isEmpty() )
//        return;

//    RulesWidget* widget = RulesWidget::getWidget( plr->getServerInfo() );
//    if ( widget != nullptr )
//    {
//        for ( auto toggle : ruleToggles )
//        {
//            if ( Helper::cmpStrings( rule, toggle ) )
//            {
//                RulesWidget::toggleRules( ruleToggles.value( toggle ), );
//                break;
//            }
//        }
//    }
//}

//void CmdHandler::getCommentsHandler(Player* plr, QString& arg)
//{

//}

//void CmdHandler::chSettingsHandler(Player* plr, QString& setting,
//                                   QString& value)
//{

//}

qint32 CmdHandler::getTimeFromCommand(const QString& timeArg,
                                      const qint32& subCmdIdx,
                                      QString& timeText, QString& reason,
                                      qint32& time, bool& stop)
{
    bool permanent{ false };
    qint32 timeMul{ 30 };
    GMSubCmds index{ static_cast<GMSubCmds>( subCmdIdx ) };

    switch ( index )
    {
        case GMSubCmds::One:
            {
                timeText = "seconds";
                time = 1;
            }
        break;
        case GMSubCmds::Two:
            {
                timeText = "minutes";
                time = 60;
            }
        break;
        case GMSubCmds::Three:
            {
                timeText = "hours";
                time = 60*60;
            }
        break;
        case GMSubCmds::Four:
            {
                timeText = "days";
                time = 60*60*24;
            }
        break;
        case GMSubCmds::Five:
            {
                timeText = "months";
                time = 60*60*24*30;
            }
        break;
        case GMSubCmds::Six:
            {
                timeText = "years";
                time = 60*60*24*30*12;
            }
        break;
        case GMSubCmds::Seven:
            {
                reason = reason.mid( Helper::getStrIndex( reason, timeArg ) )
                               .simplified();

                permanent = true;
                timeMul = 1;
                time = std::numeric_limits<qint32>::max();
                timeText = "permanent";
            }
        break;
        case GMSubCmds::Invalid:
            time = 0;
            timeMul = 0;
        break;
    }

    if ( !permanent )
    {
        if (  !stop && !timeArg.isEmpty() )
        {
            //Ensure that arg1 is a number,
            QString digit{ "" };
            for ( QChar chr : timeArg )
            {
                if ( chr.isDigit() )
                    digit += chr;
            }

            if ( !digit.isEmpty() )
            {
                reason = reason.mid( Helper::getStrIndex( reason, timeArg )
                                   + timeArg.length() ).simplified();
                timeMul = Helper::strToInt( digit, 10 );
            }
            else
                timeMul = 1;
        }
    }
    return timeMul;
}
