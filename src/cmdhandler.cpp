

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
    "vanish"
};

CmdHandler::CmdHandler(QObject* parent, ServerInfo* svr,
                       Admin* aDlg)
    : QObject(parent)
{
    //Setup our Random Device
    randDev = new RandDev();

    adminDialog = aDlg;
    server = svr;

    if ( adminDialog != nullptr )
        banDialog = adminDialog->getBanDialog();
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
        if ( plr->getGotAuthPwd() )
            retn = true;
        else
            server->sendMasterMessage( unauth, plr, false );
    }
    else
    {
        plr->setCmdAttempts( plr->getCmdAttempts() + 1 );
        invalid = invalid.arg( MAX_CMD_ATTEMPTS - plr->getCmdAttempts() );

        if ( plr->getCmdAttempts() >= MAX_CMD_ATTEMPTS )
        {
            QString reason = QString( "Auto-Banish; <Unregistered Remote "
                                      "Admin: [ %1 ] command attempts>" )
                                 .arg( plr->getCmdAttempts() );

            server->sendMasterMessage( reason, plr, false );

            //Append BIO data to the reason for the Ban log.
            reason.append( " [ %1:%2 ]: %3" );
            reason = reason.arg( plr->getPublicIP() )
                           .arg( plr->getPublicPort() )
                           .arg( QString( plr->getBioData() ) );
            banDialog->addIPBan( plr->getPublicIP(), reason );

            plr->setSoftDisconnect( true );
            server->setIpDc( server->getIpDc() + 1 );
        }
        else
            server->sendMasterMessage( invalid, plr, false );

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
    QString alias = packet.left( colIndex + 2 ).mid( 10 );

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
                    server->sendMasterMessage( "Echo: " % msg, plr, false );

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
                              && tmpPlr->getGotAuthPwd() )
                            {
                                server->sendMasterMessage( message, tmpPlr,
                                                           false );
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
                    this->banhandler( plr, argType, arg1, message, all );
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
                    this->muteHandler( arg1, argIndex, all );
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
                    this->msgHandler( arg1, message, all );
                }
                retn = true;
            }
        break;
        case CMDS::LOGIN: //6
            {
                if ( !argType.isEmpty()
                  && plr->getReqAuthPwd() )
                {
                    this->loginHandler( plr, argType );
                }
                retn = false;
                logMsg = false;
            }
        break;
        case CMDS::REGISTER: //7
            {
                if ( !argType.isEmpty()
                  && plr->getReqNewAuthPwd() )
                {
                    this->registerHandler( plr, argType );
                }
                retn = false;
                logMsg = false;
            }
        break;
        case CMDS::SHUTDOWN: //8
            {
                canUseCommands = this->canUseAdminCommands( plr );
                if ( plr->getAdminRank() >= Ranks::OWNER
                  && canUseCommands )
                {
                    this->shutDownHandler( plr, false );
                    retn = true;
                }
            }
        break;
        case CMDS::RESTART: //9
            {
                canUseCommands = this->canUseAdminCommands( plr );
                if ( plr->getAdminRank() >= Ranks::OWNER
                  && canUseCommands )
                {
                    this->shutDownHandler( plr, true );
                    retn = true;
                }
            }
        break;
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

    if ( retn && canUseCommands )
        server->sendMasterMessage( msg, plr, false);

    if ( logMsg )
    {
        QString log{ "adminUsage.txt" };
        Helper::logToFile( log, msg, true, true );
    }
    return retn;
}

void CmdHandler::banhandler(Player* plr, QString& argType, QString& arg1,
                            QString& message, bool all)
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
                    server->sendMasterMessage( reason, tmpPlr, false );

                banDialog->remoteAddSerNumBan( plr, tmpPlr, message );
                banDialog->remoteAddIPBan( plr, tmpPlr, message );

                tmpPlr->setSoftDisconnect( true );
            }
        }
        ban = false;
    }

    if ( argType.compare( "ip", Qt::CaseInsensitive ) == 0 )
        banDialog->addIPBan( sernum, message );
    else
        banDialog->addSerNumBan( sernum, message );
}

