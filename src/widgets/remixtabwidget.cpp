
#include "includes.hpp"
#include "remixtabwidget.hpp"

CreateInstance* ReMixTabWidget::createDialog{ nullptr };
ReMixTabWidget* ReMixTabWidget::tabInstance;
qint32 ReMixTabWidget::instanceCount;
QPalette ReMixTabWidget::defaultPalette;
QPalette ReMixTabWidget::customPalette;

ReMixTabWidget::ReMixTabWidget(QWidget* parent)
    : QTabWidget(parent)
{
    //Allow ServerInstance Tabs to be swapped and moved.
    this->setMovable( true );

    user = ReMix::getUser();
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

    //Refresh the server instance's ServerID when the Tabs are moved.
    QObject::connect( this->tabBar(), &QTabBar::tabMoved, [=](int from, int to)
    {
        ReMixWidget* tabA{ serverMap.take( from) };
        ReMixWidget* tabB{ serverMap.take( to ) };

        if ( tabA != nullptr )
            serverMap.insert( to, tabA );

        if ( tabB != nullptr )
            serverMap.insert( from, tabB );
    });
    defaultPalette = this->palette();
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

quint32 ReMixTabWidget::getPlayerCount()
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

quint32 ReMixTabWidget::getPrevTabIndex() const
{
    return prevTabIndex;
}

void ReMixTabWidget::setPrevTabIndex(const quint32& value)
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

#include <QLabel>
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
    nightModeButton->setText( "Night Mode" );

    this->setCornerWidget( nightModeButton, Qt::TopRightCorner );
    QObject::connect( nightModeButton, &QToolButton::clicked, [=]()
    {
        qint32 type{ 1 };
        if ( nightMode )
        {
            nightModeButton->setText( "Night Mode" );
            type = 0;
        }
        else
            nightModeButton->setText( "Normal Mode" );

        this->applyThemes( type );
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

void ReMixTabWidget::applyThemes(qint32 type)
{
    customPalette = defaultPalette;
    if ( type == Themes::DARK )
    {
        //Activated Color Roles
        customPalette.setColor( QPalette::All, QPalette::WindowText,
                          QColor( 231, 231, 231 ) );
        customPalette.setColor( QPalette::All, QPalette::Text,
                          QColor( 231, 231, 231 ) );
        customPalette.setColor( QPalette::All, QPalette::Base,
                          QColor( 51, 51, 51 ) );
        customPalette.setColor( QPalette::All, QPalette::Window,
                          QColor( 51, 51, 51 ) );
        customPalette.setColor( QPalette::All, QPalette::Shadow,
                          QColor( 105, 105, 105 ) );
        customPalette.setColor( QPalette::All, QPalette::Midlight,
                          QColor( 227, 227, 227 ) );
        customPalette.setColor( QPalette::All, QPalette::Button,
                          QColor( 35, 35, 35 ) );
        customPalette.setColor( QPalette::All, QPalette::Light,
                          QColor( 255, 255, 255 ) );
        customPalette.setColor( QPalette::All, QPalette::Dark,
                          QColor( 35, 35, 35 ) );
        customPalette.setColor( QPalette::All, QPalette::Mid,
                          QColor( 160, 160, 160 ) );
        customPalette.setColor( QPalette::All, QPalette::BrightText,
                          QColor( 255, 255, 255 ) );
        customPalette.setColor( QPalette::All, QPalette::ButtonText,
                          QColor( 231, 231, 231 ) );
        customPalette.setColor( QPalette::All, QPalette::HighlightedText,
                          QColor( 255, 255, 255 ) );
        customPalette.setColor( QPalette::All, QPalette::Link,
                          QColor( 0, 122, 144 ) );
        customPalette.setColor( QPalette::All, QPalette::LinkVisited,
                          QColor( 165, 122, 255 ) );
        customPalette.setColor( QPalette::All, QPalette::AlternateBase,
                          QColor( 81, 81, 81 ) );
        customPalette.setColor( QPalette::All, QPalette::ToolTipText,
                          QColor( 231, 231, 231 ) );

        //Disabled Color Roles
        customPalette.setColor( QPalette::Disabled, QPalette::Button,
                          QColor( 35, 35, 35 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::WindowText,
                          QColor( 255, 255, 255 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::Text,
                          QColor( 255, 255, 255 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::Base,
                          QColor( 68, 68, 68 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::Window,
                          QColor( 68, 68, 68 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::Shadow,
                          QColor( 0, 0, 0 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::Midlight,
                          QColor( 247, 247, 247 ) );
    }
    qApp->setPalette( customPalette );
}

void ReMixTabWidget::tabCloseRequestedSlot(quint32 index)
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

void ReMixTabWidget::currentChangedSlot(quint32 newTab)
{
    //Make sure there are valid Servers to access.
    if ( instanceCount == 0 )
        return;

    ReMixWidget* server{ serverMap.value( newTab ) };
    if ( server != nullptr )
    {
        ReMix::updateTitleBars( server->getServerID(),
                                server->getPrivatePort() );
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

    quint32 serverID{ 0 };
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
            if ( instance->getServerName().compare(
                 serverName, Qt::CaseInsensitive ) == 0 )
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
