
//Class includes.
#include "createinstance.hpp"
#include "ui_createinstance.h"

//Required ReMix Widget includes.
#include "widgets/remixtabwidget.hpp"

//ReMix includes.
#include "thread/mastermixthread.hpp"
#include "serverinfo.hpp"
#include "settings.hpp"
#include "randdev.hpp"
#include "logger.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QMessageBox>
#include <QCloseEvent>
#include <QTcpSocket>
#include <QSettings>

//Initialize our accepted Command List.
const QStringList CreateInstance::gameNames =
{
    "WOS",
    "TOY",
    "W97"
};

//Storage for restarting improperly shut-down servers.
QStringList CreateInstance::restartServerList;

//Storage For the Dialog's instance.
CreateInstance* CreateInstance::instance{ nullptr };

CreateInstance::CreateInstance(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::CreateInstance)
{
    ui->setupUi(this);
    collator.setNumericMode( true );
    collator.setCaseSensitivity( Qt::CaseInsensitive );
}

CreateInstance::~CreateInstance()
{
    this->disconnect();
    this->deleteLater();

    if ( ui != nullptr )
        delete ui;
}

CreateInstance* CreateInstance::getInstance(QWidget* parent)
{
    if ( instance == nullptr )
    {
        if ( parent != nullptr )
            instance = new CreateInstance( parent );
        else
            instance = new CreateInstance();
    }
    return instance;
}

void CreateInstance::updateServerList(const bool& firstRun)
{
    QMutexLocker locker( &mutex );

    if ( !firstRun ) //Remove any servers previously flagged as auto restart.
        restartServerList.clear();

    ui->servers->clear();
    ui->servers->addItem( "" );  //First item will always be blank.

    QStringList servers{ Settings::prefs->childGroups() };
    QStringList validServers;

    QString name{ "" };
    bool skip{ false };

    bool running{ false };
    quint32 serverCount{ 0 };
    for ( int i = 0; i < servers.count(); ++i )
    {
        name = servers.at( i );
        for ( const QString& key : Settings::pKeys )
        {
            if ( Helper::cmpStrings( name, key ) )
                skip = true;
        }

        if ( !skip )
        {
            running = Settings::getSetting( SKeys::Setting, SSubKeys::IsRunning, name ).toBool();
            if ( firstRun )
            {
                Settings::setSetting( false, SKeys::Setting, SSubKeys::IsRunning, name );
                if ( running ) //Initial Startup of ReMix, and a server is found as being 'online'.
                    restartServerList.append( name );  //Store Server Name.
            }
        }

        if ( !skip && !running )
        {
            validServers.append( name );
            ++serverCount;
        }

        skip = false;
        running = false;
    }

    if ( serverCount != 0 )
    {
        //Sort the server list before inserting them into the dropdown menu.
        std::sort( validServers.begin(), validServers.end(),
        [=](const QString &str1, const QString &str2)
        {
            return ( collator.compare( str1, str2 ) < 0 );
        });
        ui->servers->addItems( validServers );
        ui->servers->setEnabled( true );
    }
    else
        ui->servers->setEnabled( true );

    if ( firstRun )
    {
        if ( !restartServerList.isEmpty() )
        {
            this->setLoadingOldServers( true );
            emit this->restartServerListSignal( restartServerList );
        }
        else
            this->setLoadingOldServers( false );
    }
}

void CreateInstance::on_initializeServer_clicked()
{
    ServerInfo* server{ nullptr };
    QString svrName{ ui->servers->currentText() };
    QString title{ "Error:" };
    QString message{ "Servers cannot be initialized without a name!" };
    if ( !svrName.isEmpty() )
    {
        if ( Helper::strContainsStr( svrName, "world=" ) )
        {
            message = "Servers cannot be initialized with the World selection within the name. Please try again.";
            Helper::warningMessage( this, title, message );
        }
        else
        {
            //Verify that the server hasn't been initialized previously.
            if ( !Settings::getSetting( SKeys::Setting, SSubKeys::IsRunning, svrName ).toBool() )
            {
                server = this->initializeServer( svrName, gameNames[ ui->gameName->currentIndex() ], ui->portNumber->text( ).toUShort(),
                                                 ui->useUPNP->isChecked(), ui->isPublic->isChecked() );

                emit this->createServerAcceptedSignal( server );
                emit this->accept();
            }
            else //Warn the Server Host.
            {
                title = "Unable to Initialize Server:";
                message = "You are unable to initialize two servers with the same name!";

                Helper::warningMessage( this, title, message );
            }
        }
    }
    else
    {
        Helper::warningMessage( this, title, message );
    }
}

