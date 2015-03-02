
#include "includes.hpp"
#include "remixwidget.hpp"
#include "ui_remixwidget.h"

//Initialize our accepted Commandline Argument List.
const QStringList ReMixWidget::cmdlArgs =
{
    QStringList() << "game" << "master" << "public"
                  << "listen" << "name" << "fudge"
};

ReMixWidget::ReMixWidget(QWidget* parent, Admin* adm, Messages* msg,
                         QStringList* argList) :
    QWidget(parent),
    ui(new Ui::ReMixWidget)
{
    ui->setupUi(this);

    //Setup our Random Device
    randDev = new RandDev();

    //Setup Objects.
    settings = new Settings( this );
    server = new ServerInfo();
    sysMessages = msg;
    admin = adm;

    plrWidget = new PlrListWidget( this, server, admin );
    ui->tmpWidget->setLayout( plrWidget->layout() );
    ui->tmpWidget->layout()->addWidget( plrWidget );

    server->setServerID( Settings::getServerID() );
    server->setHostInfo( QHostInfo() );
    server->setServerRules( Settings::getServerRules() );

    //Load Data from our CommandLine Args.
    if ( argList != nullptr )
        this->parseCMDLArgs( argList );

    ReMix::getSynRealData( server );

    //Create Timer Lambda to update our UI.
    this->initUIUpdate();

    //Setup Networking Objects.
    if ( tcpServer == nullptr )
        tcpServer = new Server( this, server, admin, plrWidget->getPlrModel() );

    defaultPalette = parent->palette();
}

ReMixWidget::~ReMixWidget()
{
    server->sendMasterInfo( true );

    settings->close();
    settings->deleteLater();

    tcpServer->close();
    tcpServer->deleteLater();

    plrWidget->deleteLater();

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
                    server->setLogUsage( true );
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
    ui->serverName->setText( server->getName() );
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
            msg = QString( "Listening for incoming calls to %1:%2" )
                      .arg( server->getPrivateIP() )
                      .arg( server->getPrivatePort() );

            if ( server->getIsPublic() )
            {
                if ( server->getSentUDPCheckin() )
                {
                    if ( server->getMasterUDPResponse() )
                    {
                        QString msg2 = QString( " ( Need port forward from "
                                                "%1:%2 )" )
                                           .arg( server->getPublicIP() )
                                           .arg( server->getPublicPort() );
                        msg.append( msg2 );
                    }
                    else
                        msg = { "Sent UDP check-in to Master. "
                                "Waiting for response..." };
                }
            }
            plrWidget->resizeColumns();
        }
        ui->networkStatus->setText( msg );
    });
}

void ReMixWidget::applyThemes(QString& name)
{
    QPalette palette;
    if ( name.compare( "dark", Qt::CaseInsensitive ) == 0 )
    {
        palette.setColor( QPalette::Window, QColor( 53,53,53 ) );
        palette.setColor( QPalette::WindowText, Qt::white );
        palette.setColor( QPalette::Base, QColor( 25,25,25 ) );
        palette.setColor( QPalette::AlternateBase,
                          QColor( 53,53,53 ) );
        palette.setColor( QPalette::ToolTipBase, Qt::white );
        palette.setColor( QPalette::ToolTipText, Qt::white );
        palette.setColor( QPalette::Text, Qt::white );
        palette.setColor( QPalette::Button, QColor( 53,53,53 ) );
        palette.setColor( QPalette::ButtonText, Qt::white );
        palette.setColor( QPalette::BrightText, Qt::red );
        palette.setColor( QPalette::Link, QColor( 42, 130, 218 ) );
        palette.setColor( QPalette::Highlight,
                          QColor( 42, 130, 218 ) );
        palette.setColor( QPalette::HighlightedText, Qt::black );
    }
    else if ( name.compare( "light", Qt::CaseInsensitive ) == 0 )
    {
        palette = defaultPalette;
    }

    qApp->setPalette( palette );
}

void ReMixWidget::on_enableNetworking_clicked()
{
    //Setup Networking Objects.
    if ( tcpServer == nullptr )
        tcpServer = new Server( this, server, admin, plrWidget->getPlrModel() );

    ui->enableNetworking->setEnabled( false );
    ui->serverPort->setEnabled( false );
    tcpServer->setupServerInfo();
}

void ReMixWidget::on_openRemoteAdmins_clicked()
{
    if ( admin->isVisible() )
        admin->hide();
    else
        admin->show();
}

void ReMixWidget::on_isPublicServer_stateChanged(int)
{
    //Setup Networking Objects.
    if ( tcpServer == nullptr )
        tcpServer = new Server( this, server, admin, plrWidget->getPlrModel() );

    if ( ui->isPublicServer->isChecked() )
        //Setup a connection with the Master Server.
        tcpServer->setupPublicServer( true );
    else   //Disconnect from the Master Server if applicable.
        tcpServer->setupPublicServer( false );
}

void ReMixWidget::on_openSettings_clicked()
{
    if ( settings->isVisible() )
        settings->hide();
    else
        settings->show();
}

void ReMixWidget::on_openSysMessages_clicked()
{
    if ( sysMessages->isVisible() )
        sysMessages->hide();
    else
        sysMessages->show();
}

void ReMixWidget::on_openBanDialog_clicked()
{
    admin->showBanDialog();
}

void ReMixWidget::on_openUserComments_clicked()
{
    tcpServer->showServerComments();
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

void ReMixWidget::on_serverName_textChanged(const QString &arg1)
{
    server->setName( arg1 );
}

void ReMixWidget::on_nightMode_clicked()
{
    QString theme{ "dark" };
    if ( nightMode )
    {
        theme = "light";
        ui->nightMode->setText( "Night Mode" );
    }
    else
        ui->nightMode->setText( "Normal Mode" );

    this->applyThemes( theme );
    nightMode = !nightMode;
}
