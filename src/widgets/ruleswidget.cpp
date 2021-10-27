
//Class includes.
#include "ruleswidget.hpp"
#include "ui_ruleswidget.h"

//ReMix includes.
#include "selectworld.hpp"
#include "settings.hpp"
#include "server.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QMutexLocker>
#include <QSettings>
#include <QtCore>
#include <QDir>

QHash<Server*, RulesWidget*> RulesWidget::ruleWidgets;
QMutex RulesWidget::mutex;

RulesWidget::RulesWidget() :
    ui(new Ui::RulesWidget)
{
    ui->setupUi(this);
}

RulesWidget::~RulesWidget()
{
    delete ui;
}

RulesWidget* RulesWidget::getInstance(Server* server)
{
    RulesWidget* widget{ ruleWidgets.value( server, nullptr ) };
    if ( widget == nullptr )
    {
        widget = new RulesWidget();
        if ( widget != nullptr )
        {
            ruleWidgets.insert( server, widget );
            widget->setServerName( server->getServerName() );
            QObject::connect( widget, &RulesWidget::setMaxIdleTimeSignal, server, &Server::setMaxIdleTimeSlot, Qt::UniqueConnection );
            QObject::connect( widget, &RulesWidget::gameInfoChangedSignal, server, &Server::gameInfoChangedSlot, Qt::UniqueConnection );
            QObject::connect( server, &Server::serverNameChangedSignal, widget, &RulesWidget::nameChangedSlot, Qt::UniqueConnection );
        }
    }
    return widget;
}

void RulesWidget::deleteInstance(Server* server)
{
    RulesWidget* widget{ ruleWidgets.take( server ) };
    if ( widget != nullptr )
    {
        widget->disconnect();
        widget->setParent( nullptr );
        widget->deleteLater();
    }
}

void RulesWidget::setServerName(const QString& name)
{
    //Load Rules from file.
    QVariant val;
    QString rowText{ "" };

    val = Settings::getSetting( SKeys::Rules, SSubKeys::World, name );
    this->setCheckedState( RToggles::WorldName, !val.toString().isEmpty() );

    rowText = "World Name: [ %1 ]";
    ui->rulesView->item( static_cast<int>( RToggles::WorldName ), 0 )->setText( rowText.arg( val.toString() ) );
    this->setGameInfo( val.toString() );

    rowText = "Max Idle: [ %1 ] Minutes";
    val = Settings::getSetting( SKeys::Rules, SSubKeys::MaxIdle, name );
    if ( !val.isValid() )
        val = static_cast<qint64>( Globals::MAX_IDLE_TIME ) / 1000 / 60;

    ui->rulesView->item( static_cast<int>( RToggles::MaxIdle ), 0 )->setText( rowText.arg( val.toUInt() ) );

    rowText = "Minimum Game Version: [ %1 ]";
    val = Settings::getSetting( SKeys::Rules, SSubKeys::MinVersion, name );
    ui->rulesView->item( static_cast<int>( RToggles::MinVersion ), 0 )->setText( rowText.arg( val.toString() ) );

    rowText = "Server Home: [ %1 ]";
    val = Settings::getSetting( SKeys::Rules, SSubKeys::SvrUrl, name );
    ui->rulesView->item( static_cast<int>( RToggles::UrlAddr ), 0 )->setText( rowText.arg( val.toString() ) );

    rowText = "Max Players: [ %1 ]";
    val = Settings::getSetting( SKeys::Rules, SSubKeys::MaxPlayers, name );

    if ( !val.isValid() )
        val = static_cast<quint32>( Globals::MAX_PLAYERS );

    ui->rulesView->item( static_cast<int>( RToggles::MaxPlayers ), 0 )->setText( rowText.arg( Helper::intToStr( val.toUInt() ) ) );

    this->setCheckedState( RToggles::MinVersion, !Settings::getSetting( SKeys::Rules, SSubKeys::MinVersion, name ).toString().isEmpty() );
    this->setCheckedState( RToggles::ServerPassword, Settings::getSetting( SKeys::Rules, SSubKeys::HasSvrPassword, name ).toBool() );
    this->setCheckedState( RToggles::UrlAddr, !Settings::getSetting( SKeys::Rules, SSubKeys::SvrUrl, name ).toString().isEmpty() );
    this->setCheckedState( RToggles::MaxPlayers, Settings::getSetting( SKeys::Rules, SSubKeys::MaxPlayers, name ).toUInt() != 0 );
    this->setCheckedState( RToggles::MaxIdle, Settings::getSetting( SKeys::Rules, SSubKeys::MaxIdle, name ).toUInt() != 0 );
    this->setCheckedState( RToggles::AutoRestart, Settings::getSetting( SKeys::Rules, SSubKeys::AutoRestart, name ).toBool() );
    this->setCheckedState( RToggles::NoEavesdrop, Settings::getSetting( SKeys::Rules, SSubKeys::NoEavesdrop, name ).toBool() );
    this->setCheckedState( RToggles::NoMigrate, Settings::getSetting( SKeys::Rules, SSubKeys::NoMigrate, name ).toBool() );
    this->setCheckedState( RToggles::NoModding, Settings::getSetting( SKeys::Rules, SSubKeys::NoModding, name ).toBool() );
    this->setCheckedState( RToggles::ArenaPK, Settings::getSetting( SKeys::Rules, SSubKeys::ArenaPK, name ).toBool() );
    this->setCheckedState( RToggles::Ladder, Settings::getSetting( SKeys::Rules, SSubKeys::PKLadder, name ).toBool() );
    this->setCheckedState( RToggles::NoBleep, Settings::getSetting( SKeys::Rules, SSubKeys::NoBleep, name ).toBool() );
    this->setCheckedState( RToggles::NoCheat, Settings::getSetting( SKeys::Rules, SSubKeys::NoCheat, name ).toBool() );
    this->setCheckedState( RToggles::NoPets, Settings::getSetting( SKeys::Rules, SSubKeys::NoPets, name ).toBool() );
    this->setCheckedState( RToggles::AllPK, Settings::getSetting( SKeys::Rules, SSubKeys::AllPK, name ).toBool() );
    this->setCheckedState( RToggles::NoPK, Settings::getSetting( SKeys::Rules, SSubKeys::NoPK, name ).toBool() );

    serverName = name;
}

