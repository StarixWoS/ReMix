
#include "includes.hpp"
#include "remixwidget.hpp"
#include "ui_remixwidget.h"

//Initialize our accepted Commandline Argument List.
const QStringList ReMixWidget::cmdlArgs =
{
    QStringList() << "game" << "master" << "public"
                  << "listen" << "name" << "fudge"
};

ReMixWidget::ReMixWidget(QWidget* parent, QStringList* argList, QString svrID) :
    QWidget(parent),
    ui(new Ui::ReMixWidget)
{
    ui->setupUi(this);
    serverID = svrID;

    //Setup our Random Device
    randDev = new RandDev();

    //Setup Objects.
    contextMenu = new QMenu( this );
    messages = new MessagesWidget( serverID );
    rules = new RulesWidget( serverID );

    settings = ReMix::getSettings();
    settings->addTabObjects( messages, rules, serverID );

    server = new ServerInfo( serverID );
    user = ReMix::getUser();

    plrWidget = new PlrListWidget( this, server, user );
    ui->tmpWidget->setLayout( plrWidget->layout() );
    ui->tmpWidget->layout()->addWidget( plrWidget );

    server->setServerID( Settings::getServerID( serverID ) );
    server->setHostInfo( QHostInfo() );

    //Load Data from our CommandLine Args.
    if ( argList != nullptr )
        this->parseCMDLArgs( argList );

    ReMix::getSynRealData( server );

    //Create Timer Lambda to update our UI.
    this->initUIUpdate();

    //Setup Networking Objects.
    if ( tcpServer == nullptr )
    {
        tcpServer = new Server( this, server, user, plrWidget->getPlrModel(),
                                serverID );
    }

    defaultPalette = parent->palette();
}

ReMixWidget::~ReMixWidget()
{
    server->sendMasterInfo( true );

    tcpServer->close();
    tcpServer->deleteLater();

    plrWidget->deleteLater();

    settings->remTabObjects( serverID );

    delete randDev;
    delete server;
    delete ui;
}

void ReMixWidget::sendServerMessage(QString msg, Player* plr, bool toAll)
{
    if ( server != nullptr )
        server->sendMasterMessage( msg, plr,toAll );
}

qint32 ReMixWidget::getPlayerCount()
{
    if ( server != nullptr )
        return server->getPlayerCount();

    return 0;
}

QString ReMixWidget::getServerName() const
{
    return server->getName();
}

Settings* ReMixWidget::getSettings() const
{
    return settings;
}

Server* ReMixWidget::getTcpServer() const
{
    return tcpServer;
}

QString& ReMixWidget::getServerID()
{
    return serverID;
}

