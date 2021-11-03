
//Class includes.
#include "remixtabwidget.hpp"

//ReMix includes.
#include "createinstance.hpp"
#include "remixwidget.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "server.hpp"
#include "theme.hpp"
#include "remix.hpp"
#include "user.hpp"

//Qt Includes.
#include <QApplication>
#include <QToolButton>
#include <QTabBar>
#include <QMenu>

CreateInstance* ReMixTabWidget::createDialog{ nullptr };
ReMixTabWidget* ReMixTabWidget::tabInstance{ nullptr };
Theme* ReMixTabWidget::themeInstance{ nullptr };

QMap<int, ReMixWidget*> ReMixTabWidget::serverMap;
qint32 ReMixTabWidget::instanceCount;

ReMixTabWidget::ReMixTabWidget(QWidget* parent)
    : QTabWidget(parent)
{
    //Allow ServerInstance Tabs to be swapped and moved.
    this->setMovable( true );

    tabInstance = this;
    this->setTabsClosable( false );
    this->createTabButtons();
    this->setContextMenuPolicy( Qt::CustomContextMenu );

    this->setThemeInstance( Theme::getInstance() );

    QObject::connect( this, &ReMixTabWidget::customContextMenuRequested, this, &ReMixTabWidget::customContextMenuRequestedSlot );

    createDialog = CreateInstance::getInstance( this );

    QObject::connect( createDialog, &CreateInstance::restartServerListSignal, this, &ReMixTabWidget::restartServerListSlot );
    QObject::connect( createDialog, &CreateInstance::createServerAcceptedSignal, this, &ReMixTabWidget::createServerAcceptedSlot );
    QObject::connect( createDialog, &CreateInstance::quitSignal, ReMix::getInstance(), &ReMix::quitSlot, Qt::QueuedConnection );
    createDialog->updateServerList( true );

    //Initalize the First Server. --Only if no stored servers are marked as "AutoRestart"
    if ( !createDialog->getLoadingOldServers() )
        this->createServer();

    createInstanceTimer.start( static_cast<int>( Globals::UI_UPDATE_TIME ) );
    QObject::connect( &createInstanceTimer, &QTimer::timeout, &createInstanceTimer,
    [=]()
    {
        if ( serverMap.isEmpty() && !createDialog->isVisible() )
            createDialog->show();
    } );

    QObject::connect( this, &ReMixTabWidget::themeChangedSignal, this, &ReMixTabWidget::themeChangedSlot );
    QObject::connect( this, &QTabWidget::tabCloseRequested, this, &ReMixTabWidget::tabCloseRequestedSlot );

    //Hide Tab-specific UI dialog windows when the tabs change.
    QObject::connect( this, &QTabWidget::currentChanged, this, &ReMixTabWidget::currentChangedSlot );

    //QObject::connect( this, &QTabWidget::tabBarDoubleClicked, this, &ReMixTabWidget::renameServerTabSlot );

    QToolButton* nightButton{ this->getNightModeButton() };
                 nightButton->setCursor( Qt::ArrowCursor );
    this->setCornerWidget( nightButton, Qt::TopRightCorner );

    QObject::connect( nightButton, &QToolButton::clicked, this, &ReMixTabWidget::nightModeButtonClickedSlot );

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
    } );
}

ReMixTabWidget::~ReMixTabWidget()
{
    if ( createDialog != nullptr )
    {
        createDialog->deleteLater();
        createDialog = nullptr;
    }

    if ( themeInstance != nullptr )
    {
        themeInstance->deleteLater();
        themeInstance = nullptr;
    }

    nightModeButton->deleteLater();
    newTabButton->deleteLater();

    for ( ReMixWidget* server : serverMap )
    {
        if ( server != nullptr )
        {
            //Only Mark Servers as inactive when closing if they aren't tagged for auto-restart.
            if ( !Settings::getSetting( SKeys::Rules, SSubKeys::AutoRestart, server->getServerName() ).toBool() )
                Settings::setSetting( false, SKeys::Setting, SSubKeys::IsRunning, server->getServerName() );

            server->close();
            server->deleteLater();
        }
    }
}

void ReMixTabWidget::sendMultiServerMessage(const QString& msg)
{
    for ( ReMixWidget* server : serverMap )
    {
        if ( server != nullptr )
            server->sendServerMessage( msg );
    }
}

