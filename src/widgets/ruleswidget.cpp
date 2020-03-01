
//Class includes.
#include "ruleswidget.hpp"
#include "ui_ruleswidget.h"

//ReMix includes.
#include "selectworld.hpp"
#include "serverinfo.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "rules.hpp"

//Qt Includes.
#include <QSettings>
#include <QtCore>
#include <QDir>

QHash<ServerInfo*, RulesWidget*> RulesWidget::ruleWidgets;

RulesWidget::RulesWidget() :
    ui(new Ui::RulesWidget)
{
    ui->setupUi(this);
}

RulesWidget::~RulesWidget()
{
    delete ui;
}

RulesWidget* RulesWidget::getWidget(ServerInfo* server)
{
    RulesWidget* widget{ ruleWidgets.value( server ) };
    if ( widget == nullptr )
    {
        widget = new RulesWidget();
        ruleWidgets.insert( server, widget );
    }
    return widget;
}

void RulesWidget::deleteWidget(ServerInfo* server)
{
    RulesWidget* widget{ ruleWidgets.take( server ) };
    if ( widget != nullptr )
    {
        widget->setParent( nullptr );
        widget->deleteLater();
    }
}

void RulesWidget::setServerName(const QString& name)
{
    //Load Rules from file.
    QString rowText{ "" };
    worldCheckState = !Rules::getWorldName( name ).isEmpty();
    this->setCheckedState( Toggles::world,
                           worldCheckState );

    rowText = "World Name: [ %1 ]";
    rowText = rowText.arg( Rules::getWorldName( name ) );
    ui->rulesView->item( Toggles::world, 0 )->setText( rowText );
    emit this->gameInfoChanged( Rules::getWorldName( name ) );

    urlCheckState = !Rules::getURLAddress( name ).isEmpty();
    this->setCheckedState( Toggles::url, urlCheckState );

    rowText = "Server Home: [ %1 ]";
    rowText = rowText.arg( Rules::getURLAddress( name ) );
    ui->rulesView->item( Toggles::url, 0 )->setText( rowText );

    this->setCheckedState( Toggles::allPK,
                           Rules::getAllPKing( name ) );

    maxPlayersCheckState = !Rules::getRequireMaxPlayers( name );
    this->setCheckedState( Toggles::maxP, maxPlayersCheckState );

    rowText = "Max Players: [ %1 ]";
    rowText = rowText.arg( Rules::getMaxPlayers( name ) );
    ui->rulesView->item( Toggles::maxP, 0 )->setText( rowText );

    maxAFKCheckState = !Rules::getRequireMaxAFK( name );
    this->setCheckedState( Toggles::maxAFK, maxAFKCheckState );

    rowText = "Max AFK: [ %1 ] Minutes";
    rowText = rowText.arg( Rules::getMaxAFK( name ) );
    ui->rulesView->item( Toggles::maxAFK, 0 )->setText( rowText );

    minVersionCheckState = !Rules::getMinVersion( name ).isEmpty();
    this->setCheckedState( Toggles::minV, minVersionCheckState );

    rowText = "Min Version: [ %1 ]";
    rowText = rowText.arg( Rules::getMinVersion( name ) );
    ui->rulesView->item( Toggles::minV, 0 )->setText( rowText );

    this->setCheckedState( Toggles::ladder,
                           Rules::getReportLadder( name ) );

    this->setCheckedState( Toggles::noBleep,
                           Rules::getNoCursing( name ));

    this->setCheckedState( Toggles::noCheat,
                           Rules::getNoCheating( name ) );

    this->setCheckedState( Toggles::noEavesdrop,
                           Rules::getNoEavesdropping( name ) );

    this->setCheckedState( Toggles::noMigrate,
                           Rules::getNoMigrating( name ) );

    this->setCheckedState( Toggles::noMod,
                           Rules::getNoModding( name ) );

    this->setCheckedState( Toggles::noPets,
                           Rules::getNoPets( name ) );

    this->setCheckedState( Toggles::noPK,
                           Rules::getNoPKing( name ) );

    this->setCheckedState( Toggles::arenaPK,
                           Rules::getArenaPKing( name ) );

    serverName = name;
}

void RulesWidget::setCheckedState(const Toggles& option, const bool& val)
{
    Qt::CheckState state;
    if ( val )
        state = Qt::Checked;
    else
        state = Qt::Unchecked;

    ui->rulesView->item( option, 0 )->setCheckState( state );
}

void RulesWidget::setSelectedWorld(const QString& worldName, const bool& state)
{
    QString rowText{ "World Name: [ %1 ]" };
    if ( worldName.isEmpty() )
        rowText = rowText.arg( "Not Selected" );
    else
        rowText = rowText.arg( worldName );

    ui->rulesView->item( Toggles::world, 0 )->setText( rowText );
    ui->rulesView->item( Toggles::world, 0 )->setCheckState( state
                                                               ? Qt::Checked
                                                               : Qt::Unchecked );

    worldCheckState = state;
    Rules::setWorldName( worldName, serverName );
}

