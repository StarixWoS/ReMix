
//Class includes.
#include "remixwidget.hpp"
#include "ui_remixwidget.h"

//Required ReMix Widget includes.
#include "widgets/plrlistwidget.hpp"
#include "widgets/ruleswidget.hpp"
#include "widgets/motdwidget.hpp"

//ReMix includes.
#include "serverinfo.hpp"
#include "chatview.hpp"
#include "comments.hpp"
#include "settings.hpp"
#include "randdev.hpp"
#include "server.hpp"
#include "helper.hpp"
#include "user.hpp"

//Qt Includes.
#include <QMenu>

ReMixWidget::ReMixWidget(QWidget* parent, ServerInfo* svrInfo) :
    QWidget(parent),
    ui(new Ui::ReMixWidget)
{
    ui->setupUi( this );

    server = svrInfo;

    //Setup our Random Device
    randDev = new RandDev();

    //Setup Objects.
    motdWidget = new MOTDWidget();
    motdWidget->setServerName( server->getName() );

    rules = new RulesWidget();
    rules->setServerName( server->getName() );

    Settings::addTabObjects( motdWidget, rules, server );

    plrWidget = new PlrListWidget( this, server );
    ui->tmpWidget->setLayout( plrWidget->layout() );
    ui->tmpWidget->layout()->addWidget( plrWidget );

    //Setup Networking Objects.
    tcpServer = new Server( this, server, plrWidget->getPlrModel() );
    server->setTcpServer( tcpServer );

    //Initialize the TCP Server if we're starting as a public instance.
    if ( server->getIsPublic() )
        tcpServer->setupServerInfo();

    ui->isPublicServer->setChecked( server->getIsPublic() );
    ui->useUPNP->setChecked( server->getUseUPNP() );

    //Create Timer Lambda to update our UI.
    this->initUIUpdate();
    defaultPalette = parent->palette();
}

ReMixWidget::~ReMixWidget()
{
    server->sendMasterInfo( true );

    if ( ui->useUPNP->isChecked() )
        server->setupUPNP( false );

    tcpServer->close();
    tcpServer->deleteLater();

    plrWidget->deleteLater();

    Settings::remTabObjects( server );

    delete randDev;
    delete server;
    delete ui;
}

ServerInfo* ReMixWidget::getServerInfo() const
{
    return server;
}

void ReMixWidget::renameServer(const QString& newName)
{
    if ( !newName.isEmpty() )
    {
        Settings::copyServerSettings( server, newName );
        motdWidget->setServerName( newName );
        rules->setServerName( newName );

        server->setName( newName );
    }
}

void ReMixWidget::sendServerMessage(const QString& msg)
{
    if ( server != nullptr )
        server->sendMasterMessage( msg, nullptr, true );
}

qint32 ReMixWidget::getPlayerCount() const
{
    if ( server != nullptr )
        return server->getPlayerCount();

    return 0;
}

QString ReMixWidget::getServerName() const
{
    return server->getName();
}

Server* ReMixWidget::getTcpServer() const
{
    return tcpServer;
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
                    QString msg2{ " ( Port forward from: %1:%2 ) "
                                            "( Ping: %3 ms, "
                                            "Avg: %4 ms, "
                                            "Trend: %5 ms )" };
                            msg2 = msg2.arg( server->getPublicIP(),
                                             QString::number(
                                                 server->getPublicPort() ),
                                             QString::number(
                                                 server->getMasterPing() ),
                                             QString::number(
                                                 server->getMasterPingAvg() ),
                                             QString::number(
                                                 server->getMasterPingTrend() ));
                    msg.append( msg2 );

                    qint32 fails{ server->getMasterPingFailCount() };
                    if ( fails >= 1 )
                    {
                        QString msg3{ " ( Dropped: %1 ) " };
                                msg3 = msg3.arg( fails );

                        msg.append( msg3 );
                    }
                }
                else
                {
                    msg = "Sent UDP check-in to Master. "
                          "Waiting for response...";

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
    User* user = User::getInstance();
    if ( user != nullptr )
    {
        if ( user->isVisible() )
            user->hide();
        else
            user->show();
    }
}

void ReMixWidget::on_openSettings_clicked()
{
    Settings* settings = Settings::getInstance();
    if ( settings->isVisible() )
        settings->hide();
    else
        settings->show();
}

void ReMixWidget::on_openUserComments_clicked()
{
    if ( tcpServer != nullptr )
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
    else
    {
        QString title{ "Error:" };
        QString message{ "Unable to fetch the Server's Comment dialog!" };
        Helper::warningMessage( this, title, message );
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

void ReMixWidget::on_networkStatus_linkActivated(const QString& link)
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

void ReMixWidget::on_useUPNP_toggled(bool value)
{
    if ( value != server->getUseUPNP() )
        server->setUseUPNP( ui->useUPNP->isChecked() );
}

void ReMixWidget::on_networkStatus_customContextMenuRequested(const QPoint&)
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