void ReMixWidget::parseCMDLArgs(QStringList* argList)
{
    QString tmpArg{ "" };
    QString arg{ "" };
    QString tmp{ "" };

    int argIndex{ -1 };
    for ( int i = 0; i < argList->count(); ++i )
    {
        arg = argList->at( i );
        tmpArg.clear();
        tmp.clear();

        argIndex = -1;
        for ( int j = 0; j < cmdlArgs.count(); ++j )
        {
            if ( arg.contains( cmdlArgs.at( j ), Qt::CaseInsensitive ) )
            {
                tmpArg = cmdlArgs.at( j );
                argIndex = j;
                break;
            }
        }

        if ( !tmpArg.isEmpty() )
        {
            switch ( argIndex )
            {
                case CMDLArgs::GAME:
                    tmp = Helper::getStrStr( arg, tmpArg, "=", "" );
                    if ( !tmp.isEmpty() )
                        server->setGameName( tmp );
                break;
                case CMDLArgs::MASTER:
                    tmp = Helper::getStrStr( arg, tmpArg, "=", "" );
                    if ( !tmp.isEmpty() )
                        server->setMasterInfoHost( tmp );
                break;
                case CMDLArgs::PUBLIC:
                    tmp = Helper::getStrStr( arg, tmpArg, "=", "" );
                    if ( !tmp.isEmpty() )
                    {
                        server->setMasterIP( tmp.left( tmp.indexOf( ':' ) ) );
                        server->setMasterPort(
                                    static_cast<quint16>(
                                        tmp.mid( tmp.indexOf( ':' ) + 1 )
                                           .toInt() ) );
                    }
                    ui->isPublicServer->setChecked( true );
                break;
                case CMDLArgs::LISTEN:
                    tmp = Helper::getStrStr( arg, tmpArg, "=", "" );
                    if ( !tmp.isEmpty() )
                        server->setPrivatePort( tmp.toInt() );

                    emit ui->enableNetworking->clicked();
                break;
                case CMDLArgs::NAME:
                    tmp = Helper::getStrStr( arg, tmpArg, "=", "" );
                    if ( !tmp.isEmpty() )
                        server->setName( tmp );
                break;
                case CMDLArgs::FUDGE:
                    server->setLogFiles( true );
                break;
                case CMDLArgs::RELOAD:
                    emit reloadOldServersSignal();
                break;
                default:
                    qDebug() << "Unknown Command Line Argument: " << tmp;
                break;
            }
        }
    }

    ui->serverPort->setText(
                Helper::intToStr(
                    server->getPrivatePort(), 10 ) );

    ui->isPublicServer->setChecked( server->getIsPublic() );
}

void ReMixWidget::initUIUpdate()
{
    //Create and Connect Lamda Objects
    QObject::connect( server->getUpTimer(), &QTimer::timeout, [=]()
    {
        quint64 time = server->getUpTime();

        QString time_s = QString( "%1:%2:%3" )
                             .arg( time / 3600, 2, 10, QChar( '0' ) )
                             .arg(( time / 60 ) % 60, 2, 10, QChar( '0' ) )
                             .arg( time % 60, 2, 10, QChar( '0' ) );

        ui->onlineTime->setText( time_s );
        ui->callCount->setText(
                    QString( "#Calls: %1" )
                        .arg( server->getUserCalls() ) );

        ui->pingCount->setText(
                    QString( "#Pings: %1" )
                        .arg( server->getUserPings() ) );

        ui->packetDCCount->setText(
                    QString( "#Pkt-DC: %1" )
                        .arg( server->getPktDc() ) );

        ui->dupDCCount->setText(
                    QString( "#Dup-DC: %1" )
                        .arg( server->getDupDc() ) );

        ui->ipDCCount->setText(
                    QString( "#IP-DC: %1" )
                        .arg( server->getIpDc() ) );

        ui->packetINBD->setText(
                    QString( "#IN: %1 Bd" )
                        .arg( server->getBaudIn() ) );

        ui->packetOUTBD->setText(
                    QString( "#OUT: %1 Bd" )
                        .arg( server->getBaudOut() ) );

        //Update other Info as well.
        QString msg{ "Select Port Number and press \"Accept Calls\" "
                     "when ready!" };
        if ( server->getIsSetUp() )
        {
            msg = QString( "Listening for incoming calls "
                           "to: <a href=\"%1\"><span style=\" text-decoration: "
                           "underline; color:#007af4;\">%1:%2</span></a>" )
                      .arg( server->getPrivateIP() )
                      .arg( server->getPrivatePort() );

            if ( server->getIsPublic() )
            {
                if ( server->getSentUDPCheckin() )
                {
                    if ( server->getMasterUDPResponse() )
                    {
                        QString msg2 = QString( " ( Port forward from: %1:%2 ) "
                                                "( Ping: %3 ms, "
                                                "Avg: %4 ms, "
                                                "Trend: %5 ms )" )
                                           .arg( server->getPublicIP() )
                                           .arg( server->getPublicPort() )
                                           .arg( server->getMasterPing() )
                                           .arg( server->getMasterPingAvg() )
                                           .arg( server->getMasterPingTrend() );
                        msg.append( msg2 );
                    }
                    else
                    {
                        msg = { "Sent UDP check-in to Master. "
                                "Waiting for response..." };

                        if ( server->getMasterTimedOut() )
                        {
                            msg = "No UDP response received from master server."
                                  " Perhaps we are behind a UDP-blocking"
                                  " firewall?";
                        }
                    }
                }
            }
            //Validate the server's IP Address is still valid.
            //If it is now invalid, restart the network sockets.
            if ( Settings::getIsInvalidIPAddress( server->getPrivateIP() ) )
            {
                emit this->reValidateServerIP();
            }
            plrWidget->resizeColumns();
        }
        ui->networkStatus->setText( msg );
    });
}