quint32 ReMixTabWidget::getPlayerCount() const
{
    auto lambda = [&](int a, ReMixWidget* b){ return static_cast<int>( b->getPlayerCount() ) + a; };
    return static_cast<quint32>( std::accumulate( serverMap.begin(), serverMap.end(), 0, lambda ) );
}

quint32 ReMixTabWidget::getServerCount() const
{
    return static_cast<quint32>( std::count_if( serverMap.begin(), serverMap.end(),
                                 [](ReMixWidget* i)
                                 {
                                     return i != nullptr ;
                                 } ) );
}

qint32 ReMixTabWidget::getInstanceCount()
{
    return instanceCount;
}

ReMixTabWidget* ReMixTabWidget::getInstance(QWidget* parent)
{
    if ( tabInstance == nullptr )
        tabInstance = new ReMixTabWidget( parent );

    return tabInstance;
}

void ReMixTabWidget::remoteCloseServer(Server* server, const bool restart)
{
    ReMixTabWidget* tabWidget{ ReMixTabWidget::getInstance() };
    if ( tabWidget != nullptr
      && server != nullptr )
    {
        for ( int i = 0; i < static_cast<int>( Globals::MAX_SERVER_COUNT ); ++i )
        {
            const ReMixWidget* instance{ serverMap.value( i ) };
            if ( instance != nullptr )
            {
                if ( instance->getServer()->getServerName() == server->getServerName() )
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
    ReMixTabWidget* tabWidget{ ReMixTabWidget::getInstance() };
    if ( tabWidget != nullptr
      && widget != nullptr )
    {
        qint32 index{ serverMap.key( widget ) };

        Server* server{ widget->getServer() };

        QString bytesInUnit{ "" };
        QString bytesIn{ "" };
        Helper::sanitizeToFriendlyUnits( server->getBytesIn(), bytesIn, bytesInUnit );

        QString bytesOutUnit{ "" };
        QString bytesOut{ "" };
        Helper::sanitizeToFriendlyUnits( server->getBytesOut(), bytesOut, bytesOutUnit );

        QString toolTip{ "#Calls: %1 #Pings: %2 #Pkt-DC: %3 #Dup-DC: %4 #IP-DC: %5 #IN: %6 %7 #OUT: %8 %9" };
                toolTip = toolTip.arg( server->getUserCalls() )
                                 .arg( server->getUserPings() )
                                 .arg( server->getPktDc() )
                                 .arg( server->getDupDc() )
                                 .arg( server->getIpDc() )
                                 .arg( bytesIn )
                                 .arg( bytesInUnit )
                                 .arg( bytesOut )
                                 .arg( bytesOutUnit );

        tabWidget->setTabToolTip( index, toolTip );
    }
}

Theme* ReMixTabWidget::getThemeInstance() const
{
    if ( themeInstance == nullptr )
        themeInstance = Theme::getInstance();

    return themeInstance;
}

void ReMixTabWidget::setThemeInstance(Theme* newThemeInstance)
{
    themeInstance = newThemeInstance;
}

QToolButton* ReMixTabWidget::getNightModeButton()
{
    if ( nightModeButton == nullptr )
        this->setNightModeButton( new QToolButton( this ) );

    return nightModeButton;
}

void ReMixTabWidget::setNightModeButton(QToolButton* newNightModeButton)
{
    nightModeButton = newNightModeButton;
}

QToolButton* ReMixTabWidget::getNewTabButton()
{
    if ( newTabButton == nullptr )
        this->setNewTabButton( new QToolButton( this ) );

    return newTabButton;
}

void ReMixTabWidget::setNewTabButton(QToolButton* button)
{
    newTabButton = button;
}

Themes ReMixTabWidget::getThemeType() const
{
    return themeType;
}

void ReMixTabWidget::setThemeType(Themes newThemeType)
{
    themeType = newThemeType;
}

void ReMixTabWidget::removeServer(const qint32& index, const bool& remote, const bool& restart)
{
    ReMixTabWidget* tabWidget{ ReMixTabWidget::getInstance() };
    if ( tabWidget == nullptr )
        return;

    ReMixWidget* instance{ serverMap.value( index, nullptr ) };
    if ( instance == nullptr )
        return;

    Server* server{ instance->getServer() };
    if ( server == nullptr )
        return;

    static const QString title{ "Disable AutoRestart:" };
    static const QString prompt{ "Do you wish to disable the AutoRestart rule on the closed server?" };

    QString gameName{ server->getGameName() };
    QString name{ server->getServerName() };

    Settings::setSetting( false, SKeys::Setting, SSubKeys::IsRunning, name );

    serverMap.remove( index );
    tabWidget->removeTab( index );

    instance->disconnect();
    instance->deleteLater();
    instance = nullptr;

    instanceCount -= 1;
    if ( !restart ) //The server was designated to not restart.
    {
        if ( Settings::getSetting( SKeys::Rules, SSubKeys::AutoRestart, name ).toBool()
          && !remote )
        {
            if ( Helper::confirmAction( nullptr, title, prompt ) )
                Settings::setSetting( false, SKeys::Rules, SSubKeys::AutoRestart, name );
        }

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
        createDialog->restartServer( name, gameName, server->getPrivateIP(), server->getPrivatePort(), server->getUseUPNP(), server->getIsPublic() );

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
    for ( ReMixWidget* server : serverMap )
    {
        if ( server != nullptr )
            tempMap.insert( this->indexOf( server ), server );
    }

    serverMap = tempMap;
    tempMap.clear();
}

void ReMixTabWidget::createTabButtons()
{
    QToolButton* button = this->getNewTabButton();
    if ( button != nullptr )
    {
        this->setCornerWidget( button, Qt::TopLeftCorner );
        QObject::connect( button, &QToolButton::clicked, this, &ReMixTabWidget::createServer );

        Themes theme{ this->getThemeType() };
        if ( Settings::getSetting( SKeys::Setting, SSubKeys::DarkMode ).toBool() )
        {
            this->getNightModeButton()->setText( "Normal Mode" );
            theme = Themes::Dark;
        }
        else
        {
            this->getNightModeButton()->setText( "Night Mode" );
            theme = Themes::Light;
        }
        emit this->themeChangedSlot( theme );
    }
    button = nullptr;
}

void ReMixTabWidget::nightModeButtonClickedSlot()
{
    Themes theme{ this->getThemeType() };
    QToolButton* nightButton{ this->getNightModeButton() };
    if ( nightButton != nullptr )
    {
        if ( theme == Themes::Light )
        {
            nightButton->setText( "Normal Mode" );
            theme = Themes::Dark;
        }
        else
        {
            nightButton->setText( "Night Mode" );
            theme = Themes::Light;
        }

        Settings::setSetting( ( theme != Themes::Light ), SKeys::Setting, SSubKeys::DarkMode );
        this->getThemeInstance()->setThemeType( theme );

        emit this->themeChangedSlot( theme );
    }
}

void ReMixTabWidget::themeChangedSlot(const Themes& theme)
{
    QToolButton* button{ this->getNewTabButton() };
    if ( button != nullptr )
    {
        button->setIconSize( QSize( 32, 32 ) ); //Increase the default from 16/16 pixels to 32/32 pixels.
        QString icon{ ":/icon/newtablight.png" };
        if ( theme == Themes::Dark )
            icon = ":/icon/newtabdark.png";

        button->setIcon( QIcon( icon ) );
    }
    this->setThemeType( theme );
}

void ReMixTabWidget::createServer()
{
    if ( createDialog->isVisible() )
        createDialog->hide();
    else
        createDialog->show();
}

int ReMixTabWidget::tabAt(const QPoint& position, const qint32& cornerButtonWidth) const
{
    //Reimplemented the default QTabBar::tabAt(const QPoint& position) function to account for the usage of a QTabCornerWidget.
    //This allows us to find the *true* Tab at the QPoint position by shifting the iterated tab's QRect data by the width of the QTabCornerWidget.
    QTabBar* bar{ this->tabBar() };
    if ( bar != nullptr )
    {
        QRect currentRect{ bar->tabRect( bar->currentIndex() ) };
        for ( int i = 0; i < bar->count(); ++i )
        {
            currentRect = bar->tabRect( i );
            currentRect.adjust( cornerButtonWidth, 0, cornerButtonWidth, 0 );

            if ( currentRect.contains( position ) )
                return i;
        }
    }
    return -1;
}

void ReMixTabWidget::customContextMenuRequestedSlot(const QPoint& point)
{
    if ( point.isNull() )
        return;

    QMenu menu( this );

    QSize cornerWidgetSize{ this->cornerWidget( Qt::TopLeftCorner )->size() };
    int tabIndex = this->tabAt( point, cornerWidgetSize.width() );
    if ( tabIndex >= 0 )
    {
        const ReMixWidget* widget{ serverMap.value( tabIndex ) };
        if ( widget != nullptr )
        {
            QString name{ widget->getServerName() };

            QString renameMsg{ "Rename [ %1 ]" };
                    renameMsg = renameMsg.arg( name );

            QAction* renameAction{ new QAction( renameMsg, this ) };

            menu.addAction( renameAction );

            QObject::connect( renameAction, &QAction::triggered, renameAction,
            [=, this]()
            {
                this->renameServerTabSlot( tabIndex );

                renameAction->disconnect();
                renameAction->deleteLater();
            }, Qt::UniqueConnection );

            QString closeMsg{ "Close [ %1 ]" };
                    closeMsg = closeMsg.arg( name );

            QAction* closeAction{ new QAction( closeMsg, this ) };

            menu.addAction( closeAction );
            QObject::connect( closeAction, &QAction::triggered, closeAction,
            [=, this]()
            {
                emit this->tabCloseRequested( tabIndex );

                closeAction->disconnect();
                closeAction->deleteLater();
            }, Qt::UniqueConnection );

            menu.exec( this->mapToGlobal( point ) );
        }
    }
}

void ReMixTabWidget::tabCloseRequestedSlot(const qint32& index)
{
    QWidget* widget{ this->widget( index ) };
    if ( widget != nullptr )
    {
        QString title{ "Close [ %1 ]:" };
        QString prompt{ "You are about to shut down your ReMix game server!\r\nThis will affect [ %1 ] "
                        "User(s) connected to it.\r\n\r\nAre you certain?" };

        for ( int i = 0; ( i < static_cast<int>( Globals::MAX_SERVER_COUNT ) )
           || ( i < serverMap.size() ); ++i )
        {
            const ReMixWidget* instance = serverMap.value( i );
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
                            qint32 servercount{ static_cast<qint32>( serverMap.size() ) };
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
        ReMix::updateTitleBars( server->getServer() );
}

void ReMixTabWidget::renameServerTabSlot(int index)
{
    ReMixWidget* tabA{ serverMap.value( index ) };
    if ( tabA != nullptr )
    {
        QString title{ "Rename Server: [ %1 ]" };
                title = title.arg( tabA->getServerName() );
        QString message{ "Please enter the new name you wish to use for this server!" };

        bool accepted{ false };
        QString response{ Helper::getTextResponse( this, title, message, tabA->getServerName(), &accepted, MessageBox::SingleLine ) };

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
}

void ReMixTabWidget::createServerAcceptedSlot(Server* server)
{
    if ( server == nullptr )
        return;

    QString serverName{ server->getServerName() };

    qint32 serverID{ 0 };

    for ( int i = 0; i < static_cast<int>( Globals::MAX_SERVER_COUNT ); ++i )
    {
        const ReMixWidget* instance{ serverMap.value( i ) };
        serverID = static_cast<int>( Globals::MAX_SERVER_COUNT ) + 1;

        if ( instance == nullptr )
        {
            serverID = i;
            break;
        }
    }

    if ( serverID <= static_cast<int>( Globals::MAX_SERVER_COUNT ) )
    {
        instanceCount += 1;
        serverMap.insert( serverID, new ReMixWidget( this, server ) );
        this->insertTab( static_cast<int>( serverMap.size() - 1 ), serverMap.value( serverID ), serverName );

        if ( serverID == 0 )
            currentChangedSlot( serverID );
        else
            this->setCurrentIndex( serverID );

        QObject::connect( serverMap.value( serverID ), &ReMixWidget::crossServerCommentSignal, this, &ReMixTabWidget::crossServerCommentSlot );
    }
}

void ReMixTabWidget::restartServerListSlot(const QStringList& restartList)
{
    if ( !restartList.isEmpty() )
    {
        Server* server{ nullptr };
        for ( const QString& name : restartList )
        {
            //Only Restart the server if it is enabled for the selected server.
            if ( Settings::getSetting( SKeys::Rules, SSubKeys::AutoRestart, name ).toBool() )
            {
                server = createDialog->loadOldServer( name );
                this->createServerAcceptedSlot( server );
            }
        }
    }
}

void ReMixTabWidget::crossServerCommentSlot(Server* server, const QString& comment)
{
    emit this->crossServerCommentSignal( server, comment);
}
