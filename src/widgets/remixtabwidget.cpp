
#include "includes.hpp"
#include "remixtabwidget.hpp"

ReMixTabWidget::ReMixTabWidget(QWidget* parent, User* usr, QStringList* argList)
    : QTabWidget(parent)
{
    user = usr;
    if ( usr == nullptr )
        user = new User( this );

    this->createTabButtons();

    quint32 serverID = this->count();
    servers[ serverID ] = new ReMixWidget( this, user, argList );
    this->insertTab( serverID, servers[ serverID ],
                     servers[ serverID ]->getServerName() );

    this->setTabsClosable( true );

    instanceCount = 1; //Automatically create a server instance at start.
    QObject::connect( this, &QTabWidget::tabCloseRequested,
                      this, &ReMixTabWidget::tabCloseRequestedSlot );

    this->connectNameChange( serverID );

    //Hide Tab-specific UI dialog windows when the tabs change.
    QObject::connect( this, &QTabWidget::currentChanged,
                      this, &ReMixTabWidget::currentChangedSlot );

    defaultPalette = parent->palette();
}

ReMixTabWidget::~ReMixTabWidget()
{
    nightModeButton->deleteLater();
    newTabButton->deleteLater();

    ReMixWidget* server{ nullptr };
    for ( int i = 0; i < MAX_SERVER_COUNT; ++i )
    {
        server = servers[ i ];
        if ( server != nullptr )
            server->deleteLater();
    }
}

void ReMixTabWidget::sendMultiServerMessage(QString msg, Player* plr,
                                            bool toAll)
{
    ReMixWidget* server{ nullptr };
    for ( int i = 0; i < MAX_SERVER_COUNT; ++i )
    {
        server = servers[ i ];
        if ( server != nullptr )
            server->sendServerMessage( msg, plr, toAll );
    }
}

void ReMixTabWidget::connectNameChange(quint32 id)
{
    ReMixWidget* server{ servers[ id ] };
    if ( server == nullptr )
        return;

    QObject::connect( servers[ id ], &ReMixWidget::serverNameChanged,
                      this, [=](const QString& name)
    {
        this->setTabText( id, name );
    } );
}

quint32 ReMixTabWidget::getPlayerCount()
{
    quint32 playerCount{ 0 };

    ReMixWidget* server{ nullptr };
    for ( int i = 0; i < MAX_SERVER_COUNT; ++i )
    {
        server = servers[ i ];
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
        server = servers[ i ];
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

void ReMixTabWidget::createTabButtons()
{
    newTabButton = new QToolButton( this );
    newTabButton->setIcon( QIcon( ":/icon/newtab.png" ) );
    newTabButton->setCursor( Qt::ArrowCursor );
    newTabButton->setAutoRaise( true );

    this->setCornerWidget( newTabButton, Qt::TopLeftCorner );
    QObject::connect( newTabButton, &QToolButton::clicked, [=]()
    {
        if ( user == nullptr )
            return;

        CreateInstance* createDialog{ new CreateInstance( this ) };
        QObject::connect( createDialog, &CreateInstance::accepted,
                          [=]()
        {
            QStringList svrArgs = createDialog->getServerArgs().split( "/" );
            if ( svrArgs.isEmpty() )
            {
                svrArgs << "/game=WoS"
                        << "/fudge"
                        << "/name=Well of Lost Souls ReMix";
            }

            quint32 serverID{ 0 };
            ReMixWidget* instance{ nullptr };
            for ( int i = 0; i < MAX_SERVER_COUNT; ++i )
            {
                instance = servers[ i ];
                if ( instance == nullptr )
                {
                    serverID = i;
                    break;
                }
                else
                    serverID = MAX_SERVER_COUNT + 1;
            }

            if ( this->count() <= MAX_SERVER_COUNT )
            {
                instanceCount += 1;
                servers[ serverID ] = new ReMixWidget( this, user, &svrArgs,
                                                       QString::number( serverID ) );
                this->insertTab( serverID, servers[ serverID ],
                                 servers[ serverID ]->getServerName() );

                this->connectNameChange( serverID );
            }
            createDialog->close();
            createDialog->disconnect();
            createDialog->deleteLater();
        });
    } );

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

void ReMixTabWidget::applyThemes(qint32 type)
{
    QPalette palette;
    if ( type == Themes::DARK )
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
    else
        palette = defaultPalette;

    qApp->setPalette( palette );
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

        for ( int i = 0; i < MAX_SERVER_COUNT; ++i )
        {
            instance = servers[ i ];
            if ( instance != nullptr )
            {
                if ( widget == instance )
                {
                    title = title.arg( instance->getServerName() );
                    prompt = prompt.arg( instance->getPlayerCount() );

                    instance->sendServerMessage( "The admin is taking this "
                                                 "server down...", nullptr,
                                                 true );

                    //Last server instance is being closed. Prompt User.
                    if ( Helper::confirmAction( this, title, prompt ) )
                    {
                        //Last Server instance. Close ReMix.
                        instanceCount -= 1;
                        if ( instanceCount == 0 )
                            qApp->quit();

                        servers[ i ] = nullptr;

                        instance->close();
                        instance->deleteLater();

                        this->removeTab( index );

                        break;
                    }
                    else
                    {
                        instance->sendServerMessage( "The admin changed his"
                                                     " or her mind! (yay!)"
                                                     "...", nullptr, true );
                    }
                }
            }
        }
    }
}

void ReMixTabWidget::currentChangedSlot(quint32 newTab)
{
    quint32 prevTab{ this->getPrevTabIndex() };

    ReMixWidget* prevServer{ servers[ prevTab ] };
    if ( prevServer == nullptr )
        return;

    Settings* settings{ prevServer->getSettings() };
    Server* tcpServer{ prevServer->getTcpServer() };

    Comments* comments{ nullptr };
    bool showingPrevSettings{ false };
    bool showingPrevComments{ false };

    //Hide the Settings dialog if it is are opened.
    if ( settings != nullptr )
    {
        if ( !settings->isHidden() )
        {
            showingPrevSettings = true;
            settings->hide();
        }
    }

    //Hide the Comments dialog if it is are opened.
    if ( tcpServer != nullptr )
    {
        comments = tcpServer->getServerComments();
        if ( comments != nullptr )
        {
            if ( !comments->isHidden() )
            {
                showingPrevComments = true;
                comments->hide();
            }
        }
    }

    //Show the dialogs for the newly-opened Server Tab.
    //(Only if they were open on the previous tab.)
    ReMixWidget* server{ servers[ newTab ] };
    if ( server != nullptr )
    {
        settings = server->getSettings();
        tcpServer = server->getTcpServer();

        //Open the Settings dialog if it was are opened.
        if ( settings != nullptr )
        {
            if ( settings->isHidden() && showingPrevSettings )
                settings->show();
        }

        //Open the Comments dialog if it was are opened.
        if ( tcpServer != nullptr )
        {
            comments = tcpServer->getServerComments();
            if ( comments != nullptr )
            {
                if ( comments->isHidden() && showingPrevComments )
                    comments->show();
            }
        }
    }
    this->setPrevTabIndex( newTab );

}