void RulesWidget::setCheckedState(const RToggles& option, const bool& val)
{
    Qt::CheckState state;
    if ( val )
        state = Qt::Checked;
    else
        state = Qt::Unchecked;

    QTableWidgetItem* item{ ui->rulesView->item( static_cast<int>( option ), 0 ) };
    if ( item != nullptr )
    {
        ui->rulesView->item( static_cast<int>( option ), 0 )->setCheckState( state );
        stateMap.insert( item, state );
    }
}

bool RulesWidget::getCheckedState(const RToggles& option)
{
    return ui->rulesView->item( static_cast<int>( option ), 0 )->checkState() == Qt::Checked;
}

void RulesWidget::setGameInfo(const QString& gInfo)
{
    if ( !Helper::cmpStrings( gameInfo, gInfo) )
    {
        gameInfo = gInfo;
        emit this->gameInfoChangedSignal( gInfo );
    }
}

const QString& RulesWidget::getGameInfo() const
{
    return gameInfo;
}

void RulesWidget::on_rulesView_itemClicked(QTableWidgetItem* item)
{
    if ( item != nullptr )
    {
        if ( stateMap.value( item ) != item->checkState() )
        {
            if ( item->checkState() == Qt::Checked
              || item->checkState() == Qt::Unchecked )
            {
                this->toggleRules( item->row(), item->checkState() );
                stateMap.insert( item, item->checkState() );
            }
        }
    }
}

void RulesWidget::nameChangedSlot(const QString& name)
{
    this->setServerName( name );
}

void RulesWidget::on_rulesView_doubleClicked(const QModelIndex& index)
{
    int row = index.row();
    this->toggleRulesModel( static_cast<RToggles>( row ) );
}

void RulesWidget::toggleRulesModel(const RToggles &row)
{
    auto intRow{ static_cast<int>( row ) };

    QTableWidgetItem* item{ ui->rulesView->item( intRow, 0 ) };
    if ( item != nullptr )
    {
        Qt::CheckState val{ item->checkState() };
        Qt::CheckState state{ val == Qt::Checked ? Qt::Unchecked : Qt::Checked };

        item->setCheckState( state );
        stateMap.insert( item, item->checkState() );

        val = state;
        this->toggleRules( intRow, val );
    }
}

