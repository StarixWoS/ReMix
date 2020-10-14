
//Class includes.
#include "ruleswidget.hpp"
#include "ui_ruleswidget.h"

//ReMix includes.
#include "selectworld.hpp"
#include "serverinfo.hpp"
#include "settings.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QMutexLocker>
#include <QSettings>
#include <QtCore>
#include <QDir>

QHash<ServerInfo*, RulesWidget*> RulesWidget::ruleWidgets;
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

RulesWidget* RulesWidget::getWidget(ServerInfo* server)
{
    RulesWidget* widget{ ruleWidgets.value( server ) };
    if ( widget == nullptr )
    {
        widget = new RulesWidget();
        if ( widget != nullptr )
        {
            ruleWidgets.insert( server, widget );
            QObject::connect( widget, &RulesWidget::setMaxIdleTimeSignal, server, &ServerInfo::setMaxIdleTimeSlot );
        }
    }
    return widget;
}

void RulesWidget::deleteWidget(ServerInfo* server)
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
    worldCheckState = !val.toString().isEmpty();
    this->setCheckedState( RToggles::WorldName, worldCheckState );

    rowText = "World Name: [ %1 ]";
    ui->rulesView->item( static_cast<int>( RToggles::WorldName ), 0 )->setText( rowText.arg( val.toString() ) );
    this->setGameInfo( val.toString() );

    rowText = "Max Idle: [ %1 ] Minutes";
    val = Settings::getSetting( SKeys::Rules, SSubKeys::MaxIdle, name );
    if ( !val.isValid() )
        val = static_cast<qint64>( MAX_IDLE_TIME ) / 1000 / 60;

    ui->rulesView->item( static_cast<int>( RToggles::MaxIdle ), 0 )->setText( rowText.arg( val.toUInt() ) );

    rowText = "Minimum Game Version: [ %1 ]";
    val = Settings::getSetting( SKeys::Rules, SSubKeys::MinVersion, name );
    ui->rulesView->item( static_cast<int>( RToggles::MinVersion ), 0 )->setText( rowText.arg( val.toString() ) );

    rowText = "Server Home: [ %1 ]";
    val = Settings::getSetting( SKeys::Rules, SSubKeys::SvrUrl, name );
    ui->rulesView->item( static_cast<int>( RToggles::UrlAddr ), 0 )->setText( rowText.arg( val.toString() ) );

    rowText = "Max Players: [ %1 ]";
    val = Settings::getSetting( SKeys::Rules, SSubKeys::MaxPlayers, name );
    ui->rulesView->item( static_cast<int>( RToggles::MaxPlayers ), 0 )->setText( rowText.arg( val.toUInt() ) );


    minVersionCheckState = !Settings::getSetting( SKeys::Rules, SSubKeys::MinVersion, name ).toString().isEmpty();
    this->setCheckedState( RToggles::MinVersion, minVersionCheckState );

    maxPlayersCheckState = Settings::getSetting( SKeys::Rules, SSubKeys::MaxPlayers, name ).toUInt() != 0;
    this->setCheckedState( RToggles::MaxPlayers, maxPlayersCheckState );

    urlCheckState = !Settings::getSetting( SKeys::Rules, SSubKeys::SvrUrl, name ).toString().isEmpty();
    this->setCheckedState( RToggles::UrlAddr, urlCheckState );

    pwdCheckState = Settings::getSetting( SKeys::Rules, SSubKeys::HasSvrPassword, name ).toBool();
    this->setCheckedState( RToggles::ServerPassword, pwdCheckState );

    maxIdleCheckState = Settings::getSetting( SKeys::Rules, SSubKeys::MaxIdle, name ).toUInt() != 0;
    this->setCheckedState( RToggles::MaxIdle, maxIdleCheckState );

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

    ui->rulesView->item( static_cast<int>( option ), 0 )->setCheckState( state );
}

