
//Class includes.
#include "cmdhandler.hpp"

//ReMix includes.
#include "widgets/remixtabwidget.hpp"
#include "widgets/motdwidget.hpp"
#include "campexemption.hpp"
#include "chatview.hpp"
#include "cmdtable.hpp"
#include "settings.hpp"
#include "server.hpp"
#include "player.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "user.hpp"

//Qt includes.
#include <QtCore>

QHash<QSharedPointer<Server>, CmdHandler*> CmdHandler::cmdInstanceMap;
const QMap<TimePeriods, QString> CmdHandler::tPeriods =
{
    { TimePeriods::Years,   "years"   },
    { TimePeriods::Days,    "days"    },
    { TimePeriods::Hours,   "hours"   },
    { TimePeriods::Minutes, "minutes" },
    { TimePeriods::Seconds, "seconds" },
    { TimePeriods::Default, "seconds" },
};

const QMap<GMRanks, QString> CmdHandler::ranks =
{
    { GMRanks::User,    "User"        },
    { GMRanks::GMaster, "Game Master" },
    { GMRanks::CoAdmin, "Co-Admin"    },
    { GMRanks::Admin,   "Admin"       },
    { GMRanks::Owner,   "Owner"       },
    { GMRanks::Creator, "Creator"     },
};

CmdHandler::CmdHandler(QSharedPointer<Server> svr, QObject* parent)
    : QObject(parent),
      server( svr )
{
    cmdTable = CmdTable::getInstance();

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &CmdHandler::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );

    QObject::connect( this, &CmdHandler::insertAdminMessageSignal, ChatView::getInstance( server ), &ChatView::insertAdminMessageSlot );
}

CmdHandler::~CmdHandler()
{
    QString msg{ "CmdHandler( 0x%1 ) deconstructed." };
            msg = msg.arg( Helper::intToStr( reinterpret_cast<quintptr>( this ), IntBase::HEX, IntFills::QuadWord ) );

    emit this->insertLogSignal( server->getServerName(), msg, LKeys::MiscLog, true, true );

    this->disconnect();
    server = nullptr;
}

CmdHandler* CmdHandler::getInstance(QSharedPointer<Server> server)
{
    CmdHandler* instance{ cmdInstanceMap.value( server, nullptr ) };
    if ( instance == nullptr )
    {
        instance = new CmdHandler( server, nullptr );
        if ( instance != nullptr )
            cmdInstanceMap.insert( server, instance );
    }
    return instance;
}

void CmdHandler::deleteInstance(QSharedPointer<Server> server)
{
    CmdHandler* instance{ cmdInstanceMap.take( server ) };
    if ( instance != nullptr )
        instance->deleteLater();
}

bool CmdHandler::canUseAdminCommands(QSharedPointer<Player> admin, const GMRanks rank, const QString& cmdStr)
{
    bool retn{ false };
    QString invalidAuth{ "Error: You do not have access to the command [ %1 ]. Please refrain from attempting to use commands that you lack access to!" };

    static const QString unauth{ "While your SerNum is registered as a Remote Admin, you are not Authenticated and are unable to use these "
                                 "commands. Please reply to this message with (/login *PASS) and the server will authenticate you." };

    QString invalid{ "Error: You do not have access to the command [ %1 ]. Please refrain from attempting to use Remote Admin "
                     "commands as you will automatically be banned after [ %2 ] attempts." };

    GMRanks plrRank{ admin->getAdminRank() };
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
                         .arg( *Globals::MAX_CMD_ATTEMPTS - admin->getCmdAttempts() );

        if ( admin->getCmdAttempts() >= *Globals::MAX_CMD_ATTEMPTS )
        {
            QString reason = "Auto-Banish; <Unregistered Remote Admin[ %1 ]: [ %2 ] command attempts>";
                    reason = reason.arg (admin->getSernum_s() )
                                   .arg( admin->getCmdAttempts() );

            server->sendMasterMessage( reason, admin, false );

            //Append IP:Port and BIO data to the reason for the Ban log.
            QString append{ " [ %1:%2 ], %3" };
                    append = append.arg( admin->getIPAddress() )
                                   .arg( admin->peerPort() )
                                   .arg( admin->getBioData() );
            reason.append( append );

            User::addBan( admin, reason, PunishDurations::THIRTY_DAYS );

            admin->setDisconnected( true, DCTypes::IPDC );
        }
        else
            server->sendMasterMessage( invalid, admin, false );

        retn = false;
    }
    return retn;
}

void CmdHandler::parseMix5Command(QSharedPointer<Player> plr, const QString& packet)
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
    QString alias{ packet.left( colIndex ).mid( 10 ) };

    QString msg{ packet.mid( colIndex + 2 ) };
            msg = msg.left( msg.length() - 2 );

    if ( !alias.isEmpty()
      && !msg.trimmed().isEmpty() )
    {
        if ( !Helper::strStartsWithStr( msg, "/" )
          && !Helper::strStartsWithStr( msg, "`" ) )
        {
            //Echo the chat back to the User.
            if ( Settings::getSetting( SKeys::Setting, SSubKeys::FwdComments ).toBool() )
            {
                QString message{ "Server comment from %1 [ %2 ]: %3" };
                QString user{ "User" };

                if ( plr->getAdminRank() >= GMRanks::GMaster )
                    user = "Admin";

                message = message.arg( user )
                                 .arg( plr->getSernum_s() )
                                 .arg( msg );

                for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
                {
                    if ( tmpPlayer != nullptr
                      && tmpPlayer != plr )
                    {
                        if ( tmpPlayer->getAdminRank() >= GMRanks::GMaster
                          && tmpPlayer->getAdminPwdReceived() )
                        {
                            server->sendMasterMessage( message, tmpPlayer, false );
                        }
                    }
                }
            }

            if ( Settings::getSetting( SKeys::Setting, SSubKeys::EchoComments ).toBool() )
                server->sendMasterMessage( "Echo: " % msg, plr, false );

            QString sernum{ plr->getSernum_s() };
            emit newUserCommentSignal( plr, msg );
        }
        else
        {
            if ( this->canParseCommand( plr, msg ) )
                this->parseCommandImpl( plr, msg );
        }
    }
}

