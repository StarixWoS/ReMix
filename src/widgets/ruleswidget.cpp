
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
    QVariant val;
    QString rowText{ "" };

    val = Settings::getSetting( SKeys::Rules, SSubKeys::World, name );
    worldCheckState = !val.toString().isEmpty();
    this->setCheckedState( Toggles::world, worldCheckState );

    rowText = "World Name: [ %1 ]";
    ui->rulesView->item( Toggles::world, 0 )->setText( rowText.arg( val.toString() ) );
    emit this->gameInfoChangedSignal( val.toString() );

    rowText = "Server Home: [ %1 ]";
    val = Settings::getSetting( SKeys::Rules, SSubKeys::SvrUrl, name );
    ui->rulesView->item( Toggles::url, 0 )->setText( rowText.arg( val.toString() ) );

    rowText = "Max Players: [ %1 ]";
    val = Settings::getSetting( SKeys::Rules, SSubKeys::MaxPlayers, name );
    ui->rulesView->item( Toggles::maxP, 0 )->setText( rowText.arg( val.toUInt() ) );

    rowText = "Max AFK: [ %1 ] Minutes";
    val = Settings::getSetting( SKeys::Rules, SSubKeys::MaxAFK, name );
    ui->rulesView->item( Toggles::maxAFK, 0 )->setText( rowText.arg( val.toUInt() ) );

    rowText = "Minimum Game Version: [ %1 ]";
    val = Settings::getSetting( SKeys::Rules, SSubKeys::MinVersion, name );
    ui->rulesView->item( Toggles::minV, 0 )->setText( rowText.arg( val.toString() ) );

    minVersionCheckState = !Settings::getSetting( SKeys::Rules, SSubKeys::MinVersion, name ).toString().isEmpty();
    this->setCheckedState( Toggles::minV, minVersionCheckState );

    maxPlayersCheckState = Settings::getSetting( SKeys::Rules, SSubKeys::MaxPlayers, name ).toUInt() != 0;
    this->setCheckedState( Toggles::maxP, maxPlayersCheckState );

    urlCheckState = !Settings::getSetting( SKeys::Rules, SSubKeys::SvrUrl, name ).toString().isEmpty();
    this->setCheckedState( Toggles::url, urlCheckState );

    pwdCheckState = Settings::getSetting( SKeys::Rules, SSubKeys::HasSvrPassword, name ).toBool();
    this->setCheckedState( Toggles::svrPassword, pwdCheckState );

    maxAFKCheckState = Settings::getSetting( SKeys::Rules, SSubKeys::MaxAFK, name ).toUInt() != 0;
    this->setCheckedState( Toggles::maxAFK, maxAFKCheckState );

    this->setCheckedState( Toggles::noEavesdrop, Settings::getSetting( SKeys::Rules, SSubKeys::NoEavesdrop, name ).toBool() );
    this->setCheckedState( Toggles::noMigrate, Settings::getSetting( SKeys::Rules, SSubKeys::NoMigrate, name ).toBool() );
    this->setCheckedState( Toggles::arenaPK, Settings::getSetting( SKeys::Rules, SSubKeys::ArenaPK, name ).toBool() );
    this->setCheckedState( Toggles::ladder, Settings::getSetting( SKeys::Rules, SSubKeys::PKLadder, name ).toBool() );
    this->setCheckedState( Toggles::noBleep, Settings::getSetting( SKeys::Rules, SSubKeys::NoBleep, name ).toBool() );
    this->setCheckedState( Toggles::noCheat, Settings::getSetting( SKeys::Rules, SSubKeys::NoCheat, name ).toBool() );
    this->setCheckedState( Toggles::noMod, Settings::getSetting( SKeys::Rules, SSubKeys::NoModding, name ).toBool() );
    this->setCheckedState( Toggles::noPets, Settings::getSetting( SKeys::Rules, SSubKeys::NoPets, name ).toBool() );
    this->setCheckedState( Toggles::allPK, Settings::getSetting( SKeys::Rules, SSubKeys::AllPK, name ).toBool() );
    this->setCheckedState( Toggles::noPK, Settings::getSetting( SKeys::Rules, SSubKeys::NoPK, name ).toBool() );

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

bool RulesWidget::getCheckedState(const Toggles& option)
{
    return ui->rulesView->item( option, 0 )->checkState() == Qt::Checked;
}

