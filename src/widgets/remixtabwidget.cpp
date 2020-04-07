
//Class includes.
#include "remixtabwidget.hpp"

//ReMix includes.
#include "createinstance.hpp"
#include "remixwidget.hpp"
#include "serverinfo.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "remix.hpp"
#include "user.hpp"

//Qt Includes.
#include <QApplication>
#include <QToolButton>
#include <QTabBar>

CreateInstance* ReMixTabWidget::createDialog{ nullptr };
ReMixTabWidget* ReMixTabWidget::tabInstance{ nullptr };
QMap<int, ReMixWidget*> ReMixTabWidget::serverMap;
qint32 ReMixTabWidget::instanceCount;

ReMixTabWidget::ReMixTabWidget(QWidget* parent)
    : QTabWidget(parent)
{
    //Allow ServerInstance Tabs to be swapped and moved.
    this->setMovable( true );

    user = User::getInstance();
    createDialog = this->getCreateDialog( this );
    QObject::connect( createDialog, &CreateInstance::createServerAcceptedSignal, this, &ReMixTabWidget::createServerAcceptedSlot, Qt::QueuedConnection );

    this->setTabsClosable( true );
    this->createTabButtons();

    //Initalize the First Server.
    this->createServer();

    QObject::connect( this, &QTabWidget::tabCloseRequested, this, &ReMixTabWidget::tabCloseRequestedSlot, Qt::QueuedConnection );

    //Hide Tab-specific UI dialog windows when the tabs change.
    QObject::connect( this, &QTabWidget::currentChanged, this, &ReMixTabWidget::currentChangedSlot, Qt::QueuedConnection );

    QObject::connect( this, &QTabWidget::tabBarDoubleClicked, this,
    [=](int index)
    {
        ReMixWidget* tabA{ serverMap.value( index ) };
        if ( tabA != nullptr )
        {
            QString title{ "Rename Server: [ %1 ]" };
                    title = title.arg( tabA->getServerName() );
            QString message{ "Please enter the new name you wish to use for this server!" };

            bool accepted{ false };
            QString response{ Helper::getTextResponse( this, title, message, tabA->getServerName(), &accepted, 0 ) };

            //The User clicked OK. Do nothing if the User clicked Cancel.
            if ( accepted )
            {
                bool warnUser{ false };
                //The Response was not empty, change the Server's Name.
                if ( !response.isEmpty() )
                {
                    if ( Helper::strContainsStr( response, "world=" ) )
                    {
                        message = "Servers cannot be initialized with the World selection within the name. Please try again.";
                        warnUser = true;
                    }
                    else
                    {
                        //Check if the new name is the same as the old.
                        if ( !Helper::cmpStrings( tabA->getServerName(), response ) )
                        {
                            this->setTabText( index, response );
                            tabA->renameServer( response );

                            emit this->currentChanged( index );
                        }
                        else //The new Server name is the same as the Old.
                        {
                            message = "The new server name can not be the same as the old name!";
                            warnUser = true;
                        }
                    }
                }
                else //The Response was empty or otherwise invalid.
                {    //Inform the User.
                    message = "The server name can not be empty!";
                    warnUser = true;
                }

                if ( warnUser )
                    Helper::warningMessage( this, "Error:", message );
            }
        }
    }, Qt::QueuedConnection );

    //Refresh the server instance's ServerID when the Tabs are moved.
    QObject::connect( this->tabBar(), &QTabBar::tabMoved, this->tabBar(),
    [=](int from, int to)
    {
        ReMixWidget* tabA{ serverMap.take( from ) };
        ReMixWidget* tabB{ serverMap.take( to ) };

        if ( tabA != nullptr )
            serverMap.insert( to, tabA );

        if ( tabB != nullptr )
            serverMap.insert( from, tabB );
    }, Qt::QueuedConnection );
}

ReMixTabWidget::~ReMixTabWidget()
{
    nightModeButton->deleteLater();
    newTabButton->deleteLater();

    ReMixWidget* server{ nullptr };
    for ( int i = 0; i < MAX_SERVER_COUNT; ++i )
    {
        server = serverMap.value( i );
        if ( server != nullptr )
        {
            Settings::setSetting( false, SKeys::Setting, SSubKeys::IsRunning, server->getServerName() );
            server->close();
            server->deleteLater();
        }
    }
}

void ReMixTabWidget::sendMultiServerMessage(const QString& msg)
{
    ReMixWidget* server{ nullptr };
    for ( int i = 0; i < MAX_SERVER_COUNT; ++i )
    {
        server = serverMap.value( i );
        if ( server != nullptr )
            server->sendServerMessage( msg );
    }
}

quint32 ReMixTabWidget::getPlayerCount() const
{
    quint32 playerCount{ 0 };

    ReMixWidget* server{ nullptr };
    for ( int i = 0; i < MAX_SERVER_COUNT; ++i )
    {
        server = serverMap.value( i );
        if ( server != nullptr )
            playerCount += server->getPlayerCount();
    }
    return playerCount;
}