bool CmdHandler::canParseCommand(QSharedPointer<Player> admin, const QString& command)
{
    //Player Object must not be null.
    if ( admin == nullptr )
        return false;

    //Ignore Empty Commands.
    if ( command.isEmpty() )
        return false;

    //Ignore Commands where only the delimiter is used.
    if ( command.length() == 1 )
        return false;

    //Ignore Commands with multiple delimiters.
    if ( command.at( 1 ) == '`'
      || command.at( 1 ) == '/' )
    {
        return false;
    }

    if ( command.at( 1 ).isSpace() ) //Delimiter must not be followed by "space".
        return false;

    //Ignore Deprecated method.
    if ( Helper::strStartsWithStr( command, "/cmd" ) )
        return false;

    return true;
}

void CmdHandler::parseCommandImpl(QSharedPointer<Player> admin, QString& packet)
{
    packet = packet.mid( 1 );

    if ( admin == nullptr )
        return;

    bool logMsg{ true };
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

    GMSubCmdIndexes subCmdIdx{ GMSubCmdIndexes::Invalid };
    GMCmds argIndex{ cmdTable->getCmdIndex( cmd ) };

    //The User entered a nonexistant command. Return gracefully.
    if ( argIndex == GMCmds::Invalid )
        return;

    GMRanks subCmdRank{ GMRanks::Invalid };
    GMRanks cmdRank{ cmdTable->getCmdRank( argIndex ) };
    if ( cmdRank == GMRanks::Invalid )
        return;

    if ( !this->canUseAdminCommands( admin, cmdRank, cmd ) )
        return;

    QString message{ "" };
    if ( !subCmd.isEmpty()
      && cmdTable->isSubCommand( argIndex, subCmd ) )
    {
        subCmdIdx = cmdTable->getSubCmdIndex( argIndex, subCmd );
        if ( subCmdIdx != GMSubCmdIndexes::Invalid )
        {
            subCmdRank = cmdTable->getSubCmdRank( argIndex, subCmdIdx );
            if ( !this->canUseAdminCommands( admin, subCmdRank, subCmd ) )
            {
                if ( argIndex != GMCmds::Camp ) //Camp sub commands should be checked within the handler.
                    return;
            }
        }

        if ( Helper::cmpStrings( subCmd, "all" ) )
        {
            //Correctly handle "all" command reason/message.
            message = packet.mid( Helper::getStrIndex( packet, arg1 ) );
            if ( admin->getAdminRank() >= GMRanks::Admin
              || argIndex == GMCmds::Message )
            {
                all = true;
            }
            else    //Invalid Rank.
                return;
        }
        else if ( Helper::cmpStrings( subCmd, "SOUL" ) )
        {
            if ( !( Helper::serNumToInt( arg1, true ) & *Globals::MIN_HEX_SERNUM ) )
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

    GMCmds index{ argIndex };
    switch ( argIndex )
    {
        case GMCmds::Help:
            {
                //GMRanks subRank{ GMRanks::Invalid };

                index = GMCmds::Invalid;
                QString tSubCmd{ arg1 };

                //Prevent Users from seeing details on commands they lack access to.
                message = "The command [ %1 ] does not exist, or you lack the access required to view it's usage.";

                index = cmdTable->getCmdIndex( subCmd );
                if ( subCmd.isEmpty() )
                    index = cmdTable->getCmdIndex( cmd );

                cmdRank = cmdTable->getCmdRank( index );

                if ( !tSubCmd.isEmpty() )
                {
                    if ( admin->getAdminRank() >= cmdRank )
                        index = cmdTable->getCmdIndex( subCmd );

                    if ( !arg1.isEmpty()
                      && cmdTable->isSubCommand( index, arg1 ) )
                    {
                        subCmdIdx = cmdTable->getSubCmdIndex( index, tSubCmd, false );
                        //subRank = cmdTable->getSubCmdRank( index, subIdx );
                    }
                }

                if ( tSubCmd.isEmpty() )
                {
                    if ( subCmd.isEmpty() )
                        message = message.arg( cmd );
                    else
                        message = message.arg( subCmd );
                }
                else
                    message = message.arg( subCmd % " " % tSubCmd );

                //Send command description and usage.
                if ( index != GMCmds::Invalid )
                {
                    server->sendMasterMessage( cmdTable->getCommandInfo( index, subCmdIdx, false ), admin, false );
                    server->sendMasterMessage( cmdTable->getCommandInfo( index, subCmdIdx, true ), admin, false );
                }
                else //Inform the User that they lack access to the command.
                    server->sendMasterMessage( message, admin, false );
            }
        break;
        case GMCmds::List:
            {
                server->sendMasterMessage( cmdTable->collateCmdStrings( admin ), admin, false );
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
                    this->infoHandler( admin, index, subCmd, arg1 );
            }
        break;
        case GMCmds::Ban:
            {
                this->parseTimeArgs( message, duration, reason );
                if ( this->validateAdmin( admin, cmdRank, cmd )
                  && !subCmd.isEmpty() )
                {
                    this->banHandler( admin, arg1, duration, reason, all );
                }
            }
        break;
        case GMCmds::UnBan:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd )
                  && ( !arg1.isEmpty() && !subCmd.isEmpty() ) )
                {
                    this->unBanHandler( subCmd, arg1 );
                }
            }
        break;
        case GMCmds::Kick:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd )
                  && !subCmd.isEmpty() )
                {
                    this->kickHandler( admin, arg1, argIndex, message, all );
                }
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
            }
        break;
        case GMCmds::UnMute:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd )
                  && ( !arg1.isEmpty() && !subCmd.isEmpty() ) )
                {
                    this->unMuteHandler( admin, subCmd, arg1 );
                }
            }
        break;
        case GMCmds::Quarantine:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd )
                  && !subCmd.isEmpty() )
                {
                    this->quarantineHandler( admin, arg1, reason );
                }
            }
        break;
        case GMCmds::UnQuarantine:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd )
                  && !subCmd.isEmpty() )
                {
                    this->unQuarantineHandler( admin, arg1, reason );
                }
            }
        break;
        case GMCmds::Message:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd )
                  && !arg1.isEmpty() )
                {
                    this->msgHandler( admin, message, arg1, all );
                }
            }
        break;
        case GMCmds::LogIn:
            {
                if ( !subCmd.isEmpty() )
                {
                    if ((( admin->getAdminPwdRequested() || admin->getIsAdmin() )
                      || ( admin->getSvrPwdRequested() && !admin->getSvrPwdReceived() ) ) )
                    {
                        this->loginHandler( admin, subCmd );
                    }
                }
                logMsg = false;
            }
        break;
        case GMCmds::LogOut:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd ) )
                {
                    admin->resetAdminAuth();
                    if ( !admin->getIsVisible() )
                        admin->setIsVisible( true );

                    static const QString success{ "You are no longer authenticated with the server! You must log in to use Admin commands again." };
                    server->sendMasterMessage( success, admin, false );
                }
            }
        break;
        case GMCmds::Register:
            {
                if ( !subCmd.isEmpty()
                  && admin->getNewAdminPwdRequested() )
                {
                    this->registerHandler( admin, subCmd );
                }
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
                    this->shutDownHandler( admin, duration, reason, stop, restart );
            }
        break;
        case GMCmds::Vanish:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd ) )
                {
                    this->vanishHandler( admin, subCmd );
                    logMsg = true;
                }
            }
        break;
        case GMCmds::Version:
            {
                QString version{ "ReMix Version: [ %1 ]" };
                        version = version.arg( QString( REMIX_VERSION ) );

                server->sendMasterMessage( version, admin, false );

                //Version can be used by any User.
                //Do not log usage to file.
                logMsg = false;
            }
        break;
        case GMCmds::Ping:
            {
                if ( this->validateAdmin( admin, cmdRank, cmd ) )
                    this->pingHandler( admin, index, subCmd, arg1 );
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
                    serNum = arg2;
                    soulSubCmd = true;
                    logMsg = true;
                }
                this->campHandler( admin, serNum, subCmd, index, soulSubCmd );
            }
        break;
        case GMCmds::Guild:
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

    if ( logMsg )
        emit this->insertLogSignal( server->getServerName(), msg, LKeys::AdminLog, true, true );

    return;
}

