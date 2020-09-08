
//Class includes.
#include "cmdhandler.hpp"

//ReMix includes.
#include "widgets/remixtabwidget.hpp"
#include "widgets/motdwidget.hpp"
#include "campexemption.hpp"
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

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &CmdHandler::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );
}

CmdHandler::~CmdHandler() = default;

bool CmdHandler::canUseAdminCommands(Player* admin, const GMRanks rank, const QString& cmdStr)
{
    bool retn{ false };
    QString invalidAuth{ "Error: You do not have access to the command [ %1 ]. Please refrain from attempting to use commands that you lack access to!" };

    QString unauth{ "While your SerNum is registered as a Remote Admin, you are not Authenticated and are unable to use these "
                    "commands. Please reply to this message with (/login *PASS) and the server will authenticate you." };

    QString invalid{ "Error: You do not have access to the command [ %1 ]. Please refrain from attempting to use Remote Admin "
                     "commands as you will automatically be banned after [ %2 ] attempts." };

    GMRanks plrRank{ this->getAdminRank( admin ) };
    if ( admin->getIsAdmin() )
    {
        retn = false;
        if ( admin->getAdminPwdReceived() )
        {
            if ( plrRank < rank )
            {
                invalidAuth = invalidAuth.arg( cmdStr );
                server->sendMasterMessage( invalidAuth, admin, false );
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
                server->sendMasterMessage( unauth, admin, false );
        }
    }
    else
    {
        if ( plrRank >= rank )
            return true;

        admin->setCmdAttempts( admin->getCmdAttempts() + 1 );
        invalid = invalid.arg( cmdStr )
                         .arg( MAX_CMD_ATTEMPTS - admin->getCmdAttempts() );

        if ( admin->getCmdAttempts() >= MAX_CMD_ATTEMPTS )
        {
            QString reason = "Auto-Banish; <Unregistered Remote Admin[ %1 ]: [ %2 ] command attempts>";

            reason = reason.arg (admin->getSernum_s() )
                           .arg( admin->getCmdAttempts() );
            server->sendMasterMessage( reason, admin, false );

            //Append IP:Port and BIO data to the reason for the Ban log.
            QString append{ " [ %1:%2 ], %3" };
                    append = append.arg( admin->peerAddress().toString() )
                                   .arg( admin->peerPort() )
                                   .arg( admin->getBioData() );
            reason.append( append );

            User::addBan( nullptr, admin, reason, false, PunishDurations::THIRTY_DAYS );
            emit this->insertLogSignal( server->getServerName(), reason, LogTypes::PUNISHMENT, true, true );

            admin->setDisconnected( true, DCTypes::IPDC );
        }
        else
            server->sendMasterMessage( invalid, admin, false );

        retn = false;
    }
    return retn;
}

void CmdHandler::parseMix5Command(Player* admin, const QString& packet)
{
    if ( admin == nullptr )
        return;

    //Do not accept comments from Users without a SerNum.
    if ( admin->getSernum_s().isEmpty()
      || packet.isEmpty() )
    {
        return;
    }

    qint32 colIndex{ Helper::getStrIndex( packet, ": " ) };
    QString alias{ packet.left( colIndex ).mid( 10 ) };

    QString msg{ packet.mid( colIndex + 2 ) };
            msg = msg.left( msg.length() - 2 );

    if ( !alias.isEmpty()
      && !msg.trimmed().isEmpty() )
    {
        if ( Helper::strStartsWithStr( msg, "/" ) )
        {
            if ( Helper::strStartsWithStr( msg, "/cmd " ) )
                msg = msg.mid( Helper::getStrIndex( msg, "/cmd " ) + 4 );
            else
                msg = msg.mid( Helper::getStrIndex( msg, "/" ) + 1 );

            if ( !msg.isEmpty() )
                this->parseCommandImpl( admin, msg );
        }
        else
        {
            if ( !msg.trimmed().isEmpty() )
            {
                //Echo the chat back to the User.
                if ( Settings::getSetting( SKeys::Setting, SSubKeys::EchoComments ).toBool() )
                {
                    if ( !Settings::getSetting( SKeys::Setting, SSubKeys::FwdComments ).toBool()
                      && !admin->getAdminPwdReceived() )
                    {
                        server->sendMasterMessage( "Echo: " % msg, admin, false );
                    }
                }

                if ( Settings::getSetting( SKeys::Setting, SSubKeys::FwdComments ).toBool() )
                {
                    Player* tmpPlr{ nullptr };
                    QString message{ "Server comment from %1 [ %2 ]: %3" };
                    QString user{ "User" };
                    if ( this->getAdminRank( admin ) >= GMRanks::GMaster )
                        user = "Admin";

                    message = message.arg( user )
                                     .arg( admin->getSernum_s() )
                                     .arg( msg );
                    for ( int i = 0; i < MAX_PLAYERS; ++i )
                    {
                        tmpPlr = server->getPlayer( i );
                        if ( tmpPlr != nullptr )
                        {
                            if ( this->getAdminRank( tmpPlr ) >= GMRanks::GMaster
                              && tmpPlr->getAdminPwdReceived() )
                            {
                                server->sendMasterMessage( message, tmpPlr, false );
                            }
                        }
                    }
                }
                QString sernum{ admin->getSernum_s() };
                emit newUserCommentSignal( sernum, alias, msg );
            }
        }
    }
}

void CmdHandler::parseMix6Command(Player* admin, const QString& packet)
{
    QString cmd{ packet };

    if ( admin != nullptr
      && !packet.isEmpty() )
    {
        qint32 colIndex{ Helper::getStrIndex( packet, ": /cmd " ) };
        if ( colIndex >= 0 )
            cmd = cmd.mid( colIndex + 7 );
        else
            cmd = cmd.mid( 10 );

        cmd = cmd.left( cmd.length() - 2 );
        if ( !cmd.isEmpty() )
            this->parseCommandImpl( admin, cmd );
    }
}

bool CmdHandler::parseCommandImpl(Player* admin, QString& packet)
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
    if ( !this->canUseAdminCommands( admin, cmdRank, cmd ) )
        return false;

    QString message{ "" };
    if ( cmdTable->isSubCommand( argIndex, subCmd ) )
    {
        if ( Helper::cmpStrings( subCmd, "all" ) )
        {
            //Correctly handle "all" command reason/message.
            message = packet.mid( Helper::getStrIndex( packet, arg1 ) );
            if ( this->getAdminRank( admin ) >= GMRanks::Admin
              || argIndex == GMCmds::Message )
            {
                all = true;
            }
            else    //Invalid Rank.
                return false;
        }
        else if ( Helper::cmpStrings( subCmd, "SOUL" ) )
        {
            if ( !( Helper::serNumtoInt( arg1, true ) & MIN_HEX_SERNUM ) )
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
    {   //Command parameter is not a Sub-Command.
        message = packet.mid( Helper::getStrIndex( packet, cmd ) + cmd.length() ).simplified();
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
                    server->sendMasterMessage( cmdTable->getCommandInfo( index, false ), admin, false );
                    server->sendMasterMessage( cmdTable->getCommandInfo( index, true ), admin, false );
                }
            }
        break;
        case GMCmds::List:
            {
                server->sendMasterMessage( cmdTable->collateCmdList( this->getAdminRank( admin ) ), admin, false );
            }
        break;
        case GMCmds::MotD:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd ) )
                    this->motdHandler( admin, subCmd, arg1, message );
            }
        break;
        case GMCmds::Info:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd ) )
                {
                    //Server UpTime, Connected Users, Connected Admins.
                    qint32 adminCount{ 0 };
                    QString tmpMsg{ "Server Info: Up Time[ %1 ], Connected Users[ %2 ], Connected Admins[ %3 ]." };

                    Player* tmpPlr{ nullptr };
                    for ( int i = 0; i < MAX_PLAYERS; ++i )
                    {
                        tmpPlr = server->getPlayer( i );
                        if ( tmpPlr != nullptr )
                        {
                            //Only Track Admins that are logged in and are visible.
                            if ( tmpPlr->getAdminPwdReceived()
                              && tmpPlr->getIsVisible() )
                            {
                                ++adminCount;
                            }
                        }
                    }

                    tmpMsg = tmpMsg.arg( Helper::getTimeFormat( server->getUpTime() ) )
                                   .arg( server->getPlayerCount() )
                                   .arg( adminCount );
                    server->sendMasterMessage( tmpMsg, admin, false );
                }
                retn = true;
            }
        break;