void RulesWidget::toggleRules(const qint32& row, const Qt::CheckState& value)
{
    bool state{ value == Qt::Checked };

    QString rowText{ "" };
    QString prompt{ "" };
    QString title{ "" };

    switch ( static_cast<RToggles>( row ) )
    {
        case RToggles::ServerPassword:
            {
                QString pwd{ Settings::getSetting( SKeys::Rules, SSubKeys::SvrPassword, serverName ).toString() };

                bool ok{ false };

                Settings::setSetting( state, SKeys::Rules, SSubKeys::HasSvrPassword, serverName );

                if ( state )
                {
                    if ( pwd.isEmpty() )
                    {
                        title = "Server Password:";
                        prompt = "Password:";
                        pwd = Helper::getTextResponse( this, title, prompt, "", &ok, MessageBox::SingleLine );
                        if ( !pwd.isEmpty() )
                            pwd = Helper::hashPassword( pwd );
                    }

                    if ( pwd.isEmpty() || !ok )
                    {
                        ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                        state = false;
                        pwd.clear();
                    }
                }
                else if ( !pwd.isEmpty() )
                    pwd.clear();

                Settings::setSetting( state, SKeys::Rules, SSubKeys::HasSvrPassword, serverName );
                Settings::setSetting( pwd, SKeys::Rules, SSubKeys::SvrPassword, serverName );
            }
        break;
        case RToggles::AutoRestart:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::AutoRestart, serverName );
        break;
        case RToggles::WorldName:
            {
                QString world{ Settings::getSetting( SKeys::Rules, SSubKeys::World, serverName ).toString() };
                bool ok{ false };

                QString worldDir{ Settings::getSetting( SKeys::Setting, SSubKeys::WorldDir ).toString() };
                if ( !worldDir.isEmpty() )
                {
                    SelectWorld* selWorld = new SelectWorld( this );
                    selWorld->setRequireWorld( !world.isEmpty() );

                    QObject::connect( selWorld, &SelectWorld::finished, selWorld, [=, &world]()
                    {
                        world = selWorld->getSelectedWorld();
                        selWorld->close();
                        selWorld->deleteLater();
                    } );
                    selWorld->exec();

                    state = true;
                    if ( world.isEmpty() )
                        state = false;
                }
                else
                {
                    if ( state )
                    {
                        if ( world.isEmpty() )
                        {
                            title = "Server World:";
                            prompt = "World:";
                            world = Helper::getTextResponse( this, title, prompt, "", &ok, MessageBox::SingleLine );
                        }

                        if ( world.isEmpty() || !ok )
                            state = false;

                    }
                    else if ( !world.isEmpty() )
                    {
                        world = "";
                        state = false;
                    }
                }

                ui->rulesView->item( row, 0 )->setCheckState( state == true ? Qt::Checked : Qt::Unchecked );
                rowText = "World Name: [ %1 ]";
                if ( world.isEmpty() )
                    rowText = rowText.arg( "Not Selected" );
                else
                    rowText = rowText.arg( world );

                ui->rulesView->item( static_cast<int>( RToggles::WorldName ), 0 )->setText( rowText );
                Settings::setSetting( world, SKeys::Rules, SSubKeys::World, serverName );

                this->setGameInfo( world );
            }
        break;
        case RToggles::UrlAddr:
            {
                QString url{ Settings::getSetting( SKeys::Rules, SSubKeys::SvrUrl, serverName ).toString() };
                bool ok{ false };

                if ( state )
                {
                    if ( url.isEmpty() )
                    {
                        title = "Server URL:";
                        prompt = "URL:";
                        url = Helper::getTextResponse( this, title, prompt, "", &ok, MessageBox::SingleLine );
                    }

                    if ( url.isEmpty() || !ok )
                    {
                        ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                        state = false;
                    }
                    else
                        Settings::setSetting( url, SKeys::Rules, SSubKeys::SvrUrl, serverName );
                }
                else if ( !url.isEmpty() )
                {
                    ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                    Settings::setSetting( "", SKeys::Rules, SSubKeys::SvrUrl, serverName );
                }

                rowText = "Server Home: [ %1 ]";
                rowText = rowText.arg( Settings::getSetting( SKeys::Rules, SSubKeys::SvrUrl, serverName ).toString() );
                ui->rulesView->item( row, 0 )->setText( rowText );
            }
        break;
        case RToggles::AllPK:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::AllPK, serverName );
            if ( state )
            {
                if ( getCheckedState( RToggles::NoPK ) )
                    this->toggleRulesModel( RToggles::NoPK );

                if ( getCheckedState( RToggles::ArenaPK ) )
                    this->toggleRulesModel( RToggles::ArenaPK );
            }
        break;
        case RToggles::MaxPlayers:
            {
                quint32 maxPlrs{ Settings::getSetting( SKeys::Rules, SSubKeys::MaxPlayers, serverName ).toUInt() };
                bool ok{ false };

                if (( maxPlrs == 0 )
                   || state )
                {
                    if ( maxPlrs == 0 )
                    {
                        title = "Max-Players:";
                        prompt = "Value:";
                        maxPlrs = Helper::getTextResponse( this, title, prompt, "", &ok, MessageBox::SingleLine ).toUInt();
                    }

                    if ( maxPlrs == 0 || !ok )
                        ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );

                    Settings::setSetting( maxPlrs, SKeys::Rules, SSubKeys::MaxPlayers, serverName );
                }
                else if ( maxPlrs > 0 )
                {
                    maxPlrs = 0;
                    ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                    Settings::setSetting( "", SKeys::Rules, SSubKeys::MaxPlayers, serverName );
                }

                rowText = "Max Players: [ %1 ]";
                if ( maxPlrs > 0 )
                    rowText = rowText.arg( maxPlrs );
                else
                    rowText = rowText.arg( static_cast<int>( Globals::MAX_PLAYERS ) );

                ui->rulesView->item( row, 0 )->setText( rowText );
            }
        break;
        case RToggles::MaxIdle:
            {
                quint32 maxIdle{ Settings::getSetting( SKeys::Rules, SSubKeys::MaxIdle, serverName ).toUInt() };
                bool ok{ false };

                if (( Settings::getSetting( SKeys::Rules, SSubKeys::MaxIdle, serverName ).toUInt() == 0 )
                   && state )
                {
                    if ( maxIdle == 0 )
                    {
                        title = "Max-Idle:";
                        prompt = "Value:";
                        maxIdle = Helper::getTextResponse( this, title, prompt, "", &ok, MessageBox::SingleLine ).toUInt();
                    }

                    if ( maxIdle == 0 || !ok )
                        ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );

                    Settings::setSetting( maxIdle, SKeys::Rules, SSubKeys::MaxIdle, serverName );
                }
                else if ( maxIdle != 0 )
                {
                    ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                    Settings::setSetting( 0, SKeys::Rules, SSubKeys::MaxIdle, serverName );
                }

                rowText = "Max Idle: [ %1 ] Minutes";
                QVariant val{ Settings::getSetting( SKeys::Rules, SSubKeys::MaxIdle, serverName ) };
                if ( !val.isValid() )
                    val = static_cast<qint64>( static_cast<int>( Globals::MAX_IDLE_TIME ) ) / 1000 / 60;

                rowText = rowText.arg( val.toUInt() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                //Max Idle Duration has changed, update all Players.
                emit this->setMaxIdleTimeSignal();
            }
        break;
        case RToggles::MinVersion:
            {
                QString version{ Settings::getSetting( SKeys::Rules, SSubKeys::MinVersion, serverName ).toString() };
                bool ok{ false };

                if ( Settings::getSetting( SKeys::Rules, SSubKeys::MinVersion, serverName ).toString().isEmpty()
                  && state )
                {
                    if ( version.isEmpty() )
                    {
                        title = "Minimum Game Version:";
                        prompt = "Version:";
                        version = Helper::getTextResponse( this, title, prompt, "", &ok, MessageBox::SingleLine );
                    }

                    if ( version.isEmpty() || !ok )
                        ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                    else
                        Settings::setSetting( version, SKeys::Rules, SSubKeys::MinVersion, serverName );
                }
                else if ( !version.isEmpty() )
                {
                    ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                    Settings::setSetting( "", SKeys::Rules, SSubKeys::MinVersion, serverName );
                }
                rowText = "Minimum Game Version: [ %1 ]";
                rowText = rowText.arg( Settings::getSetting( SKeys::Rules, SSubKeys::MinVersion, serverName ).toString() );
                ui->rulesView->item( row, 0 )->setText( rowText );
            }
        break;
        case RToggles::Ladder:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::PKLadder, serverName );
        break;
        case RToggles::NoBleep:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::NoBleep, serverName );
        break;
        case RToggles::NoCheat:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::NoCheat, serverName );
        break;
        case RToggles::NoEavesdrop:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::NoEavesdrop, serverName );
        break;
        case RToggles::NoMigrate:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::NoMigrate, serverName );
        break;
        case RToggles::NoModding:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::NoModding, serverName );
        break;
        case RToggles::NoPets:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::NoPets, serverName );
        break;
        case RToggles::NoPK:
            {
                Settings::setSetting( state, SKeys::Rules, SSubKeys::NoPK, serverName );
                if ( state )
                {
                    if ( getCheckedState( RToggles::AllPK ) )
                        this->toggleRulesModel( RToggles::AllPK );
                }
            }
        break;
        case RToggles::ArenaPK:
            {
                Settings::setSetting( state, SKeys::Rules, SSubKeys::ArenaPK, serverName );
                if ( state )
                {
                    if ( getCheckedState( RToggles::AllPK ) )
                        this->toggleRulesModel( RToggles::AllPK );
                }
            }
        break;
    }
}