void RulesWidget::setSelectedWorld(const QString& worldName, const bool& state)
{
    QString rowText{ "World Name: [ %1 ]" };
    if ( worldName.isEmpty() )
        rowText = rowText.arg( "Not Selected" );
    else
        rowText = rowText.arg( worldName );

    ui->rulesView->item( Toggles::world, 0 )->setText( rowText );
    ui->rulesView->item( Toggles::world, 0 )->setCheckState( state ? Qt::Checked : Qt::Unchecked );

    worldCheckState = state;
    Settings::setSetting( worldName, SKeys::Rules, SSubKeys::World, serverName );
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
    this->toggleRulesModel( row );
}

void RulesWidget::toggleRulesModel(const qint32 &row)
{
    Qt::CheckState val = ui->rulesView->item( row, 0 )->checkState();
    ui->rulesView->item( row, 0 )->setCheckState( val == Qt::Checked ? Qt::Unchecked : Qt::Checked );

    val = ui->rulesView->item( row, 0 )->checkState();
    this->toggleRules( row, val );
}

void RulesWidget::toggleRules(const qint32& row, const Qt::CheckState& value)
{
    bool state{ value == Qt::Checked };

    QString prompt{ "" };
    QString title{ "" };

    qint32 key{ row };
    QString rowText{ "" };

    switch ( key )
    {
        case Toggles::svrPassword:
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
                            pwd = Helper::getTextResponse( this, title, prompt, "", &ok, 0 );
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
        case Toggles::world:
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
                                world = Helper::getTextResponse( this, title, prompt, "", &ok, 0 );
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
        case Toggles::url:
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
                            url = Helper::getTextResponse( this, title, prompt, "", &ok, 0 );
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
        case Toggles::allPK:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::AllPK, serverName );
            if ( state )
            {
                if ( getCheckedState( Toggles::noPK ) )
                    this->toggleRulesModel( Toggles::noPK );

                if ( getCheckedState( Toggles::arenaPK ) )
                    this->toggleRulesModel( Toggles::arenaPK );
            }
        break;
        case Toggles::maxP:
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
                            maxPlrs = Helper::getTextResponse( this, title, prompt, "", &ok, 0 ).toUInt();
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
        case Toggles::maxAFK:
            {
                quint32 maxAFK{ Settings::getSetting( SKeys::Rules, SSubKeys::MaxAFK, serverName ).toUInt() };
                bool ok{ false };

                if ( state != maxAFKCheckState )
                {
                    if (( (Settings::getSetting( SKeys::Rules, SSubKeys::MaxAFK, serverName ).toUInt() == 0)
                       || !maxAFKCheckState )
                      && state )
                    {
                        if ( maxAFK == 0 )
                        {
                            title = "Max-AFK:";
                            prompt = "Value:";
                            maxAFK = Helper::getTextResponse( this, title, prompt, "", &ok, 0 ).toUInt();
                        }

                        if ( maxAFK == 0 && !ok )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                            state = false;
                        }
                        else
                            Settings::setSetting( maxAFK, SKeys::Rules, SSubKeys::MaxAFK, serverName );
                    }
                    else if ( !( Settings::getSetting( SKeys::Rules, SSubKeys::MaxAFK, serverName ).toUInt() == 0 )
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
                            Settings::setSetting( "", SKeys::Rules, SSubKeys::MaxAFK, serverName );
                    }
                }
                rowText = "Max AFK: [ %1 ] Minutes";
                rowText = rowText.arg( Settings::getSetting( SKeys::Rules, SSubKeys::MaxAFK, serverName ).toUInt() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                maxAFKCheckState = state;
            }
        break;
        case Toggles::minV:
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
                            version = Helper::getTextResponse( this, title, prompt, "", &ok, 0 );
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
        case Toggles::ladder:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::PKLadder, serverName );
        break;
        case Toggles::noBleep:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::NoBleep, serverName );
        break;
        case Toggles::noCheat:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::NoCheat, serverName );
        break;
        case Toggles::noEavesdrop:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::NoEavesdrop, serverName );
        break;
        case Toggles::noMigrate:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::NoMigrate, serverName );
        break;
        case Toggles::noMod:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::NoModding, serverName );
        break;
        case Toggles::noPets:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::NoPets, serverName );
        break;
        case Toggles::noPK:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::NoPK, serverName );
            if ( state )
            {
                if ( getCheckedState( Toggles::allPK ) )
                    this->toggleRulesModel( Toggles::allPK );
            }
        break;
        case Toggles::arenaPK:
            Settings::setSetting( state, SKeys::Rules, SSubKeys::ArenaPK, serverName );
            if ( state )
            {
                if ( getCheckedState( Toggles::allPK ) )
                    this->toggleRulesModel( Toggles::allPK );
            }
        break;
    }
}