//        case GMCmds::NetStatus:
//            {
//            }
//        break;
        case GMCmds::Ban:
            {
                this->parseTimeArgs( message, duration, reason );
                if ( this->validateAdmin( admin, cmdRank, cmd )
                  && !subCmd.isEmpty() )
                {
                    this->banHandler( admin, arg1, duration, reason, all );
                }
                retn = true;
            }
        break;
        case GMCmds::UnBan:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd )
                  && ( !arg1.isEmpty() && !subCmd.isEmpty() ) )
                {
                    this->unBanHandler( subCmd, arg1 );
                }
                retn = true;
            }
        break;
        case GMCmds::Kick:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd )
                  && !subCmd.isEmpty() )
                {
                    this->kickHandler( admin, arg1, argIndex, message, all );
                }
                retn = true;
            }
        break;
        case GMCmds::Mute:
            {
                this->parseTimeArgs( message, duration, reason );
                if ( this->validateAdmin( admin, cmdRank, cmd )
                  && !subCmd.isEmpty() )
                {
                    this->muteHandler( admin, arg1, duration, reason, all );
                }
                retn = true;
            }
        break;
        case GMCmds::UnMute:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd )
                  && ( !arg1.isEmpty() && !subCmd.isEmpty() ) )
                {
                    this->unMuteHandler( admin, subCmd, arg1 );
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
                    tmpMsg = tmpMsg.arg( admin->getSernum_s() )
                                   .arg( all ? "Everyone" : arg1 );
                }

                if ( this->validateAdmin( admin, cmdRank, cmd )
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
                    if ((( admin->getAdminPwdRequested() || admin->getIsAdmin() )
                      || ( admin->getSvrPwdRequested() && !admin->getSvrPwdReceived() ) ) )
                    {
                        this->loginHandler( admin, subCmd );
                    }
                }
                retn = false;
                logMsg = false;
            }
        break;
        case GMCmds::Register:
            {
                if ( !subCmd.isEmpty()
                  && admin->getNewAdminPwdRequested() )
                {
                    this->registerHandler( admin, subCmd );
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

                if ( this->validateAdmin( admin, cmdRank, cmd ) )
                {
                    this->shutDownHandler( admin, duration, reason, stop, restart );
                    retn = true;
                }
            }
        break;
        case GMCmds::Vanish:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd ) )
                    this->vanishHandler( admin, subCmd );
            }
        break;
        case GMCmds::Version:
            {
                QString version{ "ReMix Version: [ %1 ]" };
                        version = version.arg( QString( REMIX_VERSION ) );

                if ( admin != nullptr )
                    server->sendMasterMessage( version, admin, false );

                //Version can be used by any User.
                //Do not log usage to file.
                logMsg = false;
            }
        break;
        case GMCmds::Camp:
            {
                logMsg = false; //Do not log User level commands to file.
                QString serNum{ "" };
                bool soulSubCmd{ false };
                if ( Helper::cmpStrings( arg1, "soul" )
                  && !arg2.isEmpty() )
                {
                    if ( !arg2.isEmpty() )
                    {
                        serNum = arg2;
                        soulSubCmd = true;
                        logMsg = true;
                    }
                }
                this->campHandler( admin, serNum, subCmd, index, soulSubCmd );
            }
        break;
        case GMCmds::Invalid:
        default:
        break;
    }

    QString msg{ "Remote-Admin: [ %1 ] issued the command [ %2 ] with ArgType [ %3 ], Arg1 [ %4 ], Arg2 [ %5 ] and Message [ %6 ]." };

    msg = msg.arg( admin->getSernum_s() )
             .arg( cmd )
             .arg( subCmd )
             .arg( arg1 )
             .arg( arg2 )
             .arg( message );

    //The command was a Message, do not send command information to online Users.
    if ( argIndex != GMCmds::Message )
    {
        if ( retn && canUseCommands )
            server->sendMasterMessage( msg, admin, false );
    }

    if ( logMsg )
        emit this->insertLogSignal( server->getServerName(), msg, LogTypes::ADMIN, true, true );

    return retn;
}