bool CmdHandler::canIssueAction(QSharedPointer<Player> admin, QSharedPointer<Player> target, const QString& arg1, const GMCmds& argIndex, const bool& all,
                                const bool& overrideLimit)
{
    if ( admin == nullptr || target == nullptr )
        return false;

    //Remote commands cannot affect the issuer.
    if ( !overrideLimit
      && this->isTargetingSelf( admin, target ) )
    {
        this->cannotIssueAction( admin, target, argIndex, all );
        return false;
    }
    else if ( !overrideLimit
           && target->getIsAdmin() )
    {   //Remote commands cannot affect other remote administrators.
        //If the Admin has a higher rank then the command will succeed.
        if ( admin->getAdminRank() <= target->getAdminRank() )
        {
            //If the Admin's rank was less or equal the command will fail.
            this->cannotIssueAction( admin, target, argIndex, all );
            return false;
        }
    }

    //The target matches the Remote-Administrators conditions.
    if ( this->isTarget( target, arg1, all ) )
        return true;

    //Fallthrough, command cannot be issued.
    return false;
}

bool CmdHandler::isTargetingSelf(QSharedPointer<Player> admin, QSharedPointer<Player> target)
{
    return ( admin == target );
}

void CmdHandler::cannotIssueAction(QSharedPointer<Player> admin, QSharedPointer<Player> target, const GMCmds& argIndex, const bool& isAll)
{
    //Do not spam the command issuer with failures.
    if ( isAll )
        return;

    if ( admin == nullptr || target == nullptr )
        return;

    QString cmdNames{ cmdTable->getCmdNames( argIndex ) };
    QString message{ "Admin [ %1 ]: Unable to issue the command [ %2 ] on the User [ %3 ]. The User may be offline or another Remote Administrator." };
            message = message.arg( admin->getSernum_s() )
                             .arg( cmdNames )
                             .arg( target->getSernum_s() );

    server->sendMasterMessage( message, admin, false );
}

bool CmdHandler::isTarget(QSharedPointer<Player> target, const QString& arg1, const bool isAll)
{
    if ( target == nullptr )
        return false;

    QString sernum{ Helper::serNumToHexStr( arg1 ) };
    if ( ( target->getIPAddress() == arg1 )
      || ( target->getSernumHex_s() == sernum )
      || isAll )
    {
        return true;
    }
    return false;
}

bool CmdHandler::validateAdmin(QSharedPointer<Player> admin, GMRanks& rank, const QString& cmdStr)
{
    return ( ( admin->getAdminRank() >= rank )
            && this->canUseAdminCommands( admin, rank, cmdStr ) );
}