quint32 ReMixTabWidget::getServerCount() const
{
    quint32 serverCount{ 0 };

    ReMixWidget* server{ nullptr };
    for ( int i = 0; i < MAX_SERVER_COUNT; ++i )
    {
        server = serverMap.value( i );
        if ( server != nullptr )
            ++serverCount;
    }
    return serverCount;
}

qint32 ReMixTabWidget::getPrevTabIndex() const
{
    return prevTabIndex;
}

void ReMixTabWidget::setPrevTabIndex(const qint32& value)
{
    prevTabIndex = value;
}

qint32 ReMixTabWidget::getInstanceCount()
{
    return instanceCount;
}

ReMixTabWidget* ReMixTabWidget::getTabInstance(QWidget* parent)
{
    if ( tabInstance == nullptr )
    {
        if ( parent != nullptr )
            tabInstance = new ReMixTabWidget( parent );
        else
            tabInstance = new ReMixTabWidget();
    }
    return tabInstance;
}

CreateInstance* ReMixTabWidget::getCreateDialog(QWidget* parent)
{
    if ( createDialog == nullptr )
    {
        createDialog = new CreateInstance( parent );
    }
    return createDialog;
}

void ReMixTabWidget::remoteCloseServer(ServerInfo* server, const bool restart)
{
    ReMixTabWidget* tabWidget{ ReMixTabWidget::getTabInstance() };
    if ( tabWidget != nullptr
      && server != nullptr )
    {
        ReMixWidget* instance{ nullptr };
        for ( int i = 0; i < MAX_SERVER_COUNT; ++i )
        {
            instance = serverMap.value( i );
            if ( instance != nullptr )
            {
                if ( instance->getServerInfo()->getServerName() == server->getServerName() )
                {
                    removeServer( i, true, restart );
                    break;
                }
            }
        }
    }
}

void ReMixTabWidget::setToolTipString(ReMixWidget* widget)
{
    ReMixTabWidget* tabWidget{ ReMixTabWidget::getTabInstance() };
    if ( tabWidget != nullptr
      && widget != nullptr )
    {
        qint32 index{ serverMap.key( widget ) };

        ServerInfo* server{ widget->getServerInfo() };
        QString toolTip{ "#Calls: %1 #Pings: %2 #Pkt-DC: %3 #Dup-DC: %4 #IP-DC: %5 #IN: %6 Bd #OUT: %7 Bd" };
                toolTip = toolTip.arg( server->getUserCalls() )
                                 .arg( server->getUserPings() )
                                 .arg( server->getPktDc() )
                                 .arg( server->getDupDc() )
                                 .arg( server->getIpDc() )
                                 .arg( server->getBaudIn() )
                                 .arg( server->getBaudOut() );

        tabWidget->setTabToolTip( index, toolTip );
    }
}

void ReMixTabWidget::removeServer(const qint32& index, const bool& remote, const bool& restart)
{
    ReMixTabWidget* tabWidget{ ReMixTabWidget::getTabInstance() };
    if ( tabWidget == nullptr )
        return;

    ReMixWidget* instance = serverMap.value( index );
    if ( instance == nullptr )
        return;

    ServerInfo* server{ instance->getServerInfo() };
    if ( server == nullptr )
        return;

    quint16 privatePort{ server->getPrivatePort() };

    QString gameName{ server->getGameName() };
    QString name{ server->getServerName() };

    bool isPublic{ server->getIsPublic() };
    bool useUPNP{ server->getUseUPNP() };

    Settings::setSetting( false, SKeys::Setting, SSubKeys::IsRunning, instance->getServerName() );

    serverMap.remove( index );
    tabWidget->removeTab( index );

    instance->disconnect();
    delete instance;

    instanceCount -= 1;
    if ( !restart ) //The server was designated to not restart.
    {
        //Server instance was the last, check if it was a remote shutdown and if so, ignore the creation dialog and gracefully close.
        if ( instanceCount == 0 )
        {
            if ( !remote )
            {
                Settings* settings{ Settings::getInstance() };
                if ( settings != nullptr )
                {
                    if ( settings->isVisible() )
                        settings->close();
                }

                User* user{ User::getInstance() };
                if ( user != nullptr )
                {
                    if ( user->isVisible() )
                        user->close();
                }

                if ( !createDialog->isVisible() )
                    createDialog->show();
            }
            else
                qApp->quit();
        }
    }
    else    //The server is set to restart. Use the previous server's information.
        createDialog->restartServer( name, gameName, privatePort, useUPNP, isPublic );

    if ( instanceCount == 1 )
        tabWidget->setCurrentIndex( 0 );
    else
        tabWidget->setCurrentIndex( index - 1 );

    tabWidget->repositionServerIndices();
}