void CmdHandler::unBanhandler(QString& argType, QString& arg1)
{
    QString sernum = Helper::serNumToHexStr( arg1 );
    if ( argType.compare( "ip", Qt::CaseInsensitive ) == 0 )
        banDialog->removeIPBan( arg1 );
    else
        banDialog->removeSerNumBan( sernum );
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
                    server->sendMasterMessage( reason, tmpPlr, false );

                tmpPlr->setSoftDisconnect( true );
            }
        }
    }
}

void CmdHandler::muteHandler(QString& arg1, qint32 argIndex, bool all)
{
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
                if ( argIndex == CMDS::MUTE )
                    tmpPlr->setNetworkMuted( true );
                else
                    tmpPlr->setNetworkMuted( false );
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
                        server->sendMasterMessage( message, tmpPlr, false );
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
    QString response{ "%1 %2 Password. Welcome!" };
    QString invalid{ "Incorrect" };
    QString valid{ "Correct" };

    bool disconnect{ false };

    QString pwd{ argType };
    if ( plr->getPwdRequested()
      && !plr->getEnteredPwd() )
    {
        if ( Settings::cmpServerPassword( pwd ) )
        {
            response = response.arg( valid );

            plr->setPwdRequested( false );
            plr->setEnteredPwd( true );
        }
        else
        {
            response = response.arg( invalid );
            disconnect = true;
        }
        response = response.arg( "Server" );
    }
    else if ( !plr->getGotAuthPwd()
           || plr->getReqAuthPwd() )
    {
        QString sernum{ plr->getSernumHex_s() };
        if ( !pwd.isEmpty()
          && Admin::cmpRemoteAdminPwd( sernum, pwd ) )
        {
            response = response.arg( valid );

            plr->setReqAuthPwd( false );
            plr->setGotAuthPwd( true );


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
                          && tmpPlr->getGotAuthPwd() )
                        {
                            //Do not Inform our own Admin.. --Redundant..
                            if ( tmpPlr != plr )
                            {
                                server->sendMasterMessage( message, tmpPlr,
                                                           false );
                            }
                        }
                    }
                }
            }
        }
        else
        {
            response = response.arg( invalid );
            disconnect = true;
        }
        response = response.arg( "Admin" );
    }

    if ( !response.isEmpty() )
        server->sendMasterMessage( response, plr, false );

    if ( disconnect )
        plr->setSoftDisconnect( true );
}

void CmdHandler::registerHandler(Player* plr, QString& argType)
{
    QString response{ "" };

    if ( !plr->getGotNewAuthPwd() )
    {
        QString sernum{ plr->getSernumHex_s() };
        if ( adminDialog->makeAdmin( sernum, argType ) )
        {
            response = "You are now registered as an Admin with the "
                       "Server. You are currently Rank-1 (Game Master) "
                       "with limited commands.";

            plr->setReqNewAuthPwd( false );
            plr->setGotNewAuthPwd( true );

            plr->setReqAuthPwd( false );
            plr->setGotAuthPwd( true );

            //Inform Other Users of this Remote-Admin's login if enabled.
            if ( Settings::getInformAdminLogin() )
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
                          && tmpPlr->getGotAuthPwd() )
                        {
                            //Do not Inform our own Admin.. --Redundant..
                            if ( tmpPlr != plr )
                                server->sendMasterMessage( message, tmpPlr,
                                                           false );
                        }
                    }
                }
            }
            adminDialog->loadServerAdmins();
        }
        else
        {
            response = "You were not registered as an Admin with the "
                       "Server. It seems something has gone wrong or "
                       "you were already registered as an Admin.";

            plr->setReqNewAuthPwd( false );
            plr->setGotNewAuthPwd( false );
        }
    }

    if ( !response.isEmpty() )
        server->sendMasterMessage( response, plr, false );
}

void CmdHandler::shutDownHandler(Player* plr, bool restart)
{
    QString message{ "Admin [ %1 ]: The Server will be %2 in 30 seconds..." };
            message = message.arg( plr->getSernum_s() );

    QTimer* timer = new QTimer();
    QObject::connect( timer, &QTimer::timeout, [=]()
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
        message = message.arg( "restarting" );
    else
        message = message.arg( "shutting down" );

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