bool CmdHandler::canIssueAction(Player* admin, Player* target, const QString& arg1, const GMCmds& argIndex, const bool& all)
{
    if ( admin == nullptr || target == nullptr )
        return false;

    //Remote commands cannot affect the issuer.
    if ( admin == target )
    {
        this->cannotIssueAction( admin, arg1, argIndex, all );
        return false;
    }
    else if ( target->getIsAdmin() )
    {   //Remote commands cannot affect other remote administrators.
        //If the Admin has a higher rank then the command will succeed.
        if ( admin->getAdminRank() <= target->getAdminRank() )
        {
            //If the Admin's rank was less or equal the command will fail.
            this->cannotIssueAction( admin, arg1, argIndex, all );
            return false;
        }
    }

    //The target matches the Remote-Administrators conditions.
    QString sernum{ Helper::serNumToHexStr( arg1 ) };
    if ( this->isTarget( target, arg1, all ) )
        return true;

    //Fallthrough, command cannot be issued.
    return false;
}

void CmdHandler::cannotIssueAction(Player* admin, const QString& arg1, const GMCmds& argIndex, const bool& isAll)
{
    //Do not spam the command issuer with failures.
    if ( isAll )
        return;

    if ( admin == nullptr )
        return;

    QString message{ "Admin [ %1 ]: Unable to issue the command [ %2 ] on the User [ %3 ]. The User may be offline or another Remote Administrator." };
            message = message.arg( admin->getSernum_s() )
                             .arg( cmdTable->getCmdName( argIndex ) )
                             .arg( arg1 );

    server->sendMasterMessage( message, admin, false );
}