void CmdHandler::motdHandler(QSharedPointer<Player> admin, const QString& subCmd, const QString& arg1, const QString& msg)
{
    if ( !subCmd.isEmpty()
      && cmdTable->isSubCommand( GMCmds::MotD, subCmd ) )
    {
        MOTDWidget* motdWidget{ MOTDWidget::getInstance( server ) };
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
    else    //Invalid argument listing. Send the command syntax.
        server->sendMasterMessage( cmdTable->getCommandInfo( GMCmds::MotD, GMSubCmdIndexes::Invalid, true ), admin, false );
}

void CmdHandler::infoHandler(QSharedPointer<Player> admin, const GMCmds& cmdIdx, const QString& subCmd, const QString& arg1)
{
    GMSubCmdIndexes subIdx{ GMSubCmdIndexes::Invalid };
    if ( !subCmd.isEmpty() )
        subIdx = cmdTable->getSubCmdIndex( cmdIdx, subCmd, false );

    QString tmpMsg{ "Server Info: Up Time [ %1 ], MasterMix Ping [ %2 ms, Avg: %3 ms, Trend: %4 ms ], Users [ Current %5 / Peak %6 ], "
                    "Muted [ %7 ], Quarantined [ %8 ]" };

    if ( subIdx == GMSubCmdIndexes::Invalid )
        subIdx = GMSubCmdIndexes::Zero; //Default to Server Information if invalid subcommand is provided.

    switch( subIdx )
    {
        case GMSubCmdIndexes::Zero: //Server
            {
                //Server UpTime, Connected Users, Connected Admins.
                qint32 adminCount{ 0 };

                //QThreadPool::activeThreadCount();
                //QThreadPool::maxThreadCount();
                //Peak Player COunt.

                for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
                {
                    if ( tmpPlayer != nullptr )
                    {
                        //Only Track Admins that are logged in and are visible.
                        if ( tmpPlayer->getAdminPwdReceived()
                          && tmpPlayer->getIsVisible() )
                        {
                            ++adminCount;
                        }
                    }
                }

                tmpMsg = tmpMsg.arg( Helper::getTimeFormat( server->getUpTime() ) )
                               .arg( server->getMasterPing() )
                               .arg( server->getMasterPingAvg() )
                               .arg( server->getMasterPingTrend() )
                               .arg( server->getPlayerCount() )
                               .arg( server->getPeakPlayerCount() )
                               .arg( server->getMutedPlayerCount() )
                               .arg( server->getQuarantinedPlayerCount() );

                if ( admin->getIsAdmin() )
                {
                    QString adminList{ ", Admins [ %1 ]." };
                    tmpMsg = tmpMsg % adminList.arg( adminCount );
                }
                else
                    tmpMsg = tmpMsg % ".";

                server->sendMasterMessage( tmpMsg, admin, false );
            }
        break;
        case GMSubCmdIndexes::One: //Player Information - SoulID.
        case GMSubCmdIndexes::Two: //Player Information - IP Address.
            {
                QSharedPointer<Player> target{ this->findTarget( admin, arg1, cmdIdx, false, true ) };
                if ( target != nullptr )
                {
                    QString plrInfo{ "Name [ %1 ], "
                                     "SerNum [ %2 ], "
                                     "IP Address [ %3 ], "
                                     "Ping [ %4 ], "
                                     "Online Duration [ %5 ], "
                                     "Rank [ %7 ], "
                                     "Muted [ %8 ], "
                                     "Quarantined [ %9 ], "
                                     "Packets In [ %10 ], "
                                     "Packets Out [ %11 ]." };

                    plrInfo = plrInfo.arg( target->getPlrName() )
                                     .arg( target->getSernum_s() )
                                     .arg( target->getIPAddress() )
                                     .arg( target->getPlrPingString() )
                                     .arg( Helper::getTimeFormat( target->getConnTime() ) )
                                     .arg( ranks.value( target->getAdminRank(), "User" ) )
                                     .arg( target->getIsMuted() )
                                     .arg( target->getIsQuarantined() )
                                     .arg( target->getBaudString( PlrCols::BytesIn ) )
                                     .arg( target->getBaudString( PlrCols::BytesOut ) );

                    server->sendMasterMessage( plrInfo, admin, false );
                }
            }
        break;
        case GMSubCmdIndexes::Three: //Muted
            {
                QString mutedList{ "Muted Users: " };
                if ( server != nullptr )
                    mutedList.append( server->getMutedPlayerList() );

                server->sendMasterMessage( mutedList, admin, false );
            }
        break;
        case GMSubCmdIndexes::Four: //Quarantined
            {
                QString quarantinedList{ "Quarantined Users: " };
                if ( server != nullptr )
                    quarantinedList.append( server->getQuarantinedPlayerList() );

                server->sendMasterMessage( quarantinedList, admin, false );
            }
        break;
        case GMSubCmdIndexes::Invalid:
        default:
        break;
    }
}

void CmdHandler::banHandler(QSharedPointer<Player> admin, const QString& arg1, const QString& duration, const QString& reason, const bool& all)
{
    QString reasonMsg{ "Remote-Admin [ %1 ] has [ Banned ] you until [ %2 ]. Reason: [ %3 ]." };
    QString msg{ reason };
    QPair<qint64, TimePeriods> pair;

    QString dateString{ "" };
    quint64 date{ static_cast<quint64>( QDateTime::currentDateTimeUtc().toSecsSinceEpoch() ) };
    qint64 banDuration{ *PunishDurations::SEVEN_DAYS };

    for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
    {
        if ( tmpPlayer != nullptr )
        {
            //Check target validity.
            if ( this->isTarget( tmpPlayer, arg1, all ) )
            {
                if ( this->canIssueAction( admin, tmpPlayer, arg1, GMCmds::Ban, all ) )
                {
                    if ( !duration.isEmpty() )
                    {
                        pair = this->getTimePeriodFromString( duration );

                        banDuration = pair.first;
                        if ( banDuration == 0 )
                            banDuration = *PunishDurations::SEVEN_DAYS;
                    }

                    date += banDuration;
                    dateString = Helper::getTimeAsString( date );

                    if ( msg.isEmpty() )
                        msg = "No Reason!";

                    reasonMsg = reasonMsg.arg( admin->getSernum_s() )
                                         .arg( dateString )
                                         .arg( msg );

                    if ( !reasonMsg.isEmpty() )
                        server->sendMasterMessage( reasonMsg, tmpPlayer, false );

                    msg = msg.prepend( "Remote-Banish; " );
                    User::addBan( admin, tmpPlayer, msg, true, static_cast<PunishDurations>( banDuration ) );

                    tmpPlayer->setDisconnected( true, DCTypes::IPDC );
                }
            }
        }
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

void CmdHandler::kickHandler(QSharedPointer<Player> admin, const QString& arg1, const GMCmds& argIndex, const QString& message, const bool& all)
{
    QString reason{ "Remote-Admin [ %1 ] has [ Kicked ] you. Reason: [ %2 ]." };

    QString msg{ message };
    if ( msg.isEmpty() )
        msg = "No Reason!";

    reason = reason.arg( admin->getSernum_s() )
                   .arg( message );

    for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
    {
        if ( tmpPlayer != nullptr )
        {
            //Check target validity.
            if ( this->isTarget( tmpPlayer, arg1, all ) )
            {
                if ( this->canIssueAction( admin, tmpPlayer, arg1, argIndex, all ) )
                {
                    server->sendMasterMessage( reason, tmpPlayer, false );

                    reason = "Remote-Kick by admin [ %1 ]; %2: [ %3 ], [ %4 ]";
                    reason = reason.arg( admin->getSernum_s() )
                                   .arg( msg )
                                   .arg( tmpPlayer->getSernum_s() )
                                   .arg( tmpPlayer->getBioData() );

                    emit this->insertLogSignal( server->getServerName(), reason, LKeys::PunishmentLog, true, true );

                    tmpPlayer->setDisconnected( true, DCTypes::IPDC );
                }
            }
        }
    }
}

void CmdHandler::muteHandler(QSharedPointer<Player> admin, const QString& arg1, const QString& duration, const QString& reason, const bool& all)
{
    QString reasonMsg{ "Remote-Admin [ %1 ] has [ Muted ] you until [ %2 ]. Reason: [ %3 ]." };
    QString msg{ reason };
    QPair<qint64, TimePeriods> pair;

    QString dateString{ "" };
    quint64 date{ static_cast<quint64>( QDateTime::currentDateTimeUtc().toSecsSinceEpoch() ) };
    qint64 muteDuration{ *PunishDurations::TEN_MINUTES };

    for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
    {
        if ( tmpPlayer != nullptr )
        {
            //Check target validity.
            if ( this->isTarget( tmpPlayer, arg1, all ) )
            {
                if ( this->canIssueAction( admin, tmpPlayer, arg1, GMCmds::Mute, all ) )
                {
                    if ( !duration.isEmpty() )
                    {
                        pair = this->getTimePeriodFromString( duration );

                        muteDuration = pair.first;
                        if ( muteDuration == 0 )
                            muteDuration = *PunishDurations::TEN_MINUTES;
                    }

                    date += muteDuration;
                    dateString = Helper::getTimeAsString( date );

                    if ( msg.isEmpty() )
                        msg = "No Reason!";

                    reasonMsg = reasonMsg.arg( admin->getSernum_s() )
                                         .arg( dateString )
                                         .arg( msg );

                    if ( !reasonMsg.isEmpty() )
                        server->sendMasterMessage( reasonMsg, tmpPlayer, false );

                    msg = msg.prepend( "Remote-Mute by admin [ %1 ]; " )
                             .arg( admin->getSernum_s() );
                    User::addMute( admin, tmpPlayer, msg, true, false, static_cast<PunishDurations>( muteDuration ) );
                }
            }
        }
    }
}

void CmdHandler::unMuteHandler(QSharedPointer<Player> admin, const QString& subCmd, const QString& arg1)
{
    QString reasonMsg{ "Remote-Admin [ %1 ] has [ Un-Muted ] you." };
            reasonMsg = reasonMsg.arg( admin->getSernum_s() );

    QString sernum{ Helper::serNumToHexStr( arg1 ) };
    const bool isSernum{ Helper::cmpStrings( subCmd, "soul" ) };
    const bool isIP{ Helper::cmpStrings( subCmd, "ip" ) };

    for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
    {
        if ( tmpPlayer != nullptr )
        {
            if ( this->isTarget( tmpPlayer, arg1, false ) )
            {
                tmpPlayer->setMuteDuration( 0 );
                server->sendMasterMessage( reasonMsg, tmpPlayer, false );
            }
        }
    }

    //Default to the provided values. User might not be online.
    if ( !isSernum && isIP )
        User::removePunishment( arg1, PunishTypes::Mute, PunishTypes::IP );
    else if ( isSernum && !isIP )
        User::removePunishment( sernum, PunishTypes::Mute, PunishTypes::SerNum );
}

void CmdHandler::quarantineHandler(QSharedPointer<Player> admin, const QString& arg1, const QString& reason)
{
    QString reasonMsg{ "Remote-Admin [ %1 ] has [ Quarantined ] you. Reason: [ %2 ]." };
    QString append{ "You may only interact with other Quarantined Users." };
    QString msg{ reason };

    QSharedPointer<Player> tmpPlr{ this->findTarget( admin, arg1, GMCmds::Quarantine, false, false ) };
    if ( tmpPlr != nullptr )
    {
        if ( msg.isEmpty() )
            msg = "No Reason!";

        reasonMsg = reasonMsg.arg( admin->getSernum_s() )
                             .arg( msg );

        if ( !reasonMsg.isEmpty() )
        {
            server->sendMasterMessage( reasonMsg, tmpPlr, false );
            server->sendMasterMessage( append, tmpPlr, false );
        }

        reasonMsg = "Remote-Quarantine by admin [ %1 ]; %2: [ %3 ], [ %4 ]";
        reasonMsg = reasonMsg.arg( admin->getSernum_s() )
                             .arg( msg )
                             .arg( tmpPlr->getSernum_s() )
                             .arg( tmpPlr->getBioData() );

        emit this->insertLogSignal( server->getServerName(), reasonMsg, LKeys::PunishmentLog, true, true );

        if ( !tmpPlr->getIsQuarantined() )
        {
            tmpPlr->setQuarantined( true );
            tmpPlr->setQuarantineOverride( false );
        }
    }
}

void CmdHandler::unQuarantineHandler(QSharedPointer<Player> admin, const QString& arg1, const QString& reason)
{
    QString reasonMsg{ "Remote-Admin [ %1 ] has [ Un-Quarantined ] you. Reason: [ %2 ]." };
    QString append{ "You may now interact with other Users." };
    QString msg{ reason };

    QSharedPointer<Player> tmpPlr{ this->findTarget( admin, arg1, GMCmds::UnQuarantine, false, false ) };
    if ( tmpPlr != nullptr )
    {
        if ( msg.isEmpty() )
            msg = "No Reason!";

        reasonMsg = reasonMsg.arg( admin->getSernum_s() )
                             .arg( msg );

        if ( !reasonMsg.isEmpty() )
        {
            server->sendMasterMessage( reasonMsg, tmpPlr, false );
            server->sendMasterMessage( append, tmpPlr, false );
        }

        reasonMsg = "Remote-UnQuarantine by admin [ %1 ]; %2: [ %3 ], [ %4 ]";
        reasonMsg = reasonMsg.arg( admin->getSernum_s() )
                             .arg( msg )
                             .arg( tmpPlr->getSernum_s() )
                             .arg( tmpPlr->getBioData() );

        emit this->insertLogSignal( server->getServerName(), reasonMsg, LKeys::PunishmentLog, true, true );

        if ( tmpPlr->getIsQuarantined() )
            tmpPlr->setQuarantineOverride( true );
    }
}

void CmdHandler::msgHandler(QSharedPointer<Player> admin, const QString& message, const QString& target, const bool& all)
{
    QString tmpMsg{ "" };
    if ( !message.isEmpty() )
    {
        tmpMsg = message;
        tmpMsg.prepend( "Admin [ %1 ] to [ %2 ]: " );
        tmpMsg = tmpMsg.arg( admin->getSernum_s() )
                       .arg( all ? "Everyone" : target );
    }

    QSharedPointer<Player> tmpPlr{ nullptr };
    if ( !message.isEmpty() )
    {
        if ( !all )
        {
            for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
            {
                tmpPlr = tmpPlayer;
                if ( tmpPlr != nullptr )
                {
                    if ( this->isTarget( tmpPlr, target ) )
                    {
                        server->sendMasterMessage( tmpMsg, tmpPlr, false );
                        break;
                    }
                }
            }
        }
        else
            server->sendMasterMessage( tmpMsg, nullptr, all );

        emit this->insertAdminMessageSignal( message, all, admin, tmpPlr );
    }
}

void CmdHandler::loginHandler(QSharedPointer<Player> admin, const QString& subCmd)
{
    QString punishedUsers{ "There are currently [ %1 ] Users Muted and [ %2 ] Users Quarantined. Please use the \"Info\" command for more information." };
    QString response{ "%1 %2 Password." };

    static const QString invalidStr{ "Incorrect" };
    static const QString validStr{ "Correct" };
    static const QString welcomeStr{ " Welcome!" };
    static const QString goodbyeStr{ " Goodbye." };

    static const QStringList pwdTypes{ "[ Server ]", "[ Admin ]" };

    bool disconnect{ false };
    bool validPwd{ false };

    PwdTypes pwdType{ PwdTypes::Invalid };

    const QString& pwd{ subCmd };
    if ( admin->getSvrPwdRequested()
      && !admin->getSvrPwdReceived() )
    {
        pwdType = PwdTypes::Server;
        if ( Settings::cmpServerPassword( server->getServerName(), pwd ) )
        {
            response = response.arg( validStr );

            admin->setSvrPwdRequested( false );
            admin->setSvrPwdReceived( true );
        }
        else
        {
            response = response.arg( invalidStr ).append( goodbyeStr );
            disconnect = true;
        }
        response = response.arg( pwdTypes.at( *pwdType ) );
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
            validPwd = true;
            response = response.arg( validStr )
                               .append( welcomeStr );

            admin->setAdminPwdRequested( false );
            admin->setAdminPwdReceived( true );

            QString loginStr{ "Remote Admin [ %1 ] Authenticated with the server." };
                    loginStr = loginStr.arg( admin->getSernum_s() ) ;

            //Inform Other Admins of this login event if enabled.
            if ( Settings::getSetting( SKeys::Setting, SSubKeys::InformAdminLogin ).toBool() )
            {
                for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
                {
                    if ( tmpPlayer != nullptr )
                    {
                        if ( tmpPlayer->getAdminRank() >= GMRanks::GMaster
                          && tmpPlayer->getAdminPwdReceived() )
                        {
                            //Do not Inform our own Admin.. --Redundant..
                            if ( tmpPlayer != admin )
                                server->sendMasterMessage( loginStr, tmpPlayer, false );
                        }
                    }
                }
            }
            emit this->insertLogSignal( server->getServerName(), loginStr, LKeys::AdminLog, true, true );
        }
        else
        {
            response = response.arg( invalidStr )
                               .append( goodbyeStr );
            disconnect = true;
        }
        response = response.arg( pwdTypes.at( *pwdType ) );
    }

    if ( !response.isEmpty() )
    {
        server->sendMasterMessage( response, admin, false );
        if ( validPwd == true
          && pwdType == PwdTypes::Admin )
        {
            punishedUsers = punishedUsers.arg( server->getMutedPlayerCount() )
                                         .arg( server->getQuarantinedPlayerCount() );

            server->sendMasterMessage( punishedUsers, admin, false );
        }
    }

    if ( disconnect )
    {
        if ( pwdType != PwdTypes::Invalid )
        {
            QString reason{ "Auto-Disconnect; Invalid %1 password: [ %2 ], [ %3 ]" };
                    reason = reason.arg( pwdTypes.at( *pwdType ) )
                                   .arg( admin->getSernum_s() )
                                   .arg( admin->getBioData() );

            emit this->insertLogSignal( server->getServerName(), reason, LKeys::PunishmentLog, true, true );
        }
        admin->setDisconnected( true, DCTypes::IPDC );
    }
}

void CmdHandler::registerHandler(QSharedPointer<Player> admin, const QString& subCmd)
{
    static const QString successStr{ "You are now registered as an Admin with the Server." };
    static const QString failStr{ "You were not registered as an Admin with the Server. "
                                  "It seems something has gone wrong or you were already registered as an Admin." };

    QString response{ "" };

    QString sernum{ admin->getSernumHex_s() };
    bool registered{ false };

    if ( !admin->getNewAdminPwdReceived() )
    {
        if (( admin->getNewAdminPwdRequested()
           || admin->getIsAdmin() )
          && !User::getHasPassword( sernum ) )
        {
            response = successStr;
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
                response = failStr;

                admin->setNewAdminPwdRequested( false );
                admin->setNewAdminPwdReceived( false );
            }
        }
    }

    //Inform Other Users of this Remote-Admin's login if enabled.
    if ( registered
      && Settings::getSetting( SKeys::Setting, SSubKeys::InformAdminLogin ).toBool() )
    {
        QString registerStr{ "User [ %1 ] Registered as a Remote Administrator with the server." };
                registerStr = registerStr.arg( admin->getSernum_s() );

        for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
        {
            if ( tmpPlayer != nullptr )
            {
                if ( tmpPlayer->getAdminRank() >= GMRanks::GMaster
                  && tmpPlayer->getAdminPwdReceived() )
                {
                    //Do not Inform our own Admin.. --Redundant..
                    if ( tmpPlayer != admin )
                        server->sendMasterMessage( registerStr, admin, false );
                }
            }
        }
        emit this->insertLogSignal( server->getServerName(), registerStr, LKeys::AdminLog, true, true );
    }

    if ( !response.isEmpty() )
        server->sendMasterMessage( response, admin, false );
}

void CmdHandler::shutDownHandler(QSharedPointer<Player> admin, const QString& duration, const QString& reason, bool& stop, bool& restart)
{
    QPair<qint64, TimePeriods> pair;
    bool sendMsg{ true };
    auto time{ 0 };

    QString message{ "Admin [ %1 ]: The Server will %2 in [ %3 ] < %4 >..." };
    QString timeText{ "seconds" };

    static const QString shutDownStr{ "shut down" };
    static const QString restartStr{ "restart" };

    if ( !duration.isEmpty() )
    {
        pair = getTimePeriodFromString( duration );

        time = pair.first;
        if ( time == 0 )
            stop = true;
    }
    else //Default the shutdown action to 30 seconds.
        time = *PunishDurations::THIRTY_SECONDS;

    if ( !stop )
    {
        if ( shutdownTimer == nullptr )
            shutdownTimer = new QTimer();

        QObject::connect( shutdownTimer, &QTimer::timeout, shutdownTimer,
        [=, this]()
        {
            ReMixTabWidget::getInstance()->remoteCloseServer( admin->getServer(), restart );

            shutdownTimer->disconnect();
            shutdownTimer->deleteLater();

            shutdownTimer = nullptr;
        } );

        message = message.arg( admin->getSernum_s() )
                         .arg( ( restart == true ? restartStr : shutDownStr ) )
                         .arg( duration )
                         .arg( timeText );

        shutdownTimer->start( time * *TimeMultiply::Milliseconds );
    }
    else
    {
        if ( shutdownTimer->isActive() )
        {
            shutdownTimer->stop();
            shutdownTimer->disconnect();

            message = "Admin [ %1 ]: Has canceled the Server %2...";
            message = message.arg( admin->getSernum_s() )
                             .arg( ( restart == true ? restartStr : shutDownStr ) );
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

void CmdHandler::vanishHandler(QSharedPointer<Player> admin, const QString& subCmd)
{
    QString message{ "Admin [ %1 ]: You are now %2 to other Players..." };
    static const QString invisibleStr{ "Invisible" };
    static const QString visibleStr{ "Visible" };

    QString state{ visibleStr };

    bool isVisible{ admin->getIsVisible() };
    if ( !isVisible )
        state = visibleStr;

    if ( subCmd.isEmpty() )
    {
        if ( isVisible )
            state = invisibleStr;

        admin->setIsVisible( !isVisible );
    }
    else
    {
        if ( cmdTable->isSubCommand( GMCmds::Vanish, subCmd ) )
        {
            GMSubCmdIndexes idx{ GMSubCmdIndexes::Invalid };
            if ( !subCmd.isEmpty() )
                idx = cmdTable->getSubCmdIndex( GMCmds::Vanish, subCmd, false );

            switch ( static_cast<GMSubCmdIndexes>( idx ) )
            {
                case GMSubCmdIndexes::Zero: //Hide
                    {
                        if ( isVisible )
                        {
                            state = invisibleStr;
                            admin->setIsVisible( false );
                        }
                    }
                break;
                case GMSubCmdIndexes::One: //Show
                    {
                        if ( isVisible )
                        {
                            state = visibleStr;
                            admin->setIsVisible( true );
                        }
                    }
                break;
                case GMSubCmdIndexes::Two: //Status.
                    {
                        message = "Admin [ %1 ]: You are currently %2 to other Players...";
                    }
                break;
                case GMSubCmdIndexes::Three:
                case GMSubCmdIndexes::Four:
                case GMSubCmdIndexes::Five:
                case GMSubCmdIndexes::Six:
                case GMSubCmdIndexes::Seven:
                case GMSubCmdIndexes::Invalid:
                break;
            }
        }
    }
    message = message.arg( admin->getSernum_s() )
                     .arg( state );
    server->sendMasterMessage( message, admin, false );
}

void CmdHandler::pingHandler(QSharedPointer<Player> admin, const GMCmds& cmdIdx, const QString& subCmd, const QString& arg1)
{
    if ( admin == nullptr )
        return;

    GMSubCmdIndexes subIdx{ GMSubCmdIndexes::Invalid };
    if ( !subCmd.isEmpty() )
        subIdx = cmdTable->getSubCmdIndex( cmdIdx, subCmd, false );

    QSharedPointer<Player> target{ nullptr };
    QString pingString{ "" };
    switch( subIdx )
    {
        case GMSubCmdIndexes::Zero: //ip
            {
                target = this->findTarget( admin, arg1, cmdIdx, false, true );
                if ( target != nullptr )
                    pingString = this->getPingString( target, true );
            }
        break;
        case GMSubCmdIndexes::One: //soul
            {
                target = this->findTarget( admin, arg1, cmdIdx, false, true );
                if ( target != nullptr )
                    pingString = this->getPingString( target, false );
            }
        break;
        case GMSubCmdIndexes::Invalid: //Default to Player's Ping.
        default:
            {
                if ( admin != nullptr )
                    pingString = this->getPingString( admin, false );
            }
        break;
    }

    if ( !pingString.isEmpty() )
        server->sendMasterMessage( pingString, admin, false );
}

void CmdHandler::campHandler(QSharedPointer<Player> admin, const QString& serNum, const QString& subCmd, const GMCmds& index, const bool& soulSubCmd)
{
    QString msg{ "" };
    static const QString allowCurrent{ "Players can not your camp if it's considered old." };
    QString allowExempt{ "The Player [ %1 ] has been [ %2 ] your Camp Exemptions list." };
    QString allowNew{ "Players can enter your camp even if it's considered old." };
    QString lockMsg{ "Your Camp has been %1!" };

    QString overrideConfirm{ "Player [ %1 ] has had their camp status changed!" };
    QString overrideLockAppend{ " by Admin [ %1 ]" };
    QString overrideAllowAppend{ "Admin [ %1 ]: " };

    static const QString unlock{ "Unlocked" };
    static const QString lock{ "Locked" };

    static const QString removed{ "Removed From" };
    static const QString added{ "Added To" };

    //Swap to the targeted Player.
    QSharedPointer<Player> tmpPlr{ nullptr };
    bool override{ false };

    GMSubCmdIndexes idx{ GMSubCmdIndexes::Invalid };
    if ( !subCmd.isEmpty() )
        idx = cmdTable->getSubCmdIndex( index, subCmd, false );

    if ( soulSubCmd
      && !serNum.isEmpty() )
    {
        for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
        {
            tmpPlr = tmpPlayer;
            if ( tmpPlr != nullptr
              && admin != tmpPlr )
            {
                //Check target validity.
                if ( this->isTarget( tmpPlr, serNum, false ) )
                {
                    if ( static_cast<GMSubCmdIndexes>( idx ) != GMSubCmdIndexes::Four  //Allow Soul exempted from Admin Checking
                      && static_cast<GMSubCmdIndexes>( idx ) != GMSubCmdIndexes::Five )//Remove Soul exempted from admin checking.
                    {
                        GMRanks rank{ cmdTable->getSubCmdRank( index, cmdTable->getSubCmdIndex( index, "soul", false ) ) };
                        if ( this->canUseAdminCommands( admin, rank, "soul" ) )
                        {
                            override = this->canIssueAction( admin, tmpPlr, serNum, GMCmds::Camp, false );
                            if ( override )
                                break;
                        }
                        else
                            return;
                    }
                    else
                        break;
                }
            }
            tmpPlr = nullptr;
        }
    }

    if ( idx != GMSubCmdIndexes::Invalid ) //Status SubCommand
    {
        if ( soulSubCmd
          && tmpPlr == nullptr )
        {
            return;
        }

        switch ( static_cast<GMSubCmdIndexes>( idx ) )
        {
            case GMSubCmdIndexes::Zero: //Lock
                {
                    msg = lockMsg.arg( lock );
                    if ( override )
                        msg = msg.append( overrideLockAppend.arg( admin->getSernum_s() ) );

                    if ( tmpPlr != nullptr )
                        tmpPlr->setIsCampLocked( true );
                    else
                        admin->setIsCampLocked( true );
                }
            break;
            case GMSubCmdIndexes::One: //Unlock
                {
                    msg = lockMsg.arg( unlock );
                    if ( override )
                        msg = msg.append( overrideLockAppend.arg( admin->getSernum_s() ) );

                    if ( tmpPlr != nullptr )
                        tmpPlr->setIsCampLocked( false );
                    else
                        admin->setIsCampLocked( false );
                }
            break;
            case GMSubCmdIndexes::Two: //OnlyCurrent.
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
            case GMSubCmdIndexes::Three: //AllowAll
                {
                    msg = allowNew;
                    if ( override )
                        msg = msg.prepend( overrideAllowAppend.arg( admin->getSernum_s() ) );

                    if ( tmpPlr != nullptr )
                    {
                        tmpPlr->setIsCampOptOut( false );
                        tmpPlr->setIsCampLocked( false );
                    }
                    else
                    {
                        admin->setIsCampOptOut( false );
                        admin->setIsCampLocked( false );
                    }
                }
            break;
            case GMSubCmdIndexes::Four: //Allow. Only online Players may be exempted for storage.
            case GMSubCmdIndexes::Five:
                {
                    if ( !this->isTargetingSelf( admin, tmpPlr ) )
                    {
                        bool add{ true };
                        if ( idx == GMSubCmdIndexes::Five )
                            add = false;

                        if ( soulSubCmd
                          && tmpPlr != nullptr )
                        {
                            override = false;

                            msg = allowExempt;
                            msg = msg.arg( tmpPlr->getSernum_s() );

                            CampExemption* exemptions{ CampExemption::getInstance() };
                            exemptions->setIsWhitelisted( admin->getSernumHex_s(), tmpPlr->getSernumHex_s(), add );

                            if ( !add )
                                msg = msg.arg( removed );
                            else
                                msg = msg.arg( added );
                        }
                        else
                            return;
                    }
                }
            break;
            case GMSubCmdIndexes::Six:
                {

                }
            case GMSubCmdIndexes::Seven:
            case GMSubCmdIndexes::Invalid:
                {
                    //Unused commands.
                }
            break;
        }
    }
    else //Send Scene Status.
    {
        QString whiteList{ CampExemption::getInstance()->getWhiteListedUsers( admin->getSernumHex_s() ) };
        QString hasWhitelisted{ " The following Users are [ Allowed ]: %1" };
                hasWhitelisted = hasWhitelisted.arg( whiteList );

        static const QString notAllowedStr{ "Not Allowed" };
        static const QString allowedStr{ "Allowed" };

        msg = "Your camp is currently [ %1 ]. New players are [ %2 ] to enter your camp if it's considered old to their client.";
        if ( admin->getIsCampLocked() )
            msg = msg.arg( lock );
        else
            msg = msg.arg( unlock );

        if ( admin->getIsCampOptOut()
          || admin->getIsCampLocked() )
        {
            msg = msg.arg( notAllowedStr );
        }
        else
            msg = msg.arg( allowedStr );

        if ( !whiteList.isEmpty() )
            msg.append( hasWhitelisted );
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

QPair<qint64, TimePeriods> CmdHandler::getTimePeriodFromString(const QString& str)
{
    static const QMap<QChar, TimePeriods> periodMap
    {
        { 'y', TimePeriods::Years   },
        { 'd', TimePeriods::Days    },
        { 'h', TimePeriods::Hours   },
        { 'm', TimePeriods::Minutes },
        { 's', TimePeriods::Seconds }
    };

    TimePeriods time{ TimePeriods::Default };
    qint64 duration{ 0 };
    QString pStr;

    for ( const QChar& c : str )
    {
        if ( c.isDigit() )
        {
            pStr += c;
        }
        else if ( periodMap.contains( c.toLower() ) )
        {
            time = periodMap.value( c.toLower(), TimePeriods::Seconds );
            duration += pStr.toLongLong() * static_cast<qint64>(time);
            pStr.clear();
        }
        else //We only care about the first non-digit character, and if it is within the periodMap. Stop here.
            break;
    }

    // If there is no duration indicator, assume seconds.
    if ( time == TimePeriods::Default )
    {
        time = TimePeriods::Seconds;
        duration = pStr.toLongLong() * static_cast<qint64>( time );
    }
    return QPair<qint64, TimePeriods>( duration, time );
}

QString CmdHandler::getPingString(QSharedPointer<Player> target, const bool& targetIP)
{
    QString pingInfo{ "Ping for Player [ %1 ][ %2 ]; [ %3 ]." };
    if ( target != nullptr )
    {
        if ( targetIP )
            pingInfo = pingInfo.arg( target->getIPAddress() );
        else
            pingInfo = pingInfo.arg( target->getSernum_s() );

        pingInfo = pingInfo.arg( target->getPlrName() )
                           .arg( target->getPlrPingString() );

        return pingInfo;
    }
    return QString( "" );
}

QSharedPointer<Player> CmdHandler::findTarget(QSharedPointer<Player> admin, const QString& arg1, const GMCmds& command,
                                              const bool& all, const bool& override)
{
    bool target{ false };
    QSharedPointer<Player> tmpPlr{ nullptr };
    for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
    {
        if ( tmpPlayer != nullptr )
        {
            tmpPlr = tmpPlayer;
            if ( this->isTarget( tmpPlayer, arg1, all ) )
            {
                target = this->canIssueAction( admin, tmpPlayer, arg1, command, all, override );
                if ( target )
                    break;
            }
            tmpPlr = nullptr;
        }
        target = false;
    }

    if ( target
         && tmpPlr != nullptr )
    {
        return tmpPlr;
    }

    tmpPlr = nullptr;
    return tmpPlr;
}
