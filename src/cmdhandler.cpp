

#include "cmdhandler.hpp"
#include "includes.hpp"

//Initialize our accepted Command List.
const QStringList CmdHandler::commands =
{
    //enum CMDS{ BAN = 0, UNBAN = 1, KICK = 2, MUTE = 3, UNMUTE = 4, MSG = 5 };
    QStringList() << "ban" << "unban" << "kick"
                  << "mute" << "unmute" << "msg"
                  << "login" << "register" << "shutdown"
                  << "restart"
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
    QString sernum = plr->getSernum_s();
    QString alias = Helper::getStrStr( packet, "", "", ": " ).mid( 10 );
    QString msg = Helper::getStrStr( packet, ": ", ": ", "" );
            msg = msg.left( msg.length() - 2 );

    if ( !alias.isEmpty()
      && !msg.isEmpty() )
    {
        if ( msg.startsWith( "/" ) )
        {
            if ( msg.startsWith( "/cmd " ) )
                msg = msg.mid( msg.indexOf( "/cmd ", 0 ) + 4 );
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

                //Send the Comment to all connected Remote-Administrators
                //Then forward the message to the Comments dialog if enabled.
                if ( Settings::getFwdComments() )
                {
                    Player* tmpPlr{ nullptr };
                    QString message{ "Server comment from %1 [ %2 ]: %3" };
                    message = message.arg( plr->getAdminRank() >= 0
                                         ? "Admin"
                                         : "User" )
                                     .arg( sernum )
                                     .arg( msg );
                    for ( int i = 0; i < MAX_PLAYERS; ++i )
                    {
                        tmpPlr = server->getPlayer( i );
                        if (tmpPlr != nullptr)
                        {
                            if ( tmpPlr->getAdminRank() >= Ranks::GMASTER
                              && tmpPlr->getGotAuthPwd() )
                            {
                                server->sendMasterMessage( message, tmpPlr,
                                                           false );
                            }
                        }
                    }
                    emit newUserCommentSignal( sernum, alias, msg );
                }
            }
        }
    }
}

void CmdHandler::parseMix6Command(Player *plr, QString &packet)
{
    if ( plr != nullptr
      && !packet.isEmpty() )
    {
        QString cmd{ packet };
        if ( cmd.contains( ": " ) )
            cmd = Helper::getStrStr( packet, ": /cmd ", ": ", "" );
        else    //0100000FA0kickCE
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
    QString cmd, argType, arg1, arg2, arg3;

    stream >> cmd >> argType >> arg1 >> arg2 >> arg3;

    qint32 argIndex{ -1 };
    for ( int i = 0; i < commands.count(); ++i )
    {
        if ( commands.at( i ).compare( cmd, Qt::CaseInsensitive ) == 0 )
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
    if ( argType.compare( "all", Qt::CaseInsensitive ) == 0 )
    {
        if ( !arg1.isEmpty() )
            message = packet.mid( packet.indexOf( arg1 ) );
    }
    else
    {
        if ( !arg2.isEmpty() )
            message = packet.mid( packet.indexOf( arg2 ) );
    }

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
                if ( !message.isEmpty() )
                {
                    message.prepend( "Admin [ %1 ] to [ %2 ]: " );
                    message = message.arg( plr->getSernum_s() )
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
                if ( !argType.isEmpty() )
                    this->loginHandler( plr, argType );

                retn = false;
                logMsg = false;
            }
        break;
        case CMDS::REGISTER: //7
            {
                if ( !argType.isEmpty() )
                    this->registerHandler( plr, argType );

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
        default:
        break;
    }

    if ( logMsg )
    {
        QString log{ "adminUsage.txt" };
        QString logMsg{ "Remote-Admin: [ %1 ] issued the command [ %2 ] with "
                        "ArgType [ %3 ], Arg1 [ %4 ], Arg2 [ %5 ] and reason "
                        "[ %6 ]." };

        logMsg = logMsg.arg( plr->getSernum_s() )
                       .arg( cmd )
                       .arg( argType )
                       .arg( arg1 )
                       .arg( arg2 )
                       .arg( message );
        Helper::logToFile( log, logMsg, true, true );
    }

    if ( retn && canUseCommands )
    {
        QString valid{ "It has been done. I hope you entered the right "
                       "command!" };
        server->sendMasterMessage( valid, plr, false);
    }
    return retn;
}

void CmdHandler::banhandler(Player* plr, QString& argType, QString& arg1,
                            QString& message, bool all)
{
    QString reason{ "A Remote-Administrator has [ Banned ] you. "
                    "Reason: [ %2 ]." };

    bool ban{ false };
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
                ban = true;
            }
            else
                ban = false;

            if ( ban )
            {
                reason = reason.arg( message.isEmpty()
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
        banDialog->addIPBan( arg1, message );
    else
        banDialog->addSerNumBan( arg1, message );
}

void CmdHandler::unBanhandler(QString& argType, QString& arg1)
{
    if ( argType.compare( "ip", Qt::CaseInsensitive ) == 0 )
        banDialog->removeIPBan( arg1 );
    else
        banDialog->removeSerNumBan( arg1 );
}

void CmdHandler::kickHandler(QString& arg1, QString& message, bool all)
{
    QString reason{ "A Remote-Administrator has [ Kicked ] you. "
                    "Reason: [ %2 ]." };

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
    QString sernum{ plr->getSernum_s() };
    QString response{ "%1 %2 Password. Welcome!" };
    QString invalid{ "Incorrect" };
    QString valid{ "Correct" };

    bool disconnect{ false };

    QVariant pwd{ argType };
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
        if ( !pwd.toString().isEmpty()
          && Admin::cmpRemoteAdminPwd( sernum, pwd ) )
        {
            response = response.arg( valid );

            plr->setReqAuthPwd( false );
            plr->setGotAuthPwd( true );


            //Inform Other Users of this Remote-Admin's login if enabled.
            if ( Settings::getInformAdminLogin() )
            {
                QString message{ "Remote Admin [ "
                                 % sernum
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
    QString sernum{ plr->getSernum_s() };
    QString response{ "" };

    if ( plr->getReqNewAuthPwd()
      && !plr->getGotNewAuthPwd() )
    {

        QVariant pwd{ argType };
        QString pass( pwd.toString() );

        if ( adminDialog->makeAdmin( sernum, pass ) )
        {
            response = "You are now registered as an Admin with the "
                       "Server. You are currently Rank-0 (Game Master) "
                       "with limited commands.";

            plr->setReqNewAuthPwd( false );
            plr->setGotNewAuthPwd( true );

            plr->setReqAuthPwd( false );
            plr->setGotAuthPwd( true );

            //Inform Other Users of this Remote-Admin's login if enabled.
            if ( Settings::getInformAdminLogin() )
            {
                QString message{ "User [ "
                                 % sernum
                                 % " ] has Registered as a Remote Administrator with the server." };

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