bool CmdHandler::isTarget(Player* target, const QString& arg1, const bool isAll)
{
    QString sernum{ Helper::serNumToHexStr( arg1 ) };
    if ( ( target->peerAddress().toString() == arg1 )
      || ( target->getSernumHex_s() == sernum )
      || isAll )
    {
        return true;
    }
    return false;
}

bool CmdHandler::validateAdmin(Player* admin, GMRanks& rank,
                               const QString& cmdStr)
{
    return ( ( this->getAdminRank( admin ) >= rank )
            && this->canUseAdminCommands( admin, rank, cmdStr ) );
}

GMRanks CmdHandler::getAdminRank(Player* admin)
{
    return static_cast<GMRanks>( admin->getAdminRank() );
}

void CmdHandler::motdHandler(Player* admin, const QString& subCmd, const QString& arg1, const QString& msg)
{
    if ( !subCmd.isEmpty()
      && cmdTable->isSubCommand( GMCmds::MotD, subCmd ) )
    {
        MOTDWidget* motdWidget{ MOTDWidget::getWidget( server ) };
        if ( motdWidget != nullptr )
        {
            QString message{ "Admin [ %1 ] has changed the Message of the Day to: [ %2 ]" };
            bool erase{ false };
            if ( Helper::cmpStrings( subCmd, "change" ) )
            {
                message = message.arg( admin->getSernum_s() )
                                 .arg( msg );
            }
            else if ( Helper::cmpStrings( subCmd, "remove" ) )
            {
                message.clear();
                if ( Helper::cmpStrings( arg1, "remove" ) )
                {
                    message = "Admin [ %1 ] has removed the Message of the Day!";
                    message = message.arg( admin->getSernum_s() );

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
        server->sendMasterMessage( cmdTable->getCommandInfo( GMCmds::MotD, true ), admin, false );
    }
}

void CmdHandler::banHandler(Player* admin, const QString& arg1, const QString& duration, const QString& reason, const bool& all)
{
    QString reasonMsg{ "Remote-Admin [ %1 ] has [ Banned ] you until [ %2 ]. Reason: [ %3 ]." };
    QString msg{ reason };

    Player* tmpPlr{ nullptr };
    bool ban{ false };

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr )
        {
            //Check target validity.
            if ( this->isTarget( tmpPlr, arg1, all ) )
            {
                ban = this->canIssueAction( admin, tmpPlr, arg1, GMCmds::Ban, all );
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
        qint32 banDuration{ static_cast<qint32>( PunishDurations::SEVEN_DAYS ) };

        if ( !duration.isEmpty() )
        {
            QString timeText{ "seconds" };
            banDuration = this->getTimePeriodFromString( duration, timeText );
            if ( banDuration == 0 )
                banDuration = static_cast<qint32>( PunishDurations::SEVEN_DAYS );
        }

        date += static_cast<quint64>( banDuration );
        dateString = Helper::getTimeAsString( date );

        if ( msg.isEmpty() )
            msg = "No Reason!";

        reasonMsg = reasonMsg.arg( admin->getSernum_s() )
                             .arg( dateString )
                             .arg( msg );

        if ( !reasonMsg.isEmpty() )
            server->sendMasterMessage( reasonMsg, tmpPlr, false );

        msg = msg.prepend( "Remote-Banish; " );
        User::addBan( admin, tmpPlr, msg, true,
                      static_cast<PunishDurations>( banDuration ) );

        msg = msg.append( ": [ %1 ], [ %2 ]" )
                 .arg( admin->getSernum_s() )
                 .arg( admin->getBioData() );

        emit this->insertLogSignal( server->getServerName(), msg, LogTypes::PUNISHMENT, true, true );

        tmpPlr->setDisconnected( true, DCTypes::IPDC );
    }
}

void CmdHandler::unBanHandler(const QString& subCmd, const QString& arg1)
{
    QString sernum{ Helper::serNumToHexStr( arg1 ) };
    if ( Helper::cmpStrings( subCmd, "ip" ) )
        User::removePunishment( arg1, PunishTypes::Ban, PunishTypes::IP );
    else
        User::removePunishment( sernum, PunishTypes::Ban, PunishTypes::SerNum );
}

void CmdHandler::kickHandler(Player* admin, const QString& arg1, const GMCmds& argIndex, const QString& message, const bool& all)
{
    QString reason{ "Remote-Admin [ %1 ] has [ Kicked ] you. Reason: [ %2 ]." };

    QString msg{ message };
    if ( msg.isEmpty() )
        msg = "No Reason!";

    reason = reason.arg( admin->getSernum_s() )
                   .arg( message );

    Player* tmpPlr{ nullptr };
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr )
        {
            //Check target validity.
            if ( this->isTarget( tmpPlr, arg1, all ) )
            {
                if ( this->canIssueAction( admin, tmpPlr, arg1, argIndex, all ) )
                {
                    server->sendMasterMessage( reason, tmpPlr, false );

                    reason = "Remote-Kick by admin [ %1 ]; %2: [ %3 ], [ %4 ]";
                    reason = reason.arg( admin->getSernum_s() )
                                   .arg( msg )
                                   .arg( tmpPlr->getSernum_s() )
                                   .arg( tmpPlr->getBioData() );

                    emit this->insertLogSignal( server->getServerName(), reason, LogTypes::PUNISHMENT, true, true );

                    tmpPlr->setDisconnected( true, DCTypes::IPDC );
                }
            }
        }
    }
}

void CmdHandler::muteHandler(Player* admin, const QString& arg1, const QString& duration, const QString& reason, const bool& all)
{
    QString reasonMsg{ "Remote-Admin [ %1 ] has [ Muted ] you until [ %2 ]. Reason: [ %3 ]." };
    QString msg{ reason };

    Player* tmpPlr{ nullptr };
    bool mute{ false };

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr )
        {
            //Check target validity.
            if ( this->isTarget( tmpPlr, arg1, all ) )
            {
                mute = this->canIssueAction( admin, tmpPlr, arg1, GMCmds::Mute, all );
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
        qint32 muteDuration{ static_cast<qint32>( PunishDurations::TEN_MINUTES ) };

        if ( !duration.isEmpty() )
        {
            QString timeText{ "seconds" };
            muteDuration = this->getTimePeriodFromString( duration, timeText );
            if ( muteDuration == 0 )
                muteDuration = static_cast<qint32>( PunishDurations::TEN_MINUTES );
        }

        date += static_cast<quint64>( muteDuration );
        dateString = Helper::getTimeAsString( date );

        if ( msg.isEmpty() )
            msg = "No Reason!";

        reasonMsg = reasonMsg.arg( admin->getSernum_s() )
                             .arg( dateString )
                             .arg( msg );

        if ( !reasonMsg.isEmpty() )
            server->sendMasterMessage( reasonMsg, tmpPlr, false );

        msg = msg.prepend( "Remote-Mute by admin [ %1 ]; " )
                 .arg( admin->getSernum_s() );
        User::addMute( admin, tmpPlr, msg, true, false, static_cast<PunishDurations>( muteDuration ) );

        msg = msg.append( ": [ %1 ], with BIO [ %2 ]" )
                 .arg( tmpPlr->getSernum_s() )
                 .arg( tmpPlr->getBioData() );

        emit this->insertLogSignal( server->getServerName(), msg, LogTypes::PUNISHMENT, true, true );
    }
}

void CmdHandler::unMuteHandler(Player* admin, const QString& subCmd, const QString& arg1)
{
    QString reasonMsg{ "Remote-Admin [ %1 ] has [ Un-Muted ] you." };
            reasonMsg = reasonMsg.arg( admin->getSernum_s() );

    QString sernum{ Helper::serNumToHexStr( arg1 ) };
    bool isSernum{ Helper::cmpStrings( subCmd, "soul" ) };

    Player* tmpPlr{ nullptr };
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr )
        {
            if ( this->isTarget( tmpPlr, arg1, false ) )
            {
                tmpPlr->setMuteDuration( 0 );
                server->sendMasterMessage( reasonMsg, tmpPlr, false );
            }
        }
        tmpPlr = nullptr;
    }

    //Default to the provided values. User might not be online.
    if ( !isSernum )
        User::removePunishment( arg1, PunishTypes::Mute, PunishTypes::IP );
    else
        User::removePunishment( sernum, PunishTypes::Mute, PunishTypes::SerNum );
}