void CreateInstance::on_close_clicked()
{
    this->close();
}

quint16 CreateInstance::genPort()
{
    quint16 portMax{ std::numeric_limits<quint16>::max() };
    quint16 portMin{ std::numeric_limits<quint16>::min() };
    quint16 port{ 0 };

    RandDev* randDev{ RandDev::getDevice() };
    if ( randDev == nullptr )
        return 8888;    //Return Arbitrary number if Generator is invalid.

    //Generate a Valid Port Number.
    if ( port == portMin )
    {
        if ( randDev != nullptr )
        {
            do
            {   //Generate a Possibly Usable Port Number.
                port = randDev->genRandNum( quint16( 10000 ), portMax );
            }   //Test our Generated Port Number for usability.
            while ( !this->testPort( port ) );
        }
    }
    return port;
}

void CreateInstance::restartServer(const QString& name, const QString& gameName, const quint16& port, const bool& useUPNP, const bool& isPublic)
{
    ServerInfo* server{ nullptr };
    if ( !name.isEmpty() )
    {
        //Verify that the server hasn't been initialized previously.
        if ( !Settings::getSetting( SKeys::Setting, SSubKeys::IsRunning, name ).toBool() )
        {
            server = this->initializeServer( name, gameName, port, useUPNP, isPublic );

            emit this->createServerAcceptedSignal( server );
            emit this->accept();
        }
        else //Warn the Server Host.
        {
            QString title{ "Unable to Initialize Server:" };
            QString message{ "You are unable to initialize two servers with the same name!" };

            Helper::warningMessage( this, title, message );
        }
    }
}

ServerInfo* CreateInstance::loadOldServer(const QString& name)
{
    ServerInfo* server{ nullptr };
    if ( !name.isEmpty() )
    {
        QString gameName{ Settings::getSetting( SKeys::Setting, SSubKeys::GameName, name ).toString() };
        QString svrPort{ Settings::getSetting( SKeys::Setting, SSubKeys::PortNumber, name ).toString() };
        bool isPublic{ Settings::getSetting( SKeys::Setting, SSubKeys::IsPublic, name ).toBool() };
        bool useUPNP{ Settings::getSetting( SKeys::Setting, SSubKeys::UseUPNP, name ).toBool() };

        server = this->initializeServer( name, gameName, svrPort.toUShort(), useUPNP, isPublic );
        return server;
    }
    return nullptr;
}

ServerInfo* CreateInstance::initializeServer(const QString& name, const QString& gameName, const quint16& port, const bool& useUPNP, const bool& isPublic)
{
    ServerInfo* server{ nullptr };

    //Verify Instance Count isn't above the maximum.
    if ( ReMixTabWidget::getInstanceCount() + 1 <= MAX_SERVER_COUNT )
    {
        server = new ServerInfo();
        if ( server == nullptr )
            return nullptr;

        server->setServerName( name );
        server->setGameName( gameName );

        QString overrideIP{ Settings::getSetting( SKeys::Setting, SSubKeys::OverrideMasterIP ).toString() };
        if ( !overrideIP.isEmpty() )
        {
            int index{ overrideIP.indexOf( ":" ) };
            if ( index > 0 )
            {
                server->setMasterIP( overrideIP.left( index ), static_cast<quint16>( overrideIP.midRef( index + 1 ).toInt() ) );
                QString msg{ "Loaded Master Server Override [ %1:%2 ]." };
                        msg = msg.arg( server->getMasterIP() )
                                 .arg( server->getMasterPort() );
                Logger::getInstance()->insertLog( server->getServerName(), msg, LogTypes::MASTERMIX, true, true );
            }
        }
        else
        {
            MasterMixThread::getInstance()->getMasterMixInfo( server );
            //Helper::getSynRealData( server );
        }

        server->setPrivatePort( port );
        server->setServerID( Settings::getServerID( name ) );
        server->setUseUPNP( useUPNP );
        server->setIsPublic( isPublic );
    }
    else
    {
        QString title{ "Unable to Initialize Server:" };
        QString message{ "You have reached the limit of [ %1 ] concurrently running Server Instances.!" };
        message = message.arg( static_cast<int>( MAX_SERVER_COUNT ) );

        Helper::warningMessage( this, title, message );
    }
    return server;
}

bool CreateInstance::getLoadingOldServers() const
{
    return loadingOldServers;
}

void CreateInstance::setLoadingOldServers(bool value)
{
    loadingOldServers = value;
}

