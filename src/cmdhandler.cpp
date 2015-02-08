

#include "cmdhandler.hpp"
#include "includes.hpp"

//Initialize our accepted Command List.
const QStringList CmdHandler::commands =
{
    //enum CMDS{ BAN = 0, UNBAN = 1, KICK = 2, MUTE = 3, UNMUTE = 4, MSG = 5 };
    QStringList() << "ban" << "unban" << "kick"
                  << "mute" << "unmute" << "msg"
                  << "login" << "register"
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

    QString sernum = plr->getSernum_s();
    if ( AdminHelper::getIsRemoteAdmin( sernum ) )
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
            emit newUserCommentSignal( sernum, alias, msg );
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

    bool canUseCommand{ this->canUseAdminCommands( plr ) };
    switch ( argIndex )
    {
        case CMDS::BAN: //0
            {
                if ( !arg1.isEmpty()
                  && canUseCommand )
                {
                    this->banhandler( plr, argType, arg1, message, all );
                }
                retn = true;
            }
        break;
        case CMDS::UNBAN: //1
            {
                if ( !arg1.isEmpty()
                  && canUseCommand )
                {
                    this->unBanhandler( argType, arg1 );
                }
                retn = true;
            }
        break;
        case CMDS::KICK: //2
            {
                if ( !arg1.isEmpty()
                  && canUseCommand )
                {
                    this->kickHandler( arg1, message, all );
                }
                retn = true;
            }
        break;
        case CMDS::MUTE: //3
        case CMDS::UNMUTE: //4
            {
                if ( !arg1.isEmpty()
                  && canUseCommand )
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
                    message.prepend( "Admin [ " % plr->getSernum_s()
                                   % " ]: " );
                }

                if ( !arg1.isEmpty()
                  && canUseCommand )
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
        default:
        break;
    }

    if ( logMsg )
    {
        QString log{ "adminUsage.txt" };
        QString logMsg{ "Remote-Admin: [ %1 ] issued the command [ %2 ] with "
                        "argument [ %3 ] and reason [ %4 ]." };

        logMsg = logMsg.arg( plr->getSernum_s() )
                       .arg( cmd )
                       .arg( arg1 )
                       .arg( message );
        Helper::logToFile( log, logMsg, true, true );
    }

    if ( retn && canUseCommand )
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
    QString reason{ "A Remote-Administrator has [ %1 ] you. "
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
                reason = reason.arg( "Banned" )
                               .arg( message.isEmpty()
                                   ? "No Reason!"
                                   : message );
                if ( !reason.isEmpty() )
                {
                    server->sendMasterMessage( reason,
                                               tmpPlr,
                                               false );
                }

                banDialog->remoteAddSerNumBan( plr,
                                               tmpPlr,
                                               message );

                banDialog->remoteAddIPBan( plr,
                                           tmpPlr,
                                           message );

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
    QString reason{ "A Remote-Administrator has [ %1 ] you. "
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
                reason = reason.arg( "Kicked" )
                               .arg( message.isEmpty()
                                   ? "No Reason!"
                                   : message );
                if ( !reason.isEmpty() )
                {
                    server->sendMasterMessage( reason, tmpPlr,
                                               false );
                }
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
                    server->sendMasterMessage( message, tmpPlr,
                                               false );
                }
            }
        }
    }
    else
    {
        server->sendMasterMessage( message,
                                   nullptr,
                                   all );
    }
}

void CmdHandler::loginHandler(Player* plr, QString& argType)
{
    QString sernum{ plr->getSernum_s() };
    QString response{ "" };

    bool disconnect{ false };

    QVariant pwd{ argType };
    if ( plr->getPwdRequested()
      && !plr->getEnteredPwd() )
    {
        if ( Helper::cmpServerPassword( pwd ) )
        {
            response = "Correct Server Password. Welcome!";

            plr->setPwdRequested( false );
            plr->setEnteredPwd( true );
        }
        else
        {
            response = "Incorrect Server Password. Please go away.";
            disconnect = true;
        }
    }
    else if ( !plr->getGotAuthPwd()
           || plr->getReqAuthPwd() )
    {
        if ( !pwd.toString().isEmpty()
             && AdminHelper::cmpRemoteAdminPwd( sernum, pwd ) )
        {
            response = "Correct Admin Password. Welcome!";

            plr->setReqAuthPwd( false );
            plr->setGotAuthPwd( true );
        }
        else
        {
            response = "Incorrect Admin Password. Please go away.";
            disconnect = true;
        }
    }

    if ( !response.isEmpty() )
    {

        server->sendMasterMessage( response, plr,
                                   false );
    }

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
    {

        server->sendMasterMessage( response, plr,
                                   false );
    }
}
