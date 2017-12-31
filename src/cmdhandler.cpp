
//Class includes.
#include "cmdhandler.hpp"

//ReMix includes.
#include "remixtabwidget.hpp"
#include "serverinfo.hpp"
#include "settings.hpp"
#include "randdev.hpp"
#include "player.hpp"
#include "helper.hpp"
#include "user.hpp"

//Qt includes.
#include <QtCore>

//Initialize our accepted Command List.
const QString CmdHandler::commands[ GM_COMMAND_COUNT ] =
{
    "ban",
    "unban",
    "kick",
    "mute",
    "unmute",
    "msg",
    "login",
    "register",
    "shutdown",
    "restart",
    "mkadmin",
    "rmadmin",
    "dmadmin",
    "chrules",
    "getcomments",
    "chsettings",
    "vanish",
    "version"
};

CmdHandler::CmdHandler(QObject* parent, ServerInfo* svr)
    : QObject(parent)
{
    server = svr;
}

CmdHandler::~CmdHandler(){}

bool CmdHandler::canUseAdminCommands(Player* plr) const
{
    bool retn{ false };
    QString unauth{ "While your SerNum is registered as a Remote Admin, "
                    "you are not Authenticated and are unable to use these "
                    "commands. Please reply to this message with "
                    "(/login *PASS) and the server will "
                    "authenticate you." };

    QString invalid{ "Your SerNum is not registered as a Remote Admin. "
                     "Please refrain from attempting to use Remote Admin "
                     "commands as you will be banned after [ %1 ] "
                     "more tries." };

    if ( plr->getIsAdmin() )
    {
        retn = false;
        if ( plr->getAdminPwdReceived() )
            retn = true;
        else
            plr->sendMessage( unauth );
    }
    else
    {
        plr->setCmdAttempts( plr->getCmdAttempts() + 1 );
        invalid = invalid.arg( MAX_CMD_ATTEMPTS - plr->getCmdAttempts() );

        if ( plr->getCmdAttempts() >= MAX_CMD_ATTEMPTS )
        {
            QString reason = "Auto-Banish; <Unregistered Remote Admin: [ %1 ] "
                             "command attempts>";

            reason = reason.arg( plr->getCmdAttempts() );
            plr->sendMessage( reason );

            //Append BIO data to the reason for the Ban log.
            QString append{ " [ %1 ], %2" };
                    append = append.arg( plr->getPublicIP(),
                                         plr->getBioData() );
            reason.append( append );

            User::addBan( nullptr, plr, reason );
            Helper::logToFile( Helper::BAN, reason, true, true );

            plr->setDisconnected( true, DCTypes::IPDC );
        }
        else
            plr->sendMessage( invalid );

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
                    plr->sendMessage( "Echo: " % msg );

                if ( Settings::getFwdComments() )
                {
                    Player* tmpPlr{ nullptr };
                    QString message{ "Server comment from %1 [ %2 ]: %3" };
                    QString user{ "User" };
                    if ( this->getAdminRank( plr ) >= GMRanks::GMaster )
                        user = "Admin";

                    message = message.arg( user,
                                           plr->getSernum_s(),
                                           msg );
                    for ( int i = 0; i < MAX_PLAYERS; ++i )
                    {
                        tmpPlr = server->getPlayer( i );
                        if ( tmpPlr != nullptr )
                        {
                            if ( this->getAdminRank( tmpPlr ) >= GMRanks::GMaster
                              && tmpPlr->getAdminPwdReceived() )
                            {
                                tmpPlr->sendMessage( message );
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
    QString argType;
    QString arg1;
    QString arg2;
    QString arg3;

    stream >> cmd >> argType >> arg1 >> arg2 >> arg3;

    GMCmds argIndex{ GMCmds::Invalid };
    for ( int i = 0; i < GM_COMMAND_COUNT; ++i )
    {
        if ( Helper::cmpStrings( commands[ i ], cmd ) )
            argIndex = static_cast<GMCmds>( i );
    }

    if ( !argType.isEmpty() )
    {
        if ( Helper::cmpStrings( argType, "all" ) )
        {
            if ( this->getAdminRank( plr ) >= GMRanks::Admin
              || argIndex == GMCmds::Message )
            {
                all = true;
            }
            else    //Invalid Rank. Give generic response.
                return false;
        }
        else if ( Helper::cmpStrings( argType, "SOUL" ) )
        {
            if ( !( arg1.toInt( 0, 16 ) & MIN_HEX_SERNUM ) )
                arg1.prepend( "SOUL " );
        }
    }

    //Correctly handle "all" command reason/message.
    QString message{ "" };
    if ( Helper::cmpStrings( argType, "all" )
      && !arg1.isEmpty() )
    {
        message = packet.mid( Helper::getStrIndex( packet, arg1 ) );
    }
    else if ( !arg2.isEmpty() )
            message = packet.mid( Helper::getStrIndex( packet, arg2 ) );

    bool canUseCommands{ false };
    switch ( argIndex )
    {
        case GMCmds::Ban: //0
            {
                if ( this->validateAdmin( plr, GMRanks::GMaster )
                  && ( !arg1.isEmpty()
                    && !argType.isEmpty() ) )
                    {
                        this->banhandler( plr, arg1, message, all );
                    }
                retn = true;
            }
        break;
        case GMCmds::UnBan: //1
            {
                if ( this->validateAdmin( plr, GMRanks::GMaster )
                  && ( !arg1.isEmpty()
                    && !argType.isEmpty() ) )
                {
                     this->unBanhandler( argType, arg1 );
                }
                retn = true;
            }
        break;
        case GMCmds::Kick: //2
            {
                if ( this->validateAdmin( plr, GMRanks::GMaster )
                  && !arg1.isEmpty() )
                {
                    this->kickHandler( plr, arg1, message, all );
                }
                retn = true;
            }
        break;
        case GMCmds::Mute: //3
        case GMCmds::UnMute: //4
            {
                if ( this->validateAdmin( plr, GMRanks::GMaster )
                  && !arg1.isEmpty() )
                {
                    this->muteHandler( plr, arg1, argIndex, message, all );
                }
                retn = true;
            }
        break;
        case GMCmds::Message: //5
            {
                QString tmpMsg{ "" };
                if ( !message.isEmpty() )
                {
                    tmpMsg = message;
                    tmpMsg.prepend( "Admin [ %1 ] to [ %2 ]: " );
                    tmpMsg = tmpMsg.arg( plr->getSernum_s(),
                                         all ? "Everyone" : arg1 );
                }

                if ( this->validateAdmin( plr, GMRanks::GMaster )
                  && !arg1.isEmpty() )
                {
                    this->msgHandler( arg1, tmpMsg, all );
                }
                retn = true;
            }
        break;
        case GMCmds::Login: //6
            {
                if ( !argType.isEmpty() )
                {
                    if ( ( ( plr->getAdminPwdRequested()
                       || !plr->getAdminPwdReceived() )
                      || ( plr->getSvrPwdRequested()
                       && !plr->getSvrPwdReceived() ) ) )
                    {
                        this->loginHandler( plr, argType );
                    }
                }
                retn = false;
                logMsg = false;
            }
        break;
        case GMCmds::Register: //7
            {
                if ( !argType.isEmpty()
                  && plr->getNewAdminPwdRequested() )
                {
                    this->registerHandler( plr, argType );
                }
                retn = false;
                logMsg = false;
            }
        break;
        case GMCmds::ShutDown: //8
            {
                if ( this->validateAdmin( plr, GMRanks::Owner ) )
                {
                    this->shutDownHandler( plr, false );
                    retn = true;
                }
            }
        break;
        case GMCmds::ReStart: //9
            {
                if ( this->validateAdmin( plr, GMRanks::Owner ) )
                {
                    this->shutDownHandler( plr, true );
                    retn = true;
                }
            }
        break;
//        case GMCmds::MKADMIN: //10
//            {
//                this->mkAdminHandler( plr, arg1, arg2 );
//            }
//        break;
//        case GMCmds::RMADMIN: //11
//            {
//                this->rmAdminHandler( plr, arg1 );
//            }
//        break;
//        case GMCmds::CHADMIN: //12
//            {
//                this->chAdminHandler( plr, arg1, arg2 );
//            }
//        break;
//        case GMCmds::ChRules: //13
//            {
//                if ( this->validateAdmin( plr, GMRanks::CoAdmin ) )
//                    this->chRulesHandler( plr, arg1, QVariant() );
//            }
//        break;
//        case GMCmds::GETCOMMENTS: //14
//            {
//                this->getCommentsHandler( plr, arg1 );
//            }
//        break;
//        case GMCmds::CHSETTINGS: //15
//            {
//                this->chSettingsHandler( plr, arg1, arg2 );
//            }
//        break;
        case GMCmds::Vanish: //16
            {
                if ( this->validateAdmin( plr, GMRanks::GMaster ) )
                    plr->setIsInvisible( !plr->getIsInvisible() );
            }
        break;
        case GMCmds::Version: //17
            {
                QString ver{ "ReMix Version: [ %1 ]" };
                        ver = ver.arg( QString( REMIX_VERSION ) );
                if ( plr != nullptr )
                    plr->sendMessage( ver );
            }
        break;
        default:
        break;
    }

    QString msg{ "Remote-Admin: [ %1 ] issued the command [ %2 ] with "
                 "ArgType [ %3 ], Arg1 [ %4 ], Arg2 [ %5 ] and Message "
                 "[ %6 ]." };

    msg = msg.arg( plr->getSernum_s(),
                   cmd,
                   argType,
                   arg1,
                   arg2,
                   message );

    //The command was a Message, do not send command information to
    //online Users.
    if ( argIndex != GMCmds::Message )
    {
        if ( retn && canUseCommands )
            plr->sendMessage( msg );
    }

    if ( logMsg )
        Helper::logToFile( Helper::ADMIN, msg, true, true );

    return retn;
}

bool CmdHandler::canIssueAction(Player* admin, Player* target,
                                const QString& arg1, const bool& all)
{
    //Remote commands cannot affect the issuer.
    if ( admin == target )
        return false;

    //Remote commands cannot affect other remote administrators.
    if ( target->getIsAdmin() )
        return false;

    //The target matches the Remote-Administrators conditions.
    QString sernum = Helper::serNumToHexStr( arg1 );
    if ( target->getPublicIP() == arg1
      || target->getSernumHex_s() == sernum
      || all )
    {
        return true;
    }

    //Fallthrough, command cannot be issued.
    return false;
}

bool CmdHandler::validateAdmin(Player* plr, const GMRanks& rank)
{
    return  ( ( this->getAdminRank( plr ) >= rank )
             && this->canUseAdminCommands( plr ) );
}

GMRanks CmdHandler::getAdminRank(Player* plr)
{
    return static_cast<GMRanks>( plr->getAdminRank() );
}

void CmdHandler::banhandler(Player* plr, const QString& arg1,
                            const QString& message, const bool& all)
{
    QString sernum = Helper::serNumToHexStr( arg1 );
    QString reason{ "Remote-Admin [ %1 ] has [ Banned ] you. "
                    "Reason: [ %2 ]." };
    QString msg = message;

    bool ban{ false };
    Player* tmpPlr{ nullptr };

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr )
        {
            //Check target validity.
            if ( this->canIssueAction( plr, tmpPlr, arg1, all ) )
            {
                ban = true;
            }
            else
                ban = false;

            if ( ban )
            {
                if ( msg.isEmpty() )
                    msg = "No Reason!";

                reason = reason.arg( plr->getSernum_s(), msg );
                if ( !reason.isEmpty() )
                    tmpPlr->sendMessage( reason );

                msg = msg.prepend( "Remote-Banish; " );
                User::addBan( plr, tmpPlr, msg, true );

                msg = msg.append( ": [ %1 ], [ %2 ]" );
                msg = msg.arg( plr->getSernum_s(),
                               plr->getBioData() );

                Helper::logToFile( Helper::BAN, msg, true, true );

                tmpPlr->setDisconnected( true, DCTypes::IPDC );
            }
        }
        ban = false;
    }
}

void CmdHandler::unBanhandler(const QString& argType, const QString& arg1)
{
    QString sernum = Helper::serNumToHexStr( arg1 );
    if ( Helper::cmpStrings( argType, "ip" ) )
        User::removeBan( arg1, 2 );
    else
        User::removeBan( sernum, 0 );
}

void CmdHandler::kickHandler(Player* plr, const QString& arg1,
                             const QString& message, const bool& all)
{
    QString reason{ "Remote-Admin [ %1 ] has [ Kicked ] you. "
                    "Reason: [ %2 ]." };

    QString msg = message;
    if ( msg.isEmpty() )
        msg = "No Reason!";

    reason = reason.arg( plr->getSernum_s(), message );

    Player* tmpPlr{ nullptr };
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr )
        {
            //Check target validity.
            if ( this->canIssueAction( plr, tmpPlr, arg1, all ) )
            {
                tmpPlr->sendMessage( reason );

                reason = "Remote-Kick; %1: [ %2 ], [ %3 ]";
                reason = reason.arg( msg,
                                     tmpPlr->getSernum_s(),
                                     tmpPlr->getBioData() );
                Helper::logToFile( Helper::DC, reason, true, true );

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
        if ( tmpPlr != nullptr )
        {
            //Check target validity.
            if ( this->canIssueAction( plr, tmpPlr, arg1, all ) )
            {
                msg = msg.arg( plr->getSernum_s(),
                               argIndex == GMCmds::Mute
                                ? "Muted" : "Un-Muted",
                               arg1,
                               message.isEmpty()
                                ? "No Reason!" : message );

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
                        tmpPlr->sendMessage( message );
                    }
                }
            }
        }
        else
            server->sendMasterMessage( message, nullptr, all );
    }
}

void CmdHandler::loginHandler(Player* plr, const QString& argType)
{
    QString response{ "%1 %2 Password." };
    QString invalid{ "Incorrect" };
    QString valid{ "Correct" };

    QString pwdTypes[ 2 ]{ "Server", "Admin" };

    bool disconnect{ false };
    PwdTypes pwdType{ PwdTypes::Invalid };

    QString pwd{ argType };
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
                                tmpPlr->sendMessage( message );
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
        plr->sendMessage( response );

    if ( disconnect )
    {
        if ( pwdType != PwdTypes::Invalid )
        {
            QString reason{ "Auto-Disconnect; Invalid %1 password: "
                            "[ %2 ], [ %3 ]" };
            reason = reason.arg( pwdTypes[ static_cast<int>( pwdType ) ],
                                 plr->getSernum_s(),
                                 plr->getBioData() );
            Helper::logToFile( Helper::DC, reason, true, true );
        }
        plr->setDisconnected( true, DCTypes::IPDC );
    }
}

void CmdHandler::registerHandler(Player* plr, const QString& argType)
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
            if ( User::makeAdmin( sernum, argType ) )
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
                        tmpPlr->sendMessage( message );
                }
            }
        }
    }

    if ( !response.isEmpty() )
        plr->sendMessage( response );
}

void CmdHandler::shutDownHandler(Player* plr, const bool& restart)
{
    QString message{ "Admin [ %1 ]: The Server will %2 in 30 seconds..." };
            message = message.arg( plr->getSernum_s() );

    QTimer* timer = new QTimer();

    ServerInfo* plrServer = plr->getServerInfo();
    if ( plrServer != nullptr )
    {

        QObject::connect( timer, &QTimer::timeout, timer,
        [=]()
        {
            ReMixTabWidget::remoteCloseServer( plrServer, restart );

            timer->disconnect();
            timer->deleteLater();
        });
    }

    if ( restart )
        message = message.arg( "restart" );
    else
        message = message.arg( "shut down" );

    timer->start( 30000 );

    if ( !message.isEmpty() )
        server->sendMasterMessage( message, nullptr, true );
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
