
#include "includes.hpp"
#include "remixwidget.hpp"
#include "ui_remixwidget.h"

ReMixWidget::ReMixWidget(QWidget* parent, ServerInfo* svrInfo) :
    QWidget(parent),
    ui(new Ui::ReMixWidget)
{
    ui->setupUi( this );

    server = svrInfo;
    serverID = server->getName();

    //Setup our Random Device
    randDev = new RandDev();

    //Setup Objects.
    motdWidget = new MOTDWidget( serverID );
    rules = new RulesWidget( serverID );

    settings = ReMix::getSettings();
    settings->addTabObjects( motdWidget, rules, serverID );

    user = ReMix::getUser();

    plrWidget = new PlrListWidget( this, server, user );
    ui->tmpWidget->setLayout( plrWidget->layout() );
    ui->tmpWidget->layout()->addWidget( plrWidget );

    //Setup Networking Objects.
    tcpServer = new Server( this, server, user, plrWidget->getPlrModel(),
                            serverID );
    server->setTcpServer( tcpServer );

    ui->isPublicServer->setChecked( server->getIsPublic() );

    //Create Timer Lambda to update our UI.
    this->initUIUpdate();
    defaultPalette = parent->palette();
}

ReMixWidget::~ReMixWidget()
{
    server->sendMasterInfo( true );

    if ( ui->isPublicServer->isChecked() )
        server->setupUPNP( true );

    tcpServer->close();
    tcpServer->deleteLater();

    plrWidget->deleteLater();

    settings->remTabObjects( serverID );

    delete randDev;
    delete server;
    delete ui;
}

void ReMixWidget::sendServerMessage(QString msg)
{
    if ( server != nullptr )
    {
        server->sendMasterMessage( msg, nullptr, true );
    }
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

quint16 ReMixWidget::getPrivatePort() const
{
    if ( server == nullptr )
        return 0;

    return server->getPrivatePort();
}

void ReMixWidget::initUIUpdate()
{
    //Create and Connect Lamda Objects
    QObject::connect( server->getUpTimer(), &QTimer::timeout,
                      server->getUpTimer(),
    [=]()
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
        QString msg{ "Toggle \"Public Servers\" when ready!" };
        if ( server->getIsPublic() )
        {
            msg = QString( "Listening for incoming calls "
                           "to: <a href=\"%1\"><span style=\" text-decoration: "
                           "underline; color:#007af4;\">%1:%2</span></a>" )
                      .arg( server->getPrivateIP() )
                      .arg( server->getPrivatePort() );

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

void ReMixWidget::on_openChatView_clicked()
{
    ChatView* viewer{ tcpServer->getChatView() };
    if ( viewer != nullptr )
    {
        if ( viewer->isVisible() )
            viewer->hide();
        else
            viewer->show();
    }
}

void ReMixWidget::on_isPublicServer_toggled(bool value)
{
    //Prevent the Server class from re-initializing the ServerInfo.
    if ( value != server->getIsPublic() )
        server->setIsPublic( ui->isPublicServer->isChecked() );
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
    }

    if ( contextMenu != nullptr )
    {
        //Populate ContextMenu with IP Addresses which were manually
        //blacklisted and removed from selection for use as the Private IP
        //For a more user-friendly method of removing them from the preferences.
    }
    //contextMenu->popup( this->mapToGlobal( pos ) );
}