void CmdHandler::msgHandler(const QString& arg1, const QString& message, const bool& all)
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
                    if ( tmpPlr->peerAddress().toString() == arg1
                      || tmpPlr->getSernum_s() == arg1 )
                    {
                        server->sendMasterMessage( message, tmpPlr, false );
                    }
                }
            }
        }
        else
            server->sendMasterMessage( message, nullptr, all );
    }
}

void CmdHandler::loginHandler(Player* admin, const QString& subCmd)
{
    QString response{ "%1 %2 Password." };
    QString invalid{ "Incorrect" };
    QString valid{ "Correct" };

    QStringList pwdTypes{ "Server", "Admin" };

    bool disconnect{ false };
    PwdTypes pwdType{ PwdTypes::Invalid };

    const QString& pwd{ subCmd };
    if ( admin->getSvrPwdRequested()
      && !admin->getSvrPwdReceived() )
    {
        pwdType = PwdTypes::Server;
        if ( Settings::cmpServerPassword( server->getServerName(), pwd ) )
        {
            response = response.arg( valid );

            admin->setSvrPwdRequested( false );
            admin->setSvrPwdReceived( true );
        }
        else
        {
            response = response.arg( invalid ).append( " Goodbye." );
            disconnect = true;
        }
        response = response.arg( pwdTypes.at( static_cast<int>( pwdType ) ) );
    }
    else if ( !admin->getAdminPwdReceived()
           || admin->getAdminPwdRequested()
           || admin->getIsAdmin() ) //Allow a Remote Admin to authenticate before a password is requested.
    {
        pwdType = PwdTypes::Admin;

        QString sernum{ admin->getSernumHex_s() };
        if ( !pwd.isEmpty()
          && User::cmpAdminPwd( sernum, pwd ) )
        {
            response = response.arg( valid ).append( " Welcome!" );

            admin->setAdminPwdRequested( false );
            admin->setAdminPwdReceived( true );

            //Inform Other Users of this Remote-Admin's login if enabled.
            if ( Settings::getSetting( SKeys::Setting, SSubKeys::InformAdminLogin ).toBool() )
            {
                QString message{ "Remote Admin [ %1 ] has Authenticated with the server." };
                        message = message.arg( admin->getSernum_s() ) ;

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
                            if ( tmpPlr != admin )
                                server->sendMasterMessage( message, tmpPlr, false );
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
        server->sendMasterMessage( response, admin, false );

    if ( disconnect )
    {
        if ( pwdType != PwdTypes::Invalid )
        {
            QString reason{ "Auto-Disconnect; Invalid %1 password: [ %2 ], [ %3 ]" };
                    reason = reason.arg( pwdTypes.at( static_cast<int>( pwdType ) ) )
                                   .arg( admin->getSernum_s() )
                                   .arg( admin->getBioData() );

            emit this->insertLogSignal( server->getServerName(), reason, LogTypes::PUNISHMENT, true, true );
        }
        admin->setDisconnected( true, DCTypes::IPDC );
    }
}

void CmdHandler::registerHandler(Player* admin, const QString& subCmd)
{
    QString success{ "You are now registered as an Admin with the Server. Congrats!" };

    QString fail{ "You were not registered as an Admin with the Server. It seems something has gone wrong or you were already registered as an Admin." };
    QString response{ "" };

    QString sernum{ admin->getSernumHex_s() };
    bool registered{ false };

    if ( !admin->getNewAdminPwdReceived() )
    {
        if (( admin->getNewAdminPwdRequested()
           || admin->getIsAdmin() )
          && !User::getHasPassword( sernum ) )
        {
            response = success;
            if ( User::makeAdmin( sernum, subCmd ) )
            {
                registered = true;

                admin->setNewAdminPwdRequested( false );
                admin->setNewAdminPwdReceived( true );

                admin->setAdminPwdRequested( false );
                admin->setAdminPwdReceived( true );
            }
            else
            {
                response = fail;

                admin->setNewAdminPwdRequested( false );
                admin->setNewAdminPwdReceived( false );
            }
        }
    }

    //Inform Other Users of this Remote-Admin's login if enabled.
    if ( registered
      && Settings::getSetting( SKeys::Setting, SSubKeys::InformAdminLogin ).toBool() )
    {
        QString message{ "User [ %1 ] has Registered as a Remote Administrator with the server." };
                message = message.arg( admin->getSernum_s() );

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
                    if ( tmpPlr != admin )
                        server->sendMasterMessage( message, admin, false );
                }
            }
        }
    }

    if ( !response.isEmpty() )
        server->sendMasterMessage( response, admin, false );
}

