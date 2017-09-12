
#include "cmdhandler.hpp"
#include "includes.hpp"

//Initialize our accepted Command List.
const QString CmdHandler::commands[ ADMIN_COMMAND_COUNT ] =
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
    //Setup our Random Device
    randDev = new RandDev();
    server = svr;
}

CmdHandler::~CmdHandler(){}

bool CmdHandler::canUseAdminCommands(Player* plr)
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
            reason.append( " [ %1:%2 ]: %3" );
            reason = reason.arg( plr->getPublicIP() )
                           .arg( plr->getPublicPort() )
                           .arg( plr->getBioData() );

            User::addBan( nullptr, plr, reason );

            plr->setDisconnected( true );
            server->setIpDc( server->getIpDc() + 1 );
        }
        else
            plr->sendMessage( invalid );

        retn = false;
    }
    return retn;
}

void CmdHandler::parseMix5Command(Player* plr, QString& packet)
{
    if ( packet.isEmpty()
      || plr == nullptr )
    {
        return;
    }

    qint32 colIndex{ packet.indexOf( ": " ) };
    QString alias = packet.left( colIndex ).mid( 10 );

    QString msg{ packet.mid( colIndex + 2 ) };
            msg = msg.left( msg.length() - 2 );

    if ( !alias.isEmpty()
      && !msg.isEmpty() )
    {
        if ( msg.startsWith( "/" ) )
        {
            if ( msg.startsWith( "/cmd " ) )
            {
                msg = msg.mid( msg.indexOf( "/cmd ", 0,
                                            Qt::CaseInsensitive ) + 4 );
            }
            else
                msg = msg.mid( msg.indexOf( "/", 0 ) + 1 );

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
                    message = message.arg( plr->getAdminRank() >= Ranks::GMASTER
                                         ? "Admin"
                                         : "User" )
                                     .arg( plr->getSernum_s() )
                                     .arg( msg );
                    for ( int i = 0; i < MAX_PLAYERS; ++i )
                    {
                        tmpPlr = server->getPlayer( i );
                        if ( tmpPlr != nullptr )
                        {
                            if ( tmpPlr->getAdminRank() >= Ranks::GMASTER
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

void CmdHandler::parseMix6Command(Player *plr, QString &packet)
{
    QString cmd{ packet };

    if ( plr != nullptr
      && !packet.isEmpty() )
    {
        qint32 colIndex{ packet.indexOf( ": /cmd ", Qt::CaseInsensitive ) };
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
    bool logMsg{ Settings::getLogFiles() };
    bool retn{ false };
    bool all{ false };

    QTextStream stream( &packet );
    QString cmd;
    QString argType;
    QString arg1;
    QString arg2;
    QString arg3;

    stream >> cmd >> argType >> arg1 >> arg2 >> arg3;

    qint32 argIndex{ -1 };
    for ( int i = 0; i < ADMIN_COMMAND_COUNT; ++i )
    {
        if ( commands[ i ].compare( cmd, Qt::CaseInsensitive ) == 0 )
            argIndex = i;
    }

    if ( !argType.isEmpty() )
    {
        if ( argType.compare( "all", Qt::CaseInsensitive ) == 0 )
        {
            if ( plr->getAdminRank() >= Ranks::ADMIN
              || argIndex == CMDS::MSG )
            {
                all = true;
            }
            else    //Invalid Rank. Give generic response.
                return false;
        }
        else if ( argType.compare( "SOUL", Qt::CaseInsensitive ) == 0 )
        {
            if ( !( arg1.toInt( 0, 16 ) & MIN_HEX_SERNUM ) )
                arg1.prepend( "SOUL " );
        }
    }

    //Correctly handle "all" command reason/message.
    QString message{ "" };
    if ( argType.compare( "all", Qt::CaseInsensitive ) == 0
      && !arg1.isEmpty() )
    {
        message = packet.mid( packet.indexOf( arg1 ) );
    }
    else if ( !arg2.isEmpty() )
            message = packet.mid( packet.indexOf( arg2 ) );

    bool canUseCommands{ false };
    switch ( argIndex )
    {
        case CMDS::BAN: //0
            {
                canUseCommands = this->canUseAdminCommands( plr );
                if (( !arg1.isEmpty() && !argType.isEmpty() )
                  && canUseCommands )
                {
                    this->banhandler( plr, arg1, message, all );
                }
                retn = true;
            }
        break;
        case CMDS::UNBAN: //1
            {
                canUseCommands = this->canUseAdminCommands( plr );
                if (( !arg1.isEmpty() && !argType.isEmpty() )
                  && canUseCommands )
                {
                    this->unBanhandler( argType, arg1 );
                }
                retn = true;
            }
        break;
        case CMDS::KICK: //2
            {
                canUseCommands = this->canUseAdminCommands( plr );
                if ( !arg1.isEmpty()
                  && canUseCommands )
                {
                    this->kickHandler( arg1, message, all );
                }
                retn = true;
            }
        break;
        case CMDS::MUTE: //3
        case CMDS::UNMUTE: //4
            {
                canUseCommands = this->canUseAdminCommands( plr );
                if ( !arg1.isEmpty()
                  && canUseCommands )
                {
                    this->muteHandler( plr, arg1, argIndex, message, all );
                }
                retn = true;
            }
        break;
        case CMDS::MSG: //5
            {
                QString tmpMsg{ "" };
                if ( !message.isEmpty() )
                {
                    tmpMsg = message;
                    tmpMsg.prepend( "Admin [ %1 ] to [ %2 ]: " );
                    tmpMsg = tmpMsg.arg( plr->getSernum_s() )
                                   .arg( all
                                       ? "Everyone"
                                       : arg1 );
                }

                canUseCommands = this->canUseAdminCommands( plr );
                if ( !arg1.isEmpty()
                  && canUseCommands )
                {
                    this->msgHandler( arg1, tmpMsg, all );
                }
                retn = true;
            }
        break;
        case CMDS::LOGIN: //6
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
        case CMDS::REGISTER: //7
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
//        case CMDS::SHUTDOWN: //8
//            {
//                canUseCommands = this->canUseAdminCommands( plr );
//                if ( plr->getAdminRank() >= Ranks::OWNER
//                  && canUseCommands )
//                {
//                    this->shutDownHandler( plr, false );
//                    retn = true;
//                }
//            }
//        break;
//        case CMDS::RESTART: //9
//            {
//                canUseCommands = this->canUseAdminCommands( plr );
//                if ( plr->getAdminRank() >= Ranks::OWNER
//                  && canUseCommands )
//                {
//                    this->shutDownHandler( plr, true );
//                    retn = true;
//                }
//            }
//        break;
//        case CMDS::MKADMIN: //10
//            {
//                this->mkAdminHandler( plr, arg1, arg2 );
//            }
//        break;
//        case CMDS::RMADMIN: //11
//            {
//                this->rmAdminHandler( plr, arg1 );
//            }
//        break;
//        case CMDS::CHADMIN: //12
//            {
//                this->chAdminHandler( plr, arg1, arg2 );
//            }
//        break;
//        case CMDS::CHRULES: //13
//            {
//                this->chRulesHandler( plr, arg1 );
//            }
//        break;
//        case CMDS::GETCOMMENTS: //14
//            {
//                this->getCommentsHandler( plr, arg1 );
//            }
//        break;
//        case CMDS::CHSETTINGS: //15
//            {
//                this->chSettingsHandler( plr, arg1, arg2 );
//            }
//        break;
//        case CMDS::VANISH: //16
//            {
//                this->vanishHandler( plr );
//            }
//        break;
//        case : //16+
//        break;
        case CMDS::VERSION: //17
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

    msg = msg.arg( plr->getSernum_s() )
             .arg( cmd )
             .arg( argType )
             .arg( arg1 )
             .arg( arg2 )
             .arg( message );

    //The command was a Message, do not send command information to
    //online Users.
    if ( argIndex != CMDS::MSG )
    {
        if ( retn && canUseCommands )
            plr->sendMessage( msg );
    }

    if ( logMsg )
    {
        QString log{ "logs/adminUsage.txt" };
        Helper::logToFile( log, msg, true, true );
    }
    return retn;
}

void CmdHandler::banhandler(Player* plr, QString& arg1, QString& message,
                            bool all)
{
    QString sernum = Helper::serNumToHexStr( arg1 );
    QString reason{ "Remote-Admin [ %1 ] has [ Banned ] you. "
                    "Reason: [ %2 ]." };

    bool ban{ false };
    Player* tmpPlr{ nullptr };

    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr )
        {
            if ( tmpPlr->getPublicIP() == arg1
              || tmpPlr->getSernumHex_s() == sernum
              || all )
            {
                ban = true;
            }
            else
                ban = false;

            if ( ban )
            {
                reason = reason.arg( plr->getSernum_s() )
                               .arg( message.isEmpty()
                                   ? "No Reason!"
                                   : message );
                if ( !reason.isEmpty() )
                    tmpPlr->sendMessage( reason );

                User::addBan( plr, tmpPlr, message, true );
                tmpPlr->setDisconnected( true );
            }
        }
        ban = false;
    }
}

void CmdHandler::unBanhandler(QString& argType, QString& arg1)
{
    QString sernum = Helper::serNumToHexStr( arg1 );
    if ( argType.compare( "ip", Qt::CaseInsensitive ) == 0 )
        User::removeBan( arg1, 2 );
    else
        User::removeBan( sernum, 0 );
}

void CmdHandler::kickHandler(QString& arg1, QString& message, bool all)
{
    QString reason{ "A Remote-Administrator has [ Kicked ] you. "
                    "Reason: [ %1 ]." };

    Player* tmpPlr{ nullptr };
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr )
        {
            if ( tmpPlr->getPublicIP() == arg1
              || tmpPlr->getSernum_s() == arg1
              || all )
            {
                reason = reason.arg( message.isEmpty()
                                   ? "No Reason!"
                                   : message );
                if ( !reason.isEmpty() )
                    tmpPlr->sendMessage( reason );

                tmpPlr->setDisconnected( true );
            }
        }
    }
}

void CmdHandler::muteHandler(Player* plr, QString& arg1, qint32 argIndex,
                             QString& message, bool all)
{
    QString msg{ "Remote-Admin [ %1 ] %2 [ %3 ]'s Network. "
                 "Reason: [ %4 ]." };

    Player* tmpPlr{ nullptr };
    for ( int i = 0; i < MAX_PLAYERS; ++i )
    {
        tmpPlr = server->getPlayer( i );
        if ( tmpPlr != nullptr )
        {
            if ( tmpPlr->getPublicIP() == arg1
              || tmpPlr->getSernum_s() == arg1
              || all )
            {
                msg = msg.arg( plr->getSernum_s() )
                          .arg( argIndex == CMDS::MUTE
                              ? "Muted"
                              : "Un-Muted" )
                          .arg( arg1 )
                          .arg( message.isEmpty()
                              ? "No Reason!"
                              : message );

                if ( argIndex == CMDS::MUTE )
                    tmpPlr->setNetworkMuted( true, msg );
                else
                    tmpPlr->setNetworkMuted( false, msg );
            }
        }
    }
}

void CmdHandler::msgHandler(QString& arg1, QString& message, bool all)
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

void CmdHandler::loginHandler(Player* plr, QString& argType)
{
    QString response{ "%1 %2 Password." };
    QString invalid{ "Incorrect" };
    QString valid{ "Correct" };

    bool disconnect{ false };

    QString pwd{ argType };
    if ( plr->getSvrPwdRequested()
      && !plr->getSvrPwdReceived() )
    {
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
        response = response.arg( "Server" );
    }
    else if ( !plr->getAdminPwdReceived()
           || plr->getAdminPwdRequested() )
    {
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
                QString message{ "Remote Admin [ "
                                 % plr->getSernum_s()
                                 % " ] has Authenticated with the server." };

                Player* tmpPlr{ nullptr };
                for ( int i = 0; i < MAX_PLAYERS; ++i )
                {
                    tmpPlr = server->getPlayer( i );
                    if ( tmpPlr != nullptr )
                    {
                        if ( tmpPlr->getAdminRank() >= Ranks::GMASTER
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
        response = response.arg( "Admin" );
    }

    if ( !response.isEmpty() )
        plr->sendMessage( response );

    if ( disconnect )
        plr->setDisconnected( true );
}

void CmdHandler::registerHandler(Player* plr, QString& argType)
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
        QString message{ "User [ "
                       % plr->getSernum_s()
                       % " ] has Registered as a Remote "
                         "Administrator with the server." };

        Player* tmpPlr{ nullptr };
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            tmpPlr = server->getPlayer( i );
            if ( tmpPlr != nullptr )
            {
                if ( tmpPlr->getAdminRank() >= Ranks::GMASTER
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

void CmdHandler::shutDownHandler(Player* plr, bool restart)
{
    QString message{ "Admin [ %1 ]: The Server will %2 in 30 seconds..." };
            message = message.arg( plr->getSernum_s() );

    QTimer* timer = new QTimer();
    QObject::connect( timer, &QTimer::timeout, timer,
    [=]()
    {
        if ( restart )
        {
            QProcess proc;
            proc.startDetached( qApp->applicationFilePath(),
                                qApp->arguments() );
        }
        qApp->quit();
    });

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

//void CmdHandler::chRulesHandler(Player* plr, QString& rules)
//{

//}

//void CmdHandler::getCommentsHandler(Player* plr, QString& arg)
//{

//}

//void CmdHandler::chSettingsHandler(Player* plr, QString& setting,
//                                   QString& value)
//{

//}

//void CmdHandler::vanishHandler(Player* plr)
//{

//}
