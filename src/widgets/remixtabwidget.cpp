
#include "includes.hpp"
#include "remixtabwidget.hpp"

ReMixTabWidget::ReMixTabWidget(QWidget* parent, User* usr, QStringList* argList)
    : QTabWidget(parent)
{
    user = usr;
    if ( usr == nullptr )
        user = new User( this );

    newTabButton = new QToolButton( this );
    newTabButton->setCursor( Qt::ArrowCursor );
    newTabButton->setAutoRaise( true );
    //newTabButton->setIcon( QIcon( ImageLocation
    //                            % "addtab.png" ) );

    this->setCornerWidget( newTabButton );

    QObject::connect( newTabButton, &QToolButton::clicked,
                      this, &ReMixTabWidget::newTab );

    quint32 serverID = this->count();
    servers[ serverID ] = new ReMixWidget( this, user, argList );
    this->insertTab( serverID, servers[ serverID ],
                     servers[ serverID ]->getServerName() );

    this->connectNameChange( serverID );

    //Hide Tab-specific UI dialog windows when the tabs change.
    //Also re-open the closed dialogs for the new tab.
    QObject::connect( this, &QTabWidget::currentChanged, [=](quint32 id)
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

        if ( settings != nullptr )
        {
            if ( !settings->isHidden() )
            {
                showingPrevSettings = true;
                settings->hide();
            }
        }

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

        ReMixWidget* server{ servers[ id ] };
        if ( server != nullptr )
        {
            settings = server->getSettings();
            tcpServer = server->getTcpServer();

            Comments* comments{ nullptr };
            if ( settings != nullptr )
            {
                if ( settings->isHidden() && showingPrevSettings )
                    settings->show();
            }

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
        this->setPrevTabIndex( id );
    } );
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

void ReMixTabWidget::newTab()
{
    if ( user == nullptr )
        return;

    QStringList list1{"/game=WoS", "/fudge",
                     "/name=Well of Lost Souls ReMix1" };

    quint32 serverID = this->count();
    if ( this->count() <= MAX_SERVER_COUNT )
    {
        servers[ serverID ] = new ReMixWidget( this, user, &list1,
                                               QString::number( serverID ) );
        this->insertTab( serverID, servers[ serverID ],
                         servers[ serverID ]->getServerName() );

        this->connectNameChange( serverID );
    }
}