void CmdHandler::shutDownHandler(Player* admin, const QString& duration, const QString& reason, bool& stop, bool& restart)
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
        ServerInfo* plrServer{ admin->getServerInfo() };
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
            } );
        }

        message = message.arg( admin->getSernum_s() )
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
            message = message.arg( admin->getSernum_s() )
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

void CmdHandler::vanishHandler(Player* admin, const QString& subCmd)
{
    QString message{ "Admin [ %1 ]: You are now %2 to other Players..." };
    QString state{ "visible" };

    bool isVisible{ admin->getIsVisible() };
    if ( !isVisible )
        state = "visible";

    if ( subCmd.isEmpty() )
    {
        if ( isVisible )
            state = "invisible";

        admin->setIsVisible( !isVisible );
    }
    else
    {
        if ( cmdTable->isSubCommand( GMCmds::Vanish, subCmd ) )
        {
            if ( Helper::cmpStrings( subCmd, "hide" )
              && isVisible )
            {
                state = "invisible";
                admin->setIsVisible( false );
            }
            else if ( Helper::cmpStrings( subCmd, "show" )
                   && !isVisible )
            {
                state = "visible";
                admin->setIsVisible( true );
            }
            else if ( Helper::cmpStrings( subCmd, "status" ) )
            {
                message = "Admin [ %1 ]: You are currently %2 to other Players...";
            }
        }
    }

    message = message.arg( admin->getSernum_s() )
                     .arg( state );
    server->sendMasterMessage( message, admin, false );
}