void ReMixTabWidget::repositionServerIndices()
{
    //Reposition the server instance's ServerID when Tabs are removed.
    QMap<int, ReMixWidget*> tempMap;
    for (  auto server : serverMap )
    {
        if ( server != nullptr )
            tempMap.insert( this->indexOf( server ), server );
    }

    serverMap = tempMap;
    tempMap.clear();
}

void ReMixTabWidget::createTabButtons()
{
    newTabButton = new QToolButton( this );
    newTabButton->setIcon( QIcon( ":/icon/newtab.png" ) );
    newTabButton->setCursor( Qt::ArrowCursor );
    newTabButton->setAutoRaise( true );

    this->setCornerWidget( newTabButton, Qt::TopLeftCorner );
    QObject::connect( newTabButton, &QToolButton::clicked, this, &ReMixTabWidget::createServer, Qt::QueuedConnection );

    nightModeButton = new QToolButton( this );
    nightModeButton->setCursor( Qt::ArrowCursor );

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::DarkMode ).toBool() )
    {
        nightModeButton->setText( "Normal Mode" );
        nightMode = !nightMode;
    }
    else
        nightModeButton->setText( "Night Mode" );

    this->setCornerWidget( nightModeButton, Qt::TopRightCorner );
    QObject::connect( nightModeButton, &QToolButton::clicked, nightModeButton,
    [=]()
    {
        bool type{ false };
        if ( !nightMode )
        {
            nightModeButton->setText( "Normal Mode" );
            type = true;
        }
        else
            nightModeButton->setText( "Night Mode" );

        QString title{ "Restart Required:" };
        QString msg{ "The theme change will take effect after a restart." };

        Helper::warningMessage( this, title, msg );
        Settings::setSetting( type, SKeys::Setting, SSubKeys::DarkMode );

        nightMode = !nightMode;
    }, Qt::QueuedConnection );
}

void ReMixTabWidget::createServer()
{
    if ( user == nullptr )
        return;

    if ( createDialog->isVisible() )
        createDialog->hide();
    else
        createDialog->show();
}

void ReMixTabWidget::tabCloseRequestedSlot(const qint32& index)
{
    QWidget* widget = this->widget( index );
    if ( widget != nullptr )
    {
        ReMixWidget* instance{ nullptr };

        QString title{ "Close [ %1 ]:" };
        QString prompt{ "You are about to shut down your ReMix game server!\r\nThis will affect [ %1 ] "
                        "User(s) connected to it.\r\n\r\nAre you certain?" };

        for ( int i = 0; ( i < MAX_SERVER_COUNT )
           || ( i < serverMap.size() ); ++i )
        {
            instance = serverMap.value( i );
            if ( instance != nullptr )
            {
                if ( widget == instance )
                {
                    title = title.arg( instance->getServerName() );
                    prompt = prompt.arg( instance->getPlayerCount() );

                    instance->sendServerMessage( "The admin is taking this server down..." );

                    //Last server instance is being closed. Prompt User.
                    if ( Helper::confirmAction( this, title, prompt ) )
                    {
                        //Correctly switch the tab to a valid server instance before closing the current instance.
                        if ( i == 0 )
                        {
                            qint32 servercount{ serverMap.size() };
                            if ( servercount >= 1 )
                                this->setCurrentIndex( i + 1 );
                        }
                        else
                            this->setCurrentIndex( i - 1 );

                        this->removeServer( i );
                        break;
                    }
                    instance->sendServerMessage( "The admin changed his or her mind! (yay!)..." );
                }
            }
        }
    }
}

void ReMixTabWidget::currentChangedSlot(const qint32& newTab)
{
    //Make sure there are valid Servers to access.
    if ( instanceCount == 0 )
        return;

    ReMixWidget* server{ serverMap.value( newTab ) };
    if ( server != nullptr )
    {
        ReMix::updateTitleBars( server->getServerInfo() );
    }
    this->setPrevTabIndex( newTab );
}

void ReMixTabWidget::createServerAcceptedSlot(ServerInfo* server)
{
    if ( server == nullptr )
        return;

    QString serverName{ server->getServerName() };

    qint32 serverID{ 0 };
    ReMixWidget* instance{ nullptr };

    for ( int i = 0; i < MAX_SERVER_COUNT; ++i )
    {
        serverID = MAX_SERVER_COUNT + 1;
        instance = serverMap.value( i );
        if ( instance == nullptr )
        {
            serverID = i;
            break;
        }
    }

    if ( serverID <= MAX_SERVER_COUNT )
    {
        instanceCount += 1;
        serverMap.insert( serverID, new ReMixWidget( this, server ) );
        this->insertTab( serverMap.size() - 1, serverMap.value( serverID ), serverName );
        this->setCurrentIndex( serverID );
        Settings::setSetting( server->getIsPublic(), SKeys::Setting, SSubKeys::IsRunning, serverName );
    }
}