bool RulesWidget::getCheckedState(const RToggles& option)
{
    return ui->rulesView->item( static_cast<int>( option ), 0 )->checkState() == Qt::Checked;
}

void RulesWidget::setSelectedWorld(const QString& worldName, const bool& state)
{
    QString rowText{ "World Name: [ %1 ]" };
    if ( worldName.isEmpty() )
        rowText = rowText.arg( "Not Selected" );
    else
        rowText = rowText.arg( worldName );

    ui->rulesView->item( static_cast<int>( RToggles::WorldName ), 0 )->setText( rowText );
    ui->rulesView->item( static_cast<int>( RToggles::WorldName ), 0 )->setCheckState( state ? Qt::Checked : Qt::Unchecked );

    worldCheckState = state;
    Settings::setSetting( worldName, SKeys::Rules, SSubKeys::World, serverName );
}

void RulesWidget::setGameInfo(const QString& gInfo)
{
    gameInfo = gInfo;
}

const QString& RulesWidget::getGameInfo() const
{
    return gameInfo;
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
    this->toggleRulesModel( static_cast<RToggles>( row ) );
}

void RulesWidget::toggleRulesModel(const RToggles &row)
{
    auto intRow{ static_cast<int>( row ) };

    Qt::CheckState val{ ui->rulesView->item( intRow , 0 )->checkState() };
    ui->rulesView->item( intRow, 0 )->setCheckState( val == Qt::Checked ? Qt::Unchecked : Qt::Checked );

    val = ui->rulesView->item( intRow, 0 )->checkState();
    this->toggleRules( intRow, val );
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

                bool reUse{ false };
                bool ok{ false };

                Settings::setSetting( state, SKeys::Rules, SSubKeys::HasSvrPassword, serverName );
                if ( state != pwdCheckState )
                {
                    if ( state )
                    {
                        //Recycyle the Old password. Assuming it wasn't deleted.
                        if ( !pwd.isEmpty() )
                        {
                            title = "Re-Use Password:";
                            prompt = "Do you wish to re-use the stored Password?";

                            reUse = Helper::confirmAction( this, title, prompt );
                        }

                        if ( pwd.isEmpty()
                          || !reUse )
                        {
                            title = "Server Password:";
                            prompt = "Password:";
                            pwd = Helper::getTextResponse( this, title, prompt, "", &ok, MessageBox::SingleLine );
                            pwd = Helper::hashPassword( pwd );
                        }

                        if (( !pwd.isEmpty()
                            && ok )
                          || reUse )
                        {
                            if ( !reUse )
                                Settings::setSetting( pwd, SKeys::Rules, SSubKeys::SvrPassword, serverName );
                        }
                        else
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                            state = false;

                            Settings::setSetting( state, SKeys::Rules, SSubKeys::HasSvrPassword, serverName );
                        }
                    }
                    else if ( !Settings::getSetting( SKeys::Rules, SSubKeys::HasSvrPassword, serverName ).toBool()
                           && !pwd.isEmpty() )
                    {
                        title = "Remove Password:";
                        prompt = "Do you wish to erase the stored Password?";

                        if ( Helper::confirmAction( this, title, prompt ) )
                        {
                            state = false;
                            pwd.clear();
                            Settings::setSetting( pwd, SKeys::Rules, SSubKeys::SvrPassword, serverName );
                        }
                    }
                }
                pwdCheckState = state;
                pwd.clear();
            }
        break;
        case RToggles::AutoRestart:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::AutoRestart, serverName );
        break;
        case RToggles::WorldName:
            {
                QString world{ Settings::getSetting( SKeys::Rules, SSubKeys::World, serverName ).toString() };
                bool ok{ false };

                if ( state != worldCheckState )
                {
                    QString worldDir{ Settings::getSetting( SKeys::Setting, SSubKeys::WorldDir ).toString() };
                    if ( !worldDir.isEmpty() )
                    {
                        selectWorld = new SelectWorld( this );
                        selectWorld->setRequireWorld( !world.isEmpty() );

                        QObject::connect( selectWorld, &SelectWorld::accepted, selectWorld,
                        [&world, this]()
                        {
                            world = selectWorld->getSelectedWorld();

                            selectWorld->close();
                            selectWorld->disconnect();
                            selectWorld->deleteLater();
                        }, Qt::DirectConnection );
                        selectWorld->exec();

                        state = true;
                        if ( world.isEmpty() )
                            state = false;
                    }
                    else
                    {
                        if (( Settings::getSetting( SKeys::Rules, SSubKeys::World, serverName ).toString().isEmpty()
                          || !worldCheckState )
                          && state )
                        {
                            if ( world.isEmpty() )
                            {
                                title = "Server World:";
                                prompt = "World:";
                                world = Helper::getTextResponse( this, title, prompt, "", &ok, MessageBox::SingleLine );
                            }

                            if ( !world.isEmpty() && !ok )
                            {
                                ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                                state = false;
                            }
                        }
                        else if ( !Settings::getSetting( SKeys::Rules, SSubKeys::World, serverName ).toString().isEmpty()
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
                                world = "";
                        }
                    }
                    this->setSelectedWorld( world, state );
                    emit this->gameInfoChangedSignal( world );
                }
            }
        break;
        case RToggles::UrlAddr:
            {
                QString url{ Settings::getSetting( SKeys::Rules, SSubKeys::SvrUrl, serverName ).toString() };
                bool ok{ false };

                if ( state != urlCheckState )
                {
                    if (( Settings::getSetting( SKeys::Rules, SSubKeys::SvrUrl, serverName ).toString().isEmpty()
                       || !urlCheckState )
                      && state )
                    {
                        if ( url.isEmpty() )
                        {
                            title = "Server URL:";
                            prompt = "URL:";
                            url = Helper::getTextResponse( this, title, prompt, "", &ok, MessageBox::SingleLine );
                        }

                        if ( url.isEmpty() && !ok )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                            state = false;
                        }
                        else
                            Settings::setSetting( url, SKeys::Rules, SSubKeys::SvrUrl, serverName );
                    }
                    else if ( !Settings::getSetting( SKeys::Rules, SSubKeys::SvrUrl, serverName ).toString().isEmpty()
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
                            Settings::setSetting( "", SKeys::Rules, SSubKeys::SvrUrl, serverName );
                    }
                }
                rowText = "Server Home: [ %1 ]";
                rowText = rowText.arg( Settings::getSetting( SKeys::Rules, SSubKeys::SvrUrl, serverName ).toString() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                urlCheckState = state;
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

                if ( state != maxPlayersCheckState )
                {
                    if (( ( Settings::getSetting( SKeys::Rules, SSubKeys::MaxPlayers, serverName ) == 0 )
                       || !maxPlayersCheckState )
                      && state )
                    {
                        if ( maxPlrs == 0 )
                        {
                            title = "Max-Players:";
                            prompt = "Value:";
                            maxPlrs = Helper::getTextResponse( this, title, prompt, "", &ok, MessageBox::SingleLine ).toUInt();
                        }

                        if ( maxPlrs == 0 && !ok )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                            state = false;
                        }
                        else
                            Settings::setSetting( maxPlrs, SKeys::Rules, SSubKeys::MaxPlayers, serverName );
                    }
                    else if ( !( Settings::getSetting( SKeys::Rules, SSubKeys::MaxPlayers, serverName ) == 0 )
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
                            Settings::setSetting( "", SKeys::Rules, SSubKeys::MaxPlayers, serverName );
                    }
                }
                rowText = "Max Players: [ %1 ]";
                rowText = rowText.arg( Settings::getSetting( SKeys::Rules, SSubKeys::MaxPlayers, serverName ).toUInt() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                maxPlayersCheckState = state;
            }
        break;
        case RToggles::MaxIdle:
            {
                quint32 maxIdle{ Settings::getSetting( SKeys::Rules, SSubKeys::MaxIdle, serverName ).toUInt() };
                bool ok{ false };

                if ( state != maxIdleCheckState )
                {
                    if (( (Settings::getSetting( SKeys::Rules, SSubKeys::MaxIdle, serverName ).toUInt() == 0)
                       || !maxIdleCheckState )
                      && state )
                    {
                        if ( maxIdle == 0 )
                        {
                            title = "Max-Idle:";
                            prompt = "Value:";
                            maxIdle = Helper::getTextResponse( this, title, prompt, "", &ok, MessageBox::SingleLine ).toUInt();
                        }

                        if ( maxIdle == 0 && !ok )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                            state = false;
                        }
                        else
                            Settings::setSetting( maxIdle, SKeys::Rules, SSubKeys::MaxIdle, serverName );
                    }
                    else if ( !( Settings::getSetting( SKeys::Rules, SSubKeys::MaxIdle, serverName ).toUInt() == 0 )
                           || maxIdle == 0 )
                    {
                        title = "Remove Max-Idle:";
                        prompt = "Do you wish to erase the stored Max-Idle Value?";

                        if ( !Helper::confirmAction( this, title, prompt ) )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Checked );
                            state = true;
                        }
                        else
                            Settings::setSetting( 0, SKeys::Rules, SSubKeys::MaxIdle, serverName );
                    }
                }
                rowText = "Max Idle: [ %1 ] Minutes";
                QVariant val{ Settings::getSetting( SKeys::Rules, SSubKeys::MaxIdle, serverName ) };
                if ( !val.isValid() )
                    val = static_cast<qint64>( MAX_IDLE_TIME ) / 1000 / 60;

                rowText = rowText.arg( val.toUInt() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                maxIdleCheckState = state;

                //Max Idle Duration has changed, update all Players.
                emit this->setMaxIdleTimeSignal();
            }
        break;
        case RToggles::MinVersion:
            {
                QString version{ Settings::getSetting( SKeys::Rules, SSubKeys::MinVersion, serverName ).toString() };
                bool ok{ false };

                if ( state != minVersionCheckState )
                {
                    if (( Settings::getSetting( SKeys::Rules, SSubKeys::MinVersion, serverName ).toString().isEmpty()
                       || !minVersionCheckState)
                      && state )
                    {
                        if ( version.isEmpty() )
                        {
                            title = "Minimum Game Version:";
                            prompt = "Version:";
                            version = Helper::getTextResponse( this, title, prompt, "", &ok, MessageBox::SingleLine );
                        }

                        if ( version.isEmpty() && !ok )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                            state = false;
                        }
                        else
                            Settings::setSetting( version, SKeys::Rules, SSubKeys::MinVersion, serverName );
                    }
                    else if ( !Settings::getSetting( SKeys::Rules, SSubKeys::MinVersion, serverName ).toString().isEmpty()
                           && !version.isEmpty() )
                    {
                        title = "Remove Minimum Game Version:";
                        prompt = "Do you wish to erase the stored Min-Version?";

                        if ( !Helper::confirmAction( this, title, prompt ) )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Checked );
                            state = true;
                        }
                        else
                            Settings::setSetting( "", SKeys::Rules, SSubKeys::MinVersion, serverName );
                    }
                }
                rowText = "Minimum Game Version: [ %1 ]";
                rowText = rowText.arg( Settings::getSetting( SKeys::Rules, SSubKeys::MinVersion, serverName ).toString() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                minVersionCheckState = state;
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
            Settings::setSetting( state, SKeys::Rules, SSubKeys::NoPK, serverName );
            if ( state )
            {
                if ( getCheckedState( RToggles::AllPK ) )
                    this->toggleRulesModel( RToggles::AllPK );
            }
        break;
        case RToggles::ArenaPK:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::ArenaPK, serverName );
            if ( state )
            {
                if ( getCheckedState( RToggles::AllPK ) )
                    this->toggleRulesModel( RToggles::AllPK );
            }
        break;
    }
}
