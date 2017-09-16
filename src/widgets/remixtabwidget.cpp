
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
#include <QToolButton>
#include <QTabBar>

CreateInstance* ReMixTabWidget::createDialog{ nullptr };
ReMixTabWidget* ReMixTabWidget::tabInstance;
qint32 ReMixTabWidget::instanceCount;

ReMixTabWidget::ReMixTabWidget(QWidget* parent)
    : QTabWidget(parent)
{
    //Allow ServerInstance Tabs to be swapped and moved.
    this->setMovable( true );

    user = User::getInstance();
    createDialog = this->getCreateDialog( this );
    QObject::connect( createDialog, &CreateInstance::createServerAcceptedSignal,
                      this, &ReMixTabWidget::createServerAcceptedSlot );

    this->setTabsClosable( true );
    this->createTabButtons();

    //Initalize the First Server.
    this->createServer();

    QObject::connect( this, &QTabWidget::tabCloseRequested,
                      this, &ReMixTabWidget::tabCloseRequestedSlot );

    //Hide Tab-specific UI dialog windows when the tabs change.
    QObject::connect( this, &QTabWidget::currentChanged,
                      this, &ReMixTabWidget::currentChangedSlot );

    QObject::connect( this, &QTabWidget::tabBarDoubleClicked, [=](int index)
    {
        ReMixWidget* tabA{ serverMap.value( index ) };
        if ( tabA != nullptr )
        {
            QString title{ "Rename Server: [ %1 ]" };
                    title = title.arg( tabA->getServerName() );
            QString message{ "Please enter the new name you wish "
                             "to use for this server!" };

            QString response{ Helper::getTextResponse( this, title, message,
                                                       nullptr, 0 ) };

            if ( response.isEmpty() )
            {
                title = "Error:";
                message = "The Server name can not be empty!";
                Helper::warningMessage( this, title, message );
            }
            else
            {
                this->setTabText( index, response );
                tabA->renameServer( response );

                emit this->currentChanged( index );
            }
        }
    });

    //Refresh the server instance's ServerID when the Tabs are moved.
    QObject::connect( this->tabBar(), &QTabBar::tabMoved, [=](int from, int to)
    {
        ReMixWidget* tabA{ serverMap.take( from ) };
        ReMixWidget* tabB{ serverMap.take( to ) };

        if ( tabA != nullptr )
            serverMap.insert( to, tabA );

        if ( tabB != nullptr )
            serverMap.insert( from, tabB );
    });
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
            Settings::setServerRunning( QVariant( false ),
                                        server->getServerName() );
            server->close();
            server->deleteLater();
        }
    }
}

void ReMixTabWidget::sendMultiServerMessage(QString msg)
{
    ReMixWidget* server{ nullptr };
    for ( int i = 0; i < MAX_SERVER_COUNT; ++i )
    {
        server = serverMap.value( i );
        if ( server != nullptr )
            server->sendServerMessage( msg );
    }
}

qint32 ReMixTabWidget::getPlayerCount()
{
    qint32 playerCount{ 0 };

    ReMixWidget* server{ nullptr };
    for ( int i = 0; i < MAX_SERVER_COUNT; ++i )
    {
        server = serverMap.value( i );
        if ( server != nullptr )
            playerCount += server->getPlayerCount();
    }
    return playerCount;
}

quint32 ReMixTabWidget::getServerCount()
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
        if (parent != nullptr )
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

void ReMixTabWidget::createTabButtons()
{
    newTabButton = new QToolButton( this );
    newTabButton->setIcon( QIcon( ":/icon/newtab.png" ) );
    newTabButton->setCursor( Qt::ArrowCursor );
    newTabButton->setAutoRaise( true );

    this->setCornerWidget( newTabButton, Qt::TopLeftCorner );
    QObject::connect( newTabButton, &QToolButton::clicked,
                      this, &ReMixTabWidget::createServer );

    nightModeButton = new QToolButton( this );
    nightModeButton->setCursor( Qt::ArrowCursor );

    if ( Settings::getDarkMode() )
    {
        nightModeButton->setText( "Normal Mode" );
        nightMode = !nightMode;
    }
    else
        nightModeButton->setText( "Night Mode" );

    this->setCornerWidget( nightModeButton, Qt::TopRightCorner );
    QObject::connect( nightModeButton, &QToolButton::clicked, [=]()
    {
        QVariant type{ false };
        if ( !nightMode )
        {
            nightModeButton->setText( "Normal Mode" );
            type = true;
        }
        else
            nightModeButton->setText( "Night Mode" );

        QString title{ "Restart Required:" };
        QString msg{ "The theme change will take effect after "
                     "a restart." };

        Helper::warningMessage( this, title, msg );
        Settings::setDarkMode( type );

        nightMode = !nightMode;
    } );
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

void ReMixTabWidget::tabCloseRequestedSlot(qint32 index)
{
    QWidget* widget = this->widget( index );
    if ( widget != nullptr )
    {
        ReMixWidget* instance{ nullptr };

        QString title = QString( "Close [ %1 ]:" );
        QString prompt = QString( "You are about to shut down your ReMix "
                                  "game server!\r\nThis will affect [ %1 ] "
                                  "User(s) connected to it.\r\n\r\nAre you "
                                  "certain?" );

        for ( int i = 0; (i < MAX_SERVER_COUNT)
                      || (i < serverMap.size()); ++i )
        {
            instance = serverMap.value( i );
            if ( instance != nullptr )
            {
                if ( widget == instance )
                {
                    title = title.arg( instance->getServerName() );
                    prompt = prompt.arg( instance->getPlayerCount() );

                    instance->sendServerMessage( "The admin is taking this "
                                                 "server down..." );

                    //Last server instance is being closed. Prompt User.
                    if ( Helper::confirmAction( this, title, prompt ) )
                    {
                        Settings::setServerRunning( QVariant( false ),
                                                    instance->getServerName() );

                        serverMap.remove( i );
                        this->removeTab( index );

                        //Last Server instance. Close ReMix.
                        instanceCount -= 1;
                        if ( instanceCount == 0 )
                            createDialog->show();
                        else if ( instanceCount == 1 )
                            this->setCurrentIndex( 0 );
                        else
                            this->setCurrentIndex( index - 1 );

                        instance->close();
                        instance->deleteLater();

                        break;
                    }
                    else
                    {
                        instance->sendServerMessage( "The admin changed his"
                                                     " or her mind! (yay!)"
                                                     "..." );
                    }
                }
            }
        }
    }
}

void ReMixTabWidget::currentChangedSlot(qint32 newTab)
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

    QString serverName{ server->getName() };
    QString title{ "Unable to Initialize Server:" };
    QString prompt{ "You are unable to initialize two servers with the same"
                    " name!" };

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
        else
        {
            if ( Helper::cmpStrings( instance->getServerName(), serverName ) )
            {
                Helper::warningMessage( this, title, prompt );
                break;
            }
        }
    }

    if ( serverID <= MAX_SERVER_COUNT )
    {
        instanceCount += 1;
        serverMap.insert( serverID, new ReMixWidget( this, server ) );
        this->insertTab( serverMap.size() - 1,
                         serverMap.value( serverID ),
                         serverName );
        this->setCurrentIndex( serverID );
        Settings::setServerRunning( QVariant( true ), serverName );
    }
}