void ReMixWidget::on_enableNetworking_clicked()
{
    //Setup Networking Objects.
    if ( tcpServer == nullptr )
    {
        tcpServer = new Server( this, server, user, plrWidget->getPlrModel(),
                                serverID );
    }

    ui->enableNetworking->setEnabled( false );
    ui->serverPort->setEnabled( false );
    tcpServer->setupServerInfo();
}

void ReMixWidget::on_openUserInfo_clicked()
{
    if ( user->isVisible() )
        user->hide();
    else
        user->show();
}

void ReMixWidget::on_openSettings_clicked()
{
    if ( settings->isVisible() )
        settings->hide();
    else
        settings->show();
}

void ReMixWidget::on_openUserComments_clicked()
{
    Comments* comments{ tcpServer->getServerComments() };
    if ( comments != nullptr )
    {
        if ( comments->isVisible() )
            comments->hide();
        else
            comments->show();
    }
}

void ReMixWidget::on_serverPort_textChanged(const QString &arg1)
{
    qint32 val = arg1.toInt();
    if ( val < 0 || val > 65535 )
        val = 0;

    //Generate a Valid Port Number.
    if ( val == 0 )
        val = randDev->genRandNum( 10000, 65535 );

    server->setPrivatePort( val );
}

void ReMixWidget::on_isPublicServer_toggled(bool)
{
    //Setup Networking Objects.
    if ( tcpServer == nullptr )
    {
        tcpServer = new Server( this, server, user, plrWidget->getPlrModel(),
                                serverID );
    }

    if ( ui->isPublicServer->isChecked() )
        //Setup a connection with the Master Server.
        tcpServer->setupPublicServer( true );
    else   //Disconnect from the Master Server if applicable.
        tcpServer->setupPublicServer( false );
}

void ReMixWidget::on_useUPNP_toggled(bool)
{
    //Tell the server to use a UPNP Port Forward.
    if ( ui->useUPNP->isChecked() )
        tcpServer->setupUPNPForward();
    else   //Remove the UPNP Port Forward if applicable.
        tcpServer->removeUPNPForward();
}

void ReMixWidget::on_networkStatus_linkActivated(const QString &link)
{
    QString title = QString( "Invalid IP:" );
    QString prompt = QString( "Do you wish to mark the IP Address [ %1 ] as "
                              "invalid and refresh the network interface?" );

    prompt = prompt.arg( link );
    if ( Helper::confirmAction( this, title, prompt ) )
    {
        Settings::setIsInvalidIPAddress( link );
        emit this->reValidateServerIP();

        title = "Note:";
        prompt = "Please refresh your server list in-game!";
        Helper::confirmAction( this, title, prompt );
    }
}

void ReMixWidget::on_networkStatus_customContextMenuRequested(const QPoint &pos)
{
    if ( contextMenu == nullptr )
    {
        contextMenu = new QMenu( this );
        if ( contextMenu != nullptr )
        {
            contextMenu->addMenu( "Test 1" );

            contextMenu->addMenu( "Test 2" );
        }
    }
    qDebug() << pos;
    contextMenu->popup( ui->networkStatus->mapToGlobal( pos ) );
}