void CmdHandler::campHandler(Player* admin, const QString& serNum, const QString& subCmd, const GMCmds& index, const bool& soulSubCmd)
{
    QString msg{ "" };
    QString allowCurrent{ "Players can not your camp if it's considered old." };
    QString allowExempt{ "The Player [ %1 ] has been [ %2 ] your Camp Exemptions list." };
    QString allowNew{ "Players can enter your camp even if it's considered old." };
    QString lockMsg{ "Your Camp has been %1!" };

    QString overrideConfirm{ "Player [ %1 ] has had their camp status changed!" };
    QString overrideLockAppend{ " by Admin [ %1 ]" };
    QString overrideAllowAppend{ "Admin [ %1 ]: " };

    QString unlock{ "Unlocked" };
    QString lock{ "Locked" };

    QString removed{ "Removed From" };
    QString added{ "Added To" };

    //Swap to the targeted Player.
    Player* tmpPlr{ nullptr };
    bool override{ false };

    qint32 idx{ -1 };
    if ( !subCmd.isEmpty() )
        idx = cmdTable->getSubCmdIndex( index, subCmd, false );

    if ( soulSubCmd
      && !serNum.isEmpty() )
    {
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            tmpPlr = server->getPlayer( i );
            if ( tmpPlr != nullptr )
            {
                //Check target validity.
                if ( this->isTarget( tmpPlr, serNum, false ) )
                {
                    if ( idx != 4 //Allow Soul is exempted from Admin checking.
                      && this->canUseAdminCommands( admin, GMRanks::Admin, "soul" ) )
                    {
                        override = this->canIssueAction( admin, tmpPlr, serNum, GMCmds::Camp, false );
                        if ( override )
                            break;
                    }
                    else
                        break;
                }
            }
        }
    }

    if ( idx != -1 )
    {
        if ( idx >= 0 )
            msg = lockMsg;

        if ( soulSubCmd && tmpPlr == nullptr )
        {
            return;
        }

        switch ( static_cast<GMSubCmds>( idx ) )
        {
            case GMSubCmds::Zero: //Lock
                {
                    msg = msg.arg( lock );
                    if ( override )
                        msg = msg.append( overrideLockAppend.arg( admin->getSernum_s() ) );

                    if ( tmpPlr != nullptr )
                        tmpPlr->setIsCampLocked( true );
                    else
                        admin->setIsCampLocked( true );
                }
            break;
            case GMSubCmds::One: //Unlock
                {
                    msg = msg.arg( unlock );
                    if ( override )
                        msg = msg.append( overrideLockAppend.arg( admin->getSernum_s() ) );

                    if ( tmpPlr != nullptr )
                        tmpPlr->setIsCampLocked( false );
                    else
                        admin->setIsCampLocked( false );
                }
            break;
            case GMSubCmds::Two: //OnlyCurrent.
                {
                    msg = allowCurrent;
                    if ( override )
                        msg = msg.prepend( overrideAllowAppend.arg( admin->getSernum_s() ) );

                    if ( tmpPlr != nullptr )
                        tmpPlr->setIsCampOptOut( true );
                    else
                        admin->setIsCampOptOut( true );
                }
            break;
            case GMSubCmds::Three: //AllowAll
                {
                    msg = allowNew;
                    if ( override )
                        msg = msg.prepend( overrideAllowAppend.arg( admin->getSernum_s() ) );

                    if ( tmpPlr != nullptr )
                        tmpPlr->setIsCampOptOut( false );
                    else
                        admin->setIsCampOptOut( false );
                }
            break;
            case GMSubCmds::Four: //Allow. Only online Players may be exempted for storage.
                {
                    if ( soulSubCmd
                      && tmpPlr != nullptr )
                    {
                        override = false;

                        msg = allowExempt;
                        msg = msg.arg( tmpPlr->getSernum_s() );
                        if ( !CampExemption::getInstance()->setPlayerExemption( admin->getSernumHex_s(), tmpPlr->getSernumHex_s() ) )
                        {
                            msg = msg.arg( removed );
                        }
                        else
                            msg = msg.arg( added );
                    }
                    else
                        return;
                }
            break;
            case GMSubCmds::Five:
            case GMSubCmds::Six:
            case GMSubCmds::Seven:
            case GMSubCmds::Invalid:
                {
                    //Unused commands.
                }
            break;
        }
    }
    else //Send Scene Status.
    {
        msg = "Your camp is currently [ %1 ]. New players are [ %2 ] to enter your camp if it's considered old to their client.";
        if ( admin->getIsCampLocked() )
            msg = msg.arg( lock );
        else
            msg = msg.arg( unlock );

        if ( admin->getIsCampOptOut() )
            msg = msg.arg( "Not Allowed" );
        else
            msg = msg.arg( "Allowed" );
    }

    if ( !msg.isEmpty() )
    {
        if ( override )
        {
            server->sendMasterMessage( msg, tmpPlr, false );
            server->sendMasterMessage( overrideConfirm.arg( tmpPlr->getSernum_s() ), admin, false );
        }
        else
            server->sendMasterMessage( msg, admin, false );
    }
}

void CmdHandler::parseTimeArgs(const QString& str, QString& timeArg, QString& reason)
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
            pStr.reserve( str.length() );

    QString::const_iterator itr{ str.constBegin() };

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
        duration = time * Helper::strToInt( pStr, static_cast<int>( IntBase::DEC ) );
    }
    return duration;
}