bool CreateInstance::testPort(const quint16& port)
{
    //Get the best possible PrivateIP.
    QHostAddress addr{ Helper::getPrivateIP() };

    //Check if the Port can be listened to via a TCP Socket.
    QTcpSocket* socket{ new QTcpSocket( this ) };

    bool canListen{ false };
    if ( socket != nullptr )
    {
        canListen = socket->bind( addr, port );

        socket->close();
        socket->deleteLater();
    }
    return canListen;
}

void CreateInstance::closeEvent(QCloseEvent* event)
{
    if ( event == nullptr )
        return;

    if ( event->type() == QEvent::Close )
    {
        QString title{ "Close ReMix:" };
        QString prompt{ "You are about to shut down your ReMix game server!\r\n\r\nAre you certain?" };

        if ( ReMixTabWidget::getInstanceCount() == 0 )
        {
            if ( Helper::confirmAction( this, title, prompt ) )
            {
                event->accept();
                qApp->quit();
            }
            else
                event->ignore();
        }
    }
}

void CreateInstance::showEvent(QShowEvent* event)
{
    if ( event == nullptr )
        return;

    if ( event->type() == QEvent::Show )
    {
        ui->gameName->setCurrentIndex( 0 );
        ui->portNumber->setText( Helper::intToStr( this->genPort() ) );
        ui->isPublic->setChecked( false );
        this->updateServerList( false );
    }
    event->accept();
}

void CreateInstance::on_servers_currentIndexChanged(int)
{
    QString svrName{ ui->servers->currentText() };
    if ( !svrName.isEmpty() )
    {
        QString gameName{ Settings::getSetting( SKeys::Setting, SSubKeys::GameName, svrName ).toString() };
        if ( !gameName.isEmpty() )
        {
            bool notFound{ true };
            for ( const QString& el : gameNames )
            {
                if ( Helper::cmpStrings( el, gameName ) )
                {
                    ui->gameName->setCurrentIndex( gameNames.indexOf( el ) );
                    notFound = false;
                }
            }

            if ( notFound )
                ui->gameName->setCurrentIndex( 0 );
        }
    }

    QString svrPort{ Settings::getSetting( SKeys::Setting, SSubKeys::PortNumber, svrName ).toString() };
    if ( !svrPort.isEmpty() )
        ui->portNumber->setText( svrPort );
    else
        ui->portNumber->setText( Helper::intToStr( this->genPort() ) );

    bool isPublic{ Settings::getSetting( SKeys::Setting, SSubKeys::IsPublic, svrName ).toBool() };
    if ( isPublic )
        ui->isPublic->setChecked( isPublic );
    else
        ui->isPublic->setChecked( false );

    bool useUPNP{ Settings::getSetting( SKeys::Setting, SSubKeys::UseUPNP, svrName ).toBool() };
    if ( useUPNP )
        ui->useUPNP->setChecked( useUPNP );
    else
        ui->useUPNP->setChecked( false );
}

void CreateInstance::on_portNumber_textChanged(const QString& arg1)
{
    //Reduce the User Inputted Port Number to within proper bounds.
    quint16 port{ arg1.toUShort() };
    bool generated{ false };

    quint16 portMax{ std::numeric_limits<quint16>::max() };
    quint16 portMin{ std::numeric_limits<quint16>::min() };

    if ( ( port > portMin )
      && ( port > portMax ) )
    {
        port = portMax - 1;
    }

    if ( port == portMin
      || !this->testPort( port ) )
    {
        port = this->genPort();
        generated = true;
    }


    if ( ( !Helper::cmpStrings( arg1, "0" )
      && !Helper::cmpStrings( arg1, "" ) )
       || generated )
    {
        ui->portNumber->setText( Helper::intToStr( port ) );
    }
}

void CreateInstance::on_servers_currentTextChanged(const QString& arg1)
{
    int index{ ui->servers->findText( arg1 ) };
    if ( index >= 0 )
        ui->servers->setCurrentIndex( index );
}

void CreateInstance::on_randomizePort_clicked()
{
    //Let "on_portNumber_textChanged" handle the port generation.
    ui->portNumber->setText( "" );
}

void CreateInstance::on_deleteServer_clicked()
{
    QString svrName{ ui->servers->currentText() };
    QString title{ "Warning:" };
    QString message{ "Please make sure that you would like the server [ %1 ] to be deleted from storage!" };
    if ( !svrName.isEmpty() )
    {
        message = message.arg( svrName );
        if ( Helper::confirmAction( this, title, message ) )
        {
            Settings::removeSetting( svrName );
            this->updateServerList( false );
        }
    }
}