void RulesWidget::on_rulesView_itemClicked(QTableWidgetItem* item)
{
    if ( item != nullptr )
    {
        if ( item->checkState() == Qt::Checked
          || item->checkState() == Qt::Unchecked )
        {
            this->toggleRules( item->row(), item->checkState() );
        }
    }
}

void RulesWidget::on_rulesView_doubleClicked(const QModelIndex& index)
{
    int row = index.row();

    Qt::CheckState val = ui->rulesView->item( row, 0 )->checkState();
    ui->rulesView->item( row, 0 )->setCheckState( val == Qt::Checked
                                                      ? Qt::Unchecked
                                                      : Qt::Checked );

    val = ui->rulesView->item( row, 0 )->checkState();
    this->toggleRules( row, val );
}

void RulesWidget::toggleRules(const qint32& row, const Qt::CheckState& value)
{
    bool state{ value == Qt::Checked };

    QSettings rules( "preferences.ini", QSettings::IniFormat );
              rules.beginGroup( serverName % "/" % Settings::keys[ Settings::Rules ] );

    QString prompt{ "" };
    QString title{ "" };

    bool removeKey{ false };
    qint32 key{ row };

    QString rowText{ "" };

    switch ( key )
    {
        case Toggles::world:
            {
                QString world{ Rules::getWorldName( serverName ) };
                bool ok{ false };

                if ( state != worldCheckState )
                {
                    QString worldDir{ Settings::getWorldDir() };
                    if ( !worldDir.isEmpty() )
                    {
                        selectWorld = new SelectWorld( this );
                        selectWorld->setRequireWorld( !world.isEmpty() );
                        QObject::connect( selectWorld, &SelectWorld::accepted,
                        [&world, this]()
                        {
                            world = selectWorld->getSelectedWorld();

                            selectWorld->close();
                            selectWorld->disconnect();
                            selectWorld->deleteLater();
                        });
                        selectWorld->exec();

                        state = true;
                        if ( world.isEmpty() )
                        {
                            state = false;
                            removeKey = true;
                        }
                    }
                    else
                    {
                        if (( Rules::getRequireWorld( serverName )
                          || !worldCheckState )
                          && state )
                        {
                            if ( world.isEmpty() )
                            {
                                title = "Server World:";
                                prompt = "World:";
                                world = Helper::getTextResponse( this, title, prompt, "", &ok, 0 );
                            }

                            if ( !world.isEmpty() && !ok )
                            {
                                ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                                state = false;
                            }
                        }
                        else if ( !Rules::getRequireWorld( serverName )
                               && !world.isEmpty() )
                        {
                            title = "Remove World:";
                            prompt = "Do you wish to erase the stored World Name?";

                            if ( !Helper::confirmAction( this, title, prompt ) )
                            {
                                ui->rulesView->item( row, 0 )->setCheckState( Qt::Checked );
                                state = true;
                            }
                            else
                            {
                                removeKey = true;
                                world = "";
                            }
                        }
                    }
                    this->setSelectedWorld( world, state );
                    emit this->gameInfoChanged( world );
                }
            }
        break;
        case Toggles::url:
            {
                QString url{ Rules::getURLAddress( serverName ) };
                bool ok{ false };

                if ( state != urlCheckState )
                {
                    if (( Rules::getRequireURL( serverName )
                       || !urlCheckState )
                      && state )
                    {
                        if ( url.isEmpty() )
                        {
                            title = "Server URL:";
                            prompt = "URL:";
                            url = Helper::getTextResponse( this, title, prompt, "", &ok, 0 );
                        }

                        if ( url.isEmpty() && !ok  )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                            state = false;
                        }
                        else
                            Rules::setURLAddress( url, serverName );
                    }
                    else if ( !Rules::getRequireURL( serverName )
                           && !url.isEmpty() )
                    {
                        title = "Remove URL:";
                        prompt = "Do you wish to erase the stored URL Address?";

                        if ( !Helper::confirmAction( this, title, prompt ) )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Checked );
                            state = true;
                        }
                        else
                            rules.remove( Rules::subKeys[ Rules::url ] );
                    }
                }
                rowText = "Server Home: [ %1 ]";
                rowText = rowText.arg( Rules::getURLAddress( serverName ) );
                ui->rulesView->item( row, 0 )->setText( rowText );

                urlCheckState = state;
            }
        break;
        case Toggles::allPK:
            if ( !state )
                removeKey = true;
            else
                Rules::setAllPKing( state, serverName );
        break;
        case Toggles::maxP:
            {
                quint32 maxPlrs{ Rules::getMaxPlayers( serverName ) };
                bool ok{ false };

                if ( state != maxPlayersCheckState )
                {
                    if (( Rules::getRequireMaxPlayers( serverName )
                       || !maxPlayersCheckState )
                      && state )
                    {
                        if ( maxPlrs == 0 )
                        {
                            title = "Max-Players:";
                            prompt = "Value:";
                            maxPlrs = Helper::getTextResponse( this, title, prompt, "", &ok, 0 )
                                                     .toUInt();
                        }

                        if ( maxPlrs == 0 && !ok  )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                            state = false;
                        }
                        else
                            Rules::setMaxPlayers( maxPlrs, serverName );
                    }
                    else if ( !Rules::getRequireMaxPlayers( serverName )
                           && maxPlrs != 0 )
                    {
                        title = "Remove Max-Players:";
                        prompt = "Do you wish to erase the stored Max-Players Value?";

                        if ( !Helper::confirmAction( this, title, prompt ) )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Checked );
                            state = true;
                        }
                        else
                            rules.remove( Rules::subKeys[ Rules::maxP ] );
                    }
                }
                rowText = "Max Players: [ %1 ]";
                rowText = rowText.arg( Rules::getMaxPlayers( serverName ) );
                ui->rulesView->item( row, 0 )->setText( rowText );

                maxPlayersCheckState = state;
            }
        break;
        case Toggles::maxAFK:
            {
                quint32 maxAFK{ Rules::getMaxAFK( serverName ) };
                bool ok{ false };

                if ( state != maxAFKCheckState )
                {
                    if (( Rules::getRequireMaxAFK( serverName )
                       || !maxAFKCheckState )
                      && state )
                    {
                        if ( maxAFK == 0 )
                        {
                            title = "Max-AFK:";
                            prompt = "Value:";
                            maxAFK = Helper::getTextResponse( this, title, prompt, "", &ok, 0 )
                                                    .toUInt();
                        }

                        if ( maxAFK == 0 && !ok  )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                            state = false;
                        }
                        else
                            Rules::setMaxAFK( maxAFK, serverName );
                    }
                    else if ( !Rules::getRequireMaxAFK( serverName )
                           || maxAFK == 0 )
                    {
                        title = "Remove Max-AFK:";
                        prompt = "Do you wish to erase the stored Max-AFK Value?";

                        if ( !Helper::confirmAction( this, title, prompt ) )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Checked );
                            state = true;
                        }
                        else
                            rules.remove( Rules::subKeys[ Rules::maxAFK ] );
                    }
                }
                rowText = "Max AFK: [ %1 ] Minutes";
                rowText = rowText.arg( Rules::getMaxAFK( serverName ) );
                ui->rulesView->item( row, 0 )->setText( rowText );

                maxAFKCheckState = state;
            }
        break;
        case Toggles::minV:
            {
                QString version{ Rules::getMinVersion( serverName ) };
                bool ok{ false };

                if ( state != minVersionCheckState )
                {
                    if (( Rules::getRequireMinVersion( serverName )
                       || !minVersionCheckState)
                      && state )
                    {
                        if ( version.isEmpty() )
                        {
                            title = "Min-Version:";
                            prompt = "Version:";
                            version = Helper::getTextResponse( this, title, prompt, "", &ok, 0 );
                        }

                        if ( version.isEmpty() && !ok  )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                            state = false;
                        }
                        else
                            Rules::setMinVersion( version, serverName );
                    }
                    else if ( !Rules::getRequireMinVersion( serverName )
                           && !version.isEmpty() )
                    {
                        title = "Remove Min-Version:";
                        prompt = "Do you wish to erase the stored Min-Version?";

                        if ( !Helper::confirmAction( this, title, prompt ) )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Checked );
                            state = true;
                        }
                        else
                            rules.remove( Rules::subKeys[ Rules::minV ] );
                    }
                }
                rowText = "Min Version: [ %1 ]";
                rowText = rowText.arg( Rules::getMinVersion( serverName ) );
                ui->rulesView->item( row, 0 )->setText( rowText );

                minVersionCheckState = state;
            }
        break;
        case Toggles::ladder:
            if ( !state )
                removeKey = true;
            else
                Rules::setReportLadder( state, serverName );
        break;
        case Toggles::noBleep:
            if ( !state )
                removeKey = true;
            else
                Rules::setNoCursing( state, serverName );
        break;
        case Toggles::noCheat:
            if ( !state )
                removeKey = true;
            else
                Rules::setNoCheating( state, serverName );
        break;
        case Toggles::noEavesdrop:
            if ( !state )
                removeKey = true;
            else
                Rules::setNoEavesdropping( state, serverName );
        break;
        case Toggles::noMigrate:
            if ( !state )
                removeKey = true;
            else
                Rules::setNoMigrating( state, serverName );
        break;
        case Toggles::noMod:
            if ( !state )
                removeKey = true;
            else
                Rules::setNoModding( state, serverName );
        break;
        case Toggles::noPets:
            if ( !state )
                removeKey = true;
            else
                Rules::setNoPets( state, serverName );
        break;
        case Toggles::noPK:
            if ( !state )
                removeKey = true;
            else
                Rules::setNoPKing( state, serverName );
        break;
        case Toggles::arenaPK:
            if ( !state )
                removeKey = true;
            else
                Rules::setArenaPKing( state, serverName );
         break;
        default:
            qDebug() << "Unknown Rule, doing nothing!";
        break;
    }

    if ( removeKey )
        rules.remove( Rules::subKeys.at( key ) );
}
