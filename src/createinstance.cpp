
#include "includes.hpp"
#include "createinstance.hpp"
#include "ui_createinstance.h"

//Initialize our accepted Command List.
const QString CreateInstance::gameNames[ GAME_NAME_COUNT ] =
{
    "WOS",
    "TOY",
    "W97"
};

CreateInstance::CreateInstance(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateInstance)
{
    ui->setupUi(this);
    collator.setNumericMode( true );
    collator.setCaseSensitivity( Qt::CaseInsensitive );

    randDev = new RandDev();
    this->updateServerList( true );
}

CreateInstance::~CreateInstance()
{
    this->disconnect();
    this->deleteLater();

    delete ui;
}

void CreateInstance::updateServerList(bool firstRun)
{
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
        for ( int i = 0; i < SETTINGS_KEY_COUNT; ++i )
        {
            if ( name.compare( Settings::keys[ i ],
                               Qt::CaseInsensitive ) == 0 )
            {
                skip = true;
            }
        }

        if ( !skip )
        {
            running = false;
            if ( firstRun )
            {
                Settings::setServerRunning( QVariant( false ), name );
            }
            else
                running = Settings::getServerRunning( name );
        }

        if ( !skip && !running )
        {
            validServers.append( name );
            //ui->servers->addItem( name );
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
}

void CreateInstance::on_initializeServer_clicked()
{
    QString svrName{ ui->servers->currentText() };
    if ( !svrName.isEmpty() )
    {
        ServerInfo* server = new ServerInfo( svrName );
        if ( server == nullptr )    //Failed to create the ServerInfo instance.
            return;

        QString gameName{ gameNames[ ui->gameName->currentIndex() ] };
        QString svrPort{ ui->portNumber->text( ) };
        bool isPublic{ ui->isPublic->isChecked() };

        server->setServerID( Settings::getServerID( svrName ) );
        server->setIsPublic( ui->isPublic->isChecked() );
        server->setPrivatePort( svrPort.toUShort() );
        server->setGameName( gameName );
        server->setLogFiles( true );
        server->setName( svrName );

        ReMix::getSynRealData( server );

        Settings::setIsPublic( QVariant( isPublic ), svrName );
        Settings::setPortNumber( QVariant( svrPort ), svrName );
        Settings::setGameName( QVariant( gameName ), svrName );

        emit this->createServerAcceptedSignal( server );
        emit this->accept();
    }
    else
    {
        QString title{ "Error:" };
        QString message{ "Servers cannot be initialized without a name!" };
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

bool CreateInstance::testPort(quint16 port)
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
        QString title = QString( "Close ReMix:" );
        QString prompt = QString( "You are about to shut down your ReMix "
                                  "game server!\r\n\r\nAre you "
                                  "certain?" );

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
        QString gameName{ Settings::getGameName( svrName ) };
        if ( !gameName.isEmpty() )
        {
            bool notFound{ true };
            for ( int i = 0; i < GAME_NAME_COUNT; ++i )
            {
                if ( gameNames[ i ].compare( gameName,
                                             Qt::CaseInsensitive ) == 0 )
                {
                    ui->gameName->setCurrentIndex( i );
                    notFound = false;
                }
            }

            if ( notFound )
                ui->gameName->setCurrentIndex( 0 );
        }
    }

    QString svrPort{ Settings::getPortNumber( svrName ) };
    if ( !svrPort.isEmpty() )
        ui->portNumber->setText( svrPort );
    else
        ui->portNumber->setText( Helper::intToStr( this->genPort() ) );

    bool isPublic{ Settings::getIsPublic( svrName ) };
    if ( isPublic )
        ui->isPublic->setChecked( isPublic );
    else
        ui->isPublic->setChecked( false );
}

void CreateInstance::on_portNumber_textChanged(const QString &arg1)
{
    //Reduce the User Inputted Port Number to within proper bounds.
    quint16 port{ arg1.toUShort() };
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
    }
    ui->portNumber->setText( Helper::intToStr( port ) );
}

void CreateInstance::on_servers_currentTextChanged(const QString &arg1)
{
    int index{ ui->servers->findText( arg1 ) };
    if ( index >= 0 )
        ui->servers->setCurrentIndex( index );
}
