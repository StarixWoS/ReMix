
//Class includes.
#include "cmdhandler.hpp"

//ReMix includes.
#include "widgets/remixtabwidget.hpp"
#include "widgets/motdwidget.hpp"
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

CmdHandler::~CmdHandler() = default;

bool CmdHandler::canUseAdminCommands(Player* plr, const GMRanks rank,
                                     const QString& cmdStr)
{
    bool retn{ false };
    QString invalidAuth{ "Error: You do not have access to the command [ %1 ]. "
                         "Please refrain from attempting to use "
                         "commands that you lack access to!" };

    QString unauth{ "While your SerNum is registered as a Remote Admin, "
                    "you are not Authenticated and are unable to use these "
                    "commands. Please reply to this message with "
                    "(/login *PASS) and the server will "
                    "authenticate you." };

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
            if ( plrRank < rank )
            {
                invalidAuth = invalidAuth.arg( cmdStr );
                plr->sendMessage( invalidAuth, false );
                retn = false;
            }
            else
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
    if ( plr == nullptr )
        return;

    //Do not accept comments from Users without a SerNum.
    if ( plr->getSernum_s().isEmpty()
      || packet.isEmpty() )
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

    QString duration;
    QString reason;

    stream >> cmd >> subCmd >> arg1 >> arg2 >> arg3;

    GMCmds argIndex{ cmdTable->getCmdIndex( cmd ) };

    //The User entered a nonexistant command. Return gracefully.
    if ( argIndex == GMCmds::Invalid )
        return false;

    GMRanks cmdRank{ cmdTable->getCmdRank( argIndex ) };
    if ( !this->canUseAdminCommands( plr, cmdRank, cmd ) )
        return false;

    QString message{ "" };
    if ( cmdTable->isSubCommand( argIndex, subCmd ) )
    {
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
    GMCmds index{ argIndex };

    switch ( argIndex )
    {
        case GMCmds::Help:
            {
                if ( !subCmd.isEmpty() )
                    index = cmdTable->getCmdIndex( subCmd );

                //Send command description and usage.
                if ( index != GMCmds::Invalid )
                {
                    plr->sendMessage( cmdTable->getCommandInfo( index, false ),
                                      false );
                    plr->sendMessage( cmdTable->getCommandInfo( index, true ),
                                      false );
                }
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
        case GMCmds::Info:
            {
                //Server UpTime, Connected Users, Connected Admins.
                qint32 adminCount{ 0 };
                QString tmpMsg{ "Server Info: Up Time[ %1 ], Connected Users[ %2 ],"
                                " Connected Admins[ %3 ]." };

                Player* tmpPlr{ nullptr };
                for ( int i = 0; i < MAX_PLAYERS; ++i )
                {
                    tmpPlr = server->getPlayer( i );
                    if ( tmpPlr != nullptr )
                    {
                        if ( tmpPlr->getIsAdmin()
                          && tmpPlr->getIsVisible() )
                        {
                            ++adminCount;
                        }
                    }
                }

                tmpMsg = tmpMsg.arg( Helper::getTimeFormat(
                                                server->getUpTime() ) )
                               .arg( server->getPlayerCount() )
                               .arg( adminCount );
                plr->sendMessage( tmpMsg, false );
            }
        break;
//        case GMCmds::NetStatus:
//            {
//            }
//        break;
        case GMCmds::Ban:
            {
                this->parseTimeArgs( message, duration, reason );
                if ( this->validateAdmin( plr, cmdRank, cmd )
                  && ( !arg1.isEmpty()
                    && !subCmd.isEmpty() ) )
                {
                    this->banHandler( plr, arg1, duration, reason, all );
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
                    this->unBanHandler( subCmd, arg1 );
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
            {
                this->parseTimeArgs( message, duration, reason );
                if ( this->validateAdmin( plr, cmdRank, cmd )
                  && ( !arg1.isEmpty()
                    && !subCmd.isEmpty() ) )
                {
                    this->muteHandler( plr, arg1, duration, reason, all );
                }
                retn = true;
            }
        break;
        case GMCmds::UnMute:
            {
                if ( this->validateAdmin( plr, cmdRank, cmd )
                  && ( !arg1.isEmpty()
                    && !subCmd.isEmpty() ) )
                {
                    this->unMuteHandler( subCmd, arg1 );
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
                bool stop{ false };
                if ( argIndex == GMCmds::ReStart )
                    restart = true;

                if ( !Helper::strContainsStr( subCmd, "stop" ) )
                    this->parseTimeArgs( message, duration, reason );
                else
                    stop = true;

                if ( this->validateAdmin( plr, cmdRank, cmd ) )
                {
                    this->shutDownHandler( plr, duration, reason,
                                           stop, restart );
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
    if ( admin == nullptr || target == nullptr )
        return false;

    //Remote commands cannot affect the issuer.
    if ( admin == target )
    {
        this->cannotIssueAction( admin, arg1, argIndex );
        return false;
    }
    else if ( target->getIsAdmin() )
    {   //Remote commands cannot affect other remote administrators.
        //If the Admin has a higher rank then the command will succeed.
        if ( admin->getAdminRank() <= target->getAdminRank() )
        {
            //If the Admin's rank was less or equal the command will fail.
            this->cannotIssueAction( admin, arg1, argIndex );
            return false;
        }
    }

    //The target matches the Remote-Administrators conditions.
    QString sernum = Helper::serNumToHexStr( arg1 );
    if ( this->isTarget( target, arg1, all ) )
        return true;

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

bool CmdHandler::isTarget(Player* target, const QString& arg1,
                          const bool isAll)
{
    QString sernum = Helper::serNumToHexStr( arg1 );
    if ( target->getPublicIP() == arg1
      || target->getSernumHex_s() == sernum
      || isAll )
    {
        return true;
    }
    return false;
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
        plr->sendMessage( cmdTable->getCommandInfo( GMCmds::MotD, true ) );
    }
}

void CmdHandler::banHandler(Player* plr, const QString& arg1,
                            const QString& duration, const QString& reason,
                            const bool& all)
{
    QString reasonMsg{ "Remote-Admin [ %1 ] has [ Banned ] you until [ %2 ]. "
                       "Reason: [ %3 ]." };
    QString msg{ reason };

    Player* tmpPlr{ nullptr };
    bool ban{ false };

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr && tmpPlr != plr )
        {
            //Check target validity.
            if ( this->isTarget( tmpPlr, arg1, all ) )
            {
                ban = this->canIssueAction( plr, tmpPlr, arg1,
                                            GMCmds::Ban, all );

                if ( ban )
                    break;
            }
        }
        ban = false;
    }

    if ( ban )
    {
        QString dateString{ "" };
        quint64 date{ QDateTime::currentDateTimeUtc().toTime_t() };
        qint32 banDuration{ static_cast<qint32>(
                                PunishDurations::THIRTY_DAYS ) };

        if ( !duration.isEmpty() )
        {
            QString timeText{ "seconds" };
            banDuration = this->getTimePeriodFromString( duration,
                                                         timeText );
            if ( banDuration == 0 )
            {
                banDuration = static_cast<qint32>(
                                  PunishDurations::THIRTY_DAYS );
            }
        }

        date += static_cast<quint64>( banDuration );
        dateString = Helper::getTimeAsString( date );

        if ( msg.isEmpty() )
            msg = "No Reason!";

        reasonMsg = reasonMsg.arg( plr->getSernum_s() )
                             .arg( dateString )
                             .arg( msg );

        if ( !reasonMsg.isEmpty() )
            tmpPlr->sendMessage( reasonMsg, false );

        msg = msg.prepend( "Remote-Banish; " );
        User::addBan( plr, tmpPlr, msg, true,
                      static_cast<PunishDurations>( banDuration ) );

        msg = msg.append( ": [ %1 ], [ %2 ]" )
                 .arg( plr->getSernum_s() )
                 .arg( plr->getBioData() );

        Logger::getInstance()->insertLog( server->getName(), msg,
                                          LogTypes::BAN, true, true );

        tmpPlr->setDisconnected( true, DCTypes::IPDC );
    }
}

void CmdHandler::unBanHandler(const QString& subCmd, const QString& arg1)
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
            if ( this->isTarget( tmpPlr, arg1, all ) )
            {
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
}

void CmdHandler::muteHandler(Player* plr, const QString& arg1,
                             const QString& duration, const QString& reason,
                             const bool& all)
{
    QString reasonMsg{ "Remote-Admin [ %1 ] has [ Muted ] you until [ %2 ]. "
                       "Reason: [ %3 ]." };
    QString msg{ reason };

    Player* tmpPlr{ nullptr };
    bool mute{ false };

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr && tmpPlr != plr )
        {
            //Check target validity.
            if ( this->isTarget( tmpPlr, arg1, all ) )
            {
                mute = this->canIssueAction( plr, tmpPlr, arg1,
                                             GMCmds::Mute, all );
                if ( mute )
                    break;
            }
        }
        mute = false;
    }

    if ( mute )
    {
        QString dateString{ "" };
        quint64 date{ QDateTime::currentDateTimeUtc().toTime_t() };
        qint32 muteDuration{ static_cast<qint32>(
                                 PunishDurations::TEN_MINUTES ) };

        if ( !duration.isEmpty() )
        {
            QString timeText{ "seconds" };
            muteDuration = this->getTimePeriodFromString( duration,
                                                          timeText );
            if ( muteDuration == 0 )
            {
                muteDuration = static_cast<qint32>(
                                   PunishDurations::TEN_MINUTES );
            }
        }

        date += static_cast<quint64>( muteDuration );
        dateString = Helper::getTimeAsString( date );

        if ( msg.isEmpty() )
            msg = "No Reason!";

        reasonMsg = reasonMsg.arg( plr->getSernum_s() )
                             .arg( dateString )
                             .arg( msg );

        if ( !reasonMsg.isEmpty() )
            tmpPlr->sendMessage( reasonMsg, false );

        msg = msg.prepend( "Remote-Mute; " );
        User::addMute( plr, tmpPlr, msg, true,
                       static_cast<PunishDurations>( muteDuration ) );

        msg = msg.append( ": [ %1 ], [ %2 ]" )
                 .arg( plr->getSernum_s() )
                 .arg( plr->getBioData() );

        Logger::getInstance()->insertLog( server->getName(), msg,
                                          LogTypes::MUTE, true, true );

        tmpPlr->setNetworkMuted( true );
    }
}

void CmdHandler::unMuteHandler(const QString& subCmd, const QString& arg1)
{
    QString sernum = Helper::serNumToHexStr( arg1 );
    if ( Helper::cmpStrings( subCmd, "ip" ) )
        User::removeMute( arg1, static_cast<int>( BanTypes::IP ) );
    else
        User::removeMute( sernum, static_cast<int>( BanTypes::SerNum ) );
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

    QStringList pwdTypes{ "Server", "Admin" };

    bool disconnect{ false };
    PwdTypes pwdType{ PwdTypes::Invalid };

    const QString& pwd{ subCmd };
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
        response = response.arg( pwdTypes.at( static_cast<int>( pwdType ) ) );
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
        response = response.arg( pwdTypes.at( static_cast<int>( pwdType ) ) );
    }

    if ( !response.isEmpty() )
        plr->sendMessage( response, false );

    if ( disconnect )
    {
        if ( pwdType != PwdTypes::Invalid )
        {
            QString reason{ "Auto-Disconnect; Invalid %1 password: "
                            "[ %2 ], [ %3 ]" };
            reason = reason.arg( pwdTypes.at( static_cast<int>( pwdType ) ),
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

void CmdHandler::shutDownHandler(Player* plr, const QString& duration,
                                 const QString& reason, bool& stop,
                                 bool& restart)
{
    bool sendMsg{ true };
    qint32 time{ 0 };

    QString message{ "Admin [ %1 ]: The Server will %2 in [ %3 ] < %4 >..." };
    QString timeText{ "seconds" };

    QString type{ "shut down" };
    if ( restart )
        type = "restart";

    if ( !duration.isEmpty() )
    {
        time = getTimePeriodFromString( duration, timeText );
        if ( time == 0 )
            stop = true;
    }
    else //Default the shutdown action to 30 seconds.
        time = static_cast<qint32>( PunishDurations::THIRTY_SECONDS );

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
                         .arg( duration )
                         .arg( timeText );

        shutdownTimer->start( time * 1000 );
    }
    else
    {
        if ( shutdownTimer->isActive() )
        {
            shutdownTimer->stop();
            shutdownTimer->disconnect();

            message = "Admin [ %1 ]: Has canceled the Server %2...";
            message = message.arg( plr->getSernum_s() )
                             .arg( type );
        }
        else
            sendMsg = false;
    }

    if ( !message.isEmpty() && sendMsg )
    {
        if ( !reason.isEmpty() )
        {
            message.append( " Reason: < %1 >" );
            message = message.arg( reason );
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
        state = "visible";

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
              && isVisible )
            {
                state = "invisible";
                plr->setIsVisible( false );
            }
            else if ( Helper::cmpStrings( subCmd, "show" )
                   && !isVisible )
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

void CmdHandler::parseTimeArgs( const QString& str, QString& timeArg,
                                QString& reason )
{
    //This is the Action duration.
    QString duration{ Helper::getStrStr( str, "", "", " " ) };
    if ( !duration.isEmpty() )
    {
        if ( duration.at( 0 ).isDigit() )
        {
            reason = Helper::getStrStr( str, duration, " ", "" );
        }
        else
        {   //This is the action Reason.
            //No Action Duration was given so it will default to Minutes.
            reason = str;
        }
    }
    timeArg = duration;
}

qint32 CmdHandler::getTimePeriodFromString(const QString& str, QString& timeTxt)
{
    qint32 duration{ 0 };
    qint32 time{ 0 };
    QString pStr;
            pStr.reserve( 10 );

    QString::const_iterator itr = str.constBegin();

    while ( itr != nullptr )
    {
        // always starts with a number.
        if( !itr->isDigit() )
            break;

        pStr.clear();
        while( itr->isDigit() && *itr != nullptr )
        {
            pStr += *itr;
            ++itr;
        }
        // try to find a letter
        if( *itr != nullptr )
        {
            // check the type
            switch( itr->toLower().toLatin1() )
            {
                case 'y': //Year
                    {
                        timeTxt = "years";
                        time = 365*24*60*60;
                    }
                break;
                case 'd': //Days
                    {
                        timeTxt = "days";
                        time = 24*60*60;
                    }
                break;
                case 'h': //Hours
                    {
                        timeTxt = "hours";
                        time = 60*60;
                    }
                break;
                case 'm': //Minutes
                    {
                        timeTxt = "minutes";
                        time = 60;
                    }
                break;
                case 's': //Seconds
                    {
                        timeTxt = "seconds";
                        time = 1;
                    }
                break;
                default: //Default, no duration.
                    time = 0;
                break;
            }
            ++itr;
        }
        else //Minutes if no letter is given.
        {
            timeTxt = "seconds";
            time = 1;
        }
        duration = time * Helper::strToInt( pStr, 10 );
    }
    return duration;
}
