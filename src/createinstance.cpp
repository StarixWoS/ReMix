
//Class includes.
#include "createinstance.hpp"
#include "ui_createinstance.h"

//Required ReMix Widget includes.
#include "widgets/remixtabwidget.hpp"

//ReMix includes.
#include "thread/mastermixthread.hpp"
#include "settings.hpp"
#include "randdev.hpp"
#include "logger.hpp"
#include "helper.hpp"
#include "server.hpp"

//Qt Includes.
#include <QNetworkInterface>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTcpSocket>
#include <QSettings>

//Initialize our accepted Games List.
const QMap<Games, QString> CreateInstance::gameNames =
{
    { Games::WoS, "WoS" },
    { Games::ToY, "TOY" },
    { Games::W97, "W97" },
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

    QObject::connect( this, &CreateInstance::getMasterMixInfoSignal,
                      MasterMixThread::getInstance(), &MasterMixThread::getMasterMixInfoSlot, Qt::UniqueConnection );

    //Load the Network Interface List.
    QList<QHostAddress> ipList{ QNetworkInterface::allAddresses() };
    QStringList items;

    //Default to our localhost address if nothing valid is found.
    QHostAddress ipAddress{ QHostAddress::Null };
    for ( const QHostAddress& ip : ipList )
    {
        items.append( ip.toString() );
    }

    ui->netInterface->addItems( items );
    ui->netInterface->setCurrentIndex( 0 );
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
    QMutexLocker<QMutex> locker( &mutex );

    if ( !firstRun ) //Remove any servers previously flagged as auto restart.
        restartServerList.clear();

    ui->servers->clear();
    ui->servers->addItem( "" );  //First item will always be blank.

    QStringList servers{ Settings::prefs->childGroups() };
    QStringList validServers;

    bool isRunning{ false };
    bool restart{ false };

    for ( const QString& name : servers )
    {
        const bool skip =
        {
            std::any_of( Settings::pKeys.cbegin(), Settings::pKeys.cend(),
            [name]( const QString& key )
            {
                return Helper::cmpStrings( name, key );
            } )
        };

        if ( !skip )
        {
            isRunning = Settings::getSetting( SKeys::Setting, SSubKeys::IsRunning, name ).toBool();
            restart = Settings::getSetting( SKeys::Rules, SSubKeys::AutoRestart, name ).toBool();

            if ( firstRun )
            {
                Settings::setSetting( false, SKeys::Setting, SSubKeys::IsRunning, name );
                if ( restart ) //Initial Startup of ReMix, and a server is found as being 'online'.
                    restartServerList.append( name );  //Store Server Name.
            }
        }

        if ( !skip
          && ( ( restart && !isRunning )
            || ( !restart && !isRunning ) ) )
        {
            validServers.append( name );
        }
        restart = false;
    }

    if ( validServers.count() != 0 )
    {
        //Sort the server list before inserting them into the dropdown menu.
        std::sort( validServers.begin(), validServers.end(),
        [&](const QString& str1, const QString& str2)
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
                const QString game{ gameNames.value( static_cast<Games>( ui->gameName->currentIndex() ), "WoS" ) };
                emit this->createServerAcceptedSignal(
                            this->initializeServer( svrName, game, ui->netInterface->currentText(),
                                                    ui->portNumber->text( ).toUShort(), ui->useUPNP->isChecked(), ui->isPublic->isChecked() ) );
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
    quint16 portMin{ std::numeric_limits<quint16>::min() };
    quint16 port{ 0 };

    //Generate a Valid Port Number.
    if ( port == portMin )
    {
        do
        {   //Generate a Possibly Usable Port Number.
            const quint16 portMax{ std::numeric_limits<quint16>::max() };
            port = RandDev::getInstance().getGen(quint16( 10000 ), portMax );
        }   //Test our Generated Port Number for usability.
        while ( !this->testPort( port ) );
    }
    return port;
}

void CreateInstance::restartServer(const QString& name, const QString& gameName, const QString& netInterface, const quint16& port,
                                   const bool& useUPNP, const bool& isPublic)
{
    if ( !name.isEmpty() )
    {
        //Verify that the server hasn't been initialized previously.
        if ( !Settings::getSetting( SKeys::Setting, SSubKeys::IsRunning, name ).toBool() )
        {
            emit this->createServerAcceptedSignal( this->initializeServer( name, gameName, netInterface, port, useUPNP, isPublic ) );
            emit this->accept();
        }
        else //Warn the Server Host.
        {
            static const QString title{ "Unable to Initialize Server:" };
            static const QString message{ "You are unable to initialize two servers with the same name!" };

            Helper::warningMessage( this, title, message );
        }
    }
}

QSharedPointer<Server> CreateInstance::loadOldServer(const QString& name)
{
    if ( !name.isEmpty() )
    {
        QString netInterface{ Settings::getSetting( SKeys::Setting, SSubKeys::NetInterface, name ).toString() };
        QString gameName{ Settings::getSetting( SKeys::Setting, SSubKeys::GameName, name ).toString() };
        QString svrPort{ Settings::getSetting( SKeys::Setting, SSubKeys::PortNumber, name ).toString() };
        bool isPublic{ Settings::getSetting( SKeys::Setting, SSubKeys::IsPublic, name ).toBool() };
        bool useUPNP{ Settings::getSetting( SKeys::Setting, SSubKeys::UseUPNP, name ).toBool() };

        return this->initializeServer( name, gameName, netInterface, svrPort.toUShort(), useUPNP, isPublic );
    }
    return nullptr;
}

QSharedPointer<Server> CreateInstance::initializeServer(const QString& name, const QString& gameName, const QString& netInterface, const quint16& port,
                                                        const bool& useUPNP, const bool& isPublic)
{
    //Verify Instance Count isn't above the maximum.
    QSharedPointer<Server> server{ nullptr };
    if ( ReMixTabWidget::getInstanceCount() + 1 <= *Globals::MAX_SERVER_COUNT )
    {
        Games gameID{ gameNames.key( gameName, Games::WoS ) };

        server = QSharedPointer<Server>( new Server( nullptr ), Server::customDeconstruct );
        server->setServerName( name );
        server->setGameName( gameName );
        server->setGameId( gameID );
        server->setPrivatePort( port );
        server->setServerID( Settings::getServerID( name ) );
        server->setUseUPNP( useUPNP );

        if ( !netInterface.isEmpty() )
            server->setIsPublic( isPublic, netInterface );
        else
            server->setIsPublic( isPublic, Helper::getPrivateIP().toString() );

        emit this->getMasterMixInfoSignal( server );
    }
    else
    {
        static const QString title{ "Unable to Initialize Server:" };
        QString message{ "You have reached the limit of [ %1 ] concurrently running Server Instances.!" };
                message = message.arg( *Globals::MAX_SERVER_COUNT );

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
        static const QString title{ "Close ReMix:" };
        static const QString prompt{ "You are about to shut down your ReMix game server!\r\n\r\nAre you certain?" };

        if ( ReMixTabWidget::getInstanceCount() == 0 )
        {
            if ( Helper::confirmAction( this, title, prompt ) )
            {
                event->accept();
                emit this->quitSignal();
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
        this->updateServerList( false );

        ui->gameName->setCurrentIndex( 0 );
        ui->portNumber->setText( Helper::intToStr( this->genPort() ) );
        ui->isPublic->setChecked( false );
    }
    event->accept();
}

void CreateInstance::on_servers_currentIndexChanged(int)
{
    const QString serverName{ ui->servers->currentText() };

    //If the serverName is empty, reset the UI, and return early.
    if ( serverName.isEmpty() )
    {
        ui->gameName->setCurrentIndex( 0 );
        ui->netInterface->setCurrentIndex( 0 );
        ui->portNumber->setText( Helper::intToStr( this->genPort() ) );
        ui->isPublic->setChecked( false );
        ui->useUPNP->setChecked( false );
        return;
    }

    const QString netInterface{ Settings::getSetting( SKeys::Setting, SSubKeys::NetInterface, serverName ).toString() };
    const QString svrPort{ Settings::getSetting( SKeys::Setting, SSubKeys::PortNumber, serverName ).toString() };
    const QString gameName{ Settings::getSetting( SKeys::Setting, SSubKeys::GameName, serverName ).toString() };
    const qint32 netIndex{ ui->netInterface->findText( netInterface ) };
    const Games game{ *gameNames.key( gameName, Games::Invalid ) };

    ui->isPublic->setChecked( Settings::getSetting( SKeys::Setting, SSubKeys::IsPublic, serverName ).toBool() );
    ui->useUPNP->setChecked( Settings::getSetting( SKeys::Setting, SSubKeys::UseUPNP, serverName ).toBool() );
    ui->portNumber->setText( svrPort.isEmpty() ? Helper::intToStr( this->genPort() ) : svrPort );
    ui->gameName->setCurrentIndex( game == Games::Invalid ? 0 : *game );
    ui->netInterface->setCurrentIndex( netIndex >= 0 ? netIndex : 0 );
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
    static const QString title{ "Warning:" };
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

void CreateInstance::on_useUPNP_clicked()
{
    if ( ui->useUPNP->checkState() == Qt::Checked )
    {
        static const QString title{ "UPNP Feature:" };
        static const QString prompt{ "Enabling the UPNP Feature may result in ReMix being unable to ping the Master Mix! "
                                     "If you are unable to ping the Master Mix try disabling the UPNP Feature and manusally forward your desired ports.." };
        Helper::warningMessage( this, title, prompt );
    }
}
