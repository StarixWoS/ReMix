
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
    QString rowText{ "" };
    pwdCheckState = !Rules::getRule( name, RuleKeys::SvrPassword ).toString().isEmpty();
    this->setCheckedState( Toggles::svrPassword, pwdCheckState );

    worldCheckState = !Rules::getRule( name, RuleKeys::World ).toString().isEmpty();
    this->setCheckedState( Toggles::world, worldCheckState );

    rowText = "World Name: [ %1 ]";
    ui->rulesView->item( Toggles::world, 0 )->setText( rowText.arg( Rules::getRule( name, RuleKeys::World ).toString() ) );
    emit this->gameInfoChangedSignal( Rules::getRule( name, RuleKeys::World ).toString() );

    urlCheckState = !Rules::getRule( name, RuleKeys::SvrUrl ).toString().isEmpty();
    this->setCheckedState( Toggles::url, urlCheckState );

    rowText = "Server Home: [ %1 ]";
    ui->rulesView->item( Toggles::url, 0 )->setText( rowText.arg( Rules::getRule( name, RuleKeys::SvrUrl ).toString() ) );

    this->setCheckedState( Toggles::allPK, Rules::getRule( name, RuleKeys::AllPK ).toBool() );

    maxPlayersCheckState = Rules::getRule( name, RuleKeys::MaxPlayers ).toUInt() != 0;
    this->setCheckedState( Toggles::maxP, maxPlayersCheckState );

    rowText = "Max Players: [ %1 ]";
    ui->rulesView->item( Toggles::maxP, 0 )->setText( rowText.arg( Rules::getRule( name, RuleKeys::MaxPlayers ).toUInt() ) );

    maxAFKCheckState = Rules::getRule( name, RuleKeys::MaxAFK ).toUInt() != 0;
    this->setCheckedState( Toggles::maxAFK, maxAFKCheckState );

    rowText = "Max AFK: [ %1 ] Minutes";
    ui->rulesView->item( Toggles::maxAFK, 0 )->setText( rowText.arg( Rules::getRule( name, RuleKeys::MaxAFK ).toUInt() ) );

    minVersionCheckState = !Rules::getRule( name, RuleKeys::MinVersion ).toString().isEmpty();
    this->setCheckedState( Toggles::minV, minVersionCheckState );

    rowText = "Minimum Game Version: [ %1 ]";
    ui->rulesView->item( Toggles::minV, 0 )->setText( rowText.arg( Rules::getRule( name, RuleKeys::MinVersion ).toString() ) );
    this->setCheckedState( Toggles::ladder, Rules::getRule( name, RuleKeys::PKLadder ).toBool() );
    this->setCheckedState( Toggles::noBleep, Rules::getRule( name, RuleKeys::NoBleep ).toBool() );
    this->setCheckedState( Toggles::noCheat, Rules::getRule( name, RuleKeys::NoCheat ).toBool() );
    this->setCheckedState( Toggles::noEavesdrop, Rules::getRule( name, RuleKeys::NoEavesdrop ).toBool() );
    this->setCheckedState( Toggles::noMigrate, Rules::getRule( name, RuleKeys::NoMigrate ).toBool() );
    this->setCheckedState( Toggles::noMod, Rules::getRule( name, RuleKeys::NoModding ).toBool() );
    this->setCheckedState( Toggles::noPets, Rules::getRule( name, RuleKeys::NoPets ).toBool() );
    this->setCheckedState( Toggles::noPK, Rules::getRule( name, RuleKeys::NoPK ).toBool() );
    this->setCheckedState( Toggles::arenaPK, Rules::getRule( name, RuleKeys::ArenaPK ).toBool() );

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
    Rules::setRule( serverName, worldName, RuleKeys::World );
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
                QString pwd{ Rules::getRule( serverName, RuleKeys::SvrPassword ).toString() };

                bool reUse{ false };
                bool ok{ false };

                Rules::setRule( serverName, state, RuleKeys::SvrPassword );
                if ( state != pwdCheckState )
                {
                    if ( !Rules::getRule( serverName, RuleKeys::SvrPassword ).toString().isEmpty() )
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
                                Rules::setRule( serverName, pwd, RuleKeys::SvrPassword );
                        }
                        else
                        {
                            ui->rulesView->item( row, 0 )->setCheckState( Qt::Unchecked );
                            state = false;

                            pwd.clear();
                            Rules::setRule( serverName, pwd, RuleKeys::SvrPassword );
                        }
                    }
                    else if ( !Rules::getRule( serverName, RuleKeys::SvrPassword ).toString().isEmpty()
                           && !pwd.isEmpty() )
                    {
                        title = "Remove Password:";
                        prompt = "Do you wish to erase the stored Password?";

                        if ( Helper::confirmAction( this, title, prompt ) )
                        {
                            state = false;
                            pwd.clear();
                            Rules::setRule( serverName, pwd, RuleKeys::SvrPassword );
                        }
                    }
                }
                pwdCheckState = state;
            }
        break;
        case Toggles::world:
            {
                QString world{ Rules::getRule( serverName, RuleKeys::World ).toString() };
                bool ok{ false };

                if ( state != worldCheckState )
                {
                    QString worldDir{ Settings::getSetting( SettingKeys::Setting, SettingSubKeys::WorldDir ).toString() };
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
                        if (( Rules::getRule( serverName, RuleKeys::World ).toString().isEmpty()
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
                        else if ( !Rules::getRule( serverName, RuleKeys::World ).toString().isEmpty()
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
                QString url{ Rules::getRule( serverName, RuleKeys::SvrUrl ).toString() };
                bool ok{ false };

                if ( state != urlCheckState )
                {
                    if (( Rules::getRule( serverName, RuleKeys::SvrUrl ).toString().isEmpty()
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
                            Rules::setRule( serverName, url, RuleKeys::SvrUrl );
                    }
                    else if ( !Rules::getRule( serverName, RuleKeys::SvrUrl ).toString().isEmpty()
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
                            Rules::setRule( serverName, "", RuleKeys::SvrUrl );
                    }
                }
                rowText = "Server Home: [ %1 ]";
                rowText = rowText.arg( Rules::getRule( serverName, RuleKeys::SvrUrl ).toString() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                urlCheckState = state;
            }
        break;
        case Toggles::allPK:
            Rules::setRule( serverName, state, RuleKeys::AllPK );
            if ( state )
            {
                if ( getCheckedState( Toggles::noPK ) )
                    this->toggleRulesModel( Toggles::noPK );
            }
        break;
        case Toggles::maxP:
            {
                quint32 maxPlrs{ Rules::getRule( serverName, RuleKeys::MaxPlayers ).toUInt() };
                bool ok{ false };

                if ( state != maxPlayersCheckState )
                {
                    if (( ( Rules::getRule( serverName, RuleKeys::MaxPlayers ) == 0 )
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
                            Rules::setRule( serverName, maxPlrs, RuleKeys::MaxPlayers );
                    }
                    else if ( !( Rules::getRule( serverName, RuleKeys::MaxPlayers ) == 0 )
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
                            Rules::setRule( serverName, "", RuleKeys::MaxPlayers );
                    }
                }
                rowText = "Max Players: [ %1 ]";
                rowText = rowText.arg( Rules::getRule( serverName, RuleKeys::MaxPlayers ).toUInt() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                maxPlayersCheckState = state;
            }
        break;
        case Toggles::maxAFK:
            {
                quint32 maxAFK{ Rules::getRule( serverName, RuleKeys::MaxAFK ).toUInt() };
                bool ok{ false };

                if ( state != maxAFKCheckState )
                {
                    if (( (Rules::getRule( serverName, RuleKeys::MaxAFK ).toUInt() == 0)
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
                            Rules::setRule( serverName, maxAFK, RuleKeys::MaxAFK );
                    }
                    else if ( !( Rules::getRule( serverName, RuleKeys::MaxAFK ).toUInt() == 0 )
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
                            Rules::setRule( serverName, "", RuleKeys::MaxAFK );
                    }
                }
                rowText = "Max AFK: [ %1 ] Minutes";
                rowText = rowText.arg( Rules::getRule( serverName, RuleKeys::MaxAFK ).toUInt() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                maxAFKCheckState = state;
            }
        break;
        case Toggles::minV:
            {
                QString version{ Rules::getRule( serverName, RuleKeys::MinVersion ).toString() };
                bool ok{ false };

                if ( state != minVersionCheckState )
                {
                    if (( Rules::getRule( serverName, RuleKeys::MinVersion ).toString().isEmpty()
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
                            Rules::setRule( serverName, version, RuleKeys::MinVersion );
                    }
                    else if ( !Rules::getRule( serverName, RuleKeys::MinVersion ).toString().isEmpty()
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
                            Rules::setRule( serverName, "", RuleKeys::MinVersion );
                    }
                }
                rowText = "Minimum Game Version: [ %1 ]";
                rowText = rowText.arg( Rules::getRule( serverName, RuleKeys::MinVersion ).toString() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                minVersionCheckState = state;
            }
        break;
        case Toggles::ladder:
            Rules::setRule( serverName, state, RuleKeys::PKLadder );
        break;
        case Toggles::noBleep:
            Rules::setRule( serverName, state, RuleKeys::NoBleep );
        break;
        case Toggles::noCheat:
            Rules::setRule( serverName, state, RuleKeys::NoCheat );
        break;
        case Toggles::noEavesdrop:
            Rules::setRule( serverName, state, RuleKeys::NoEavesdrop );
        break;
        case Toggles::noMigrate:
            Rules::setRule( serverName, state, RuleKeys::NoMigrate );
        break;
        case Toggles::noMod:
            Rules::setRule( serverName, state, RuleKeys::NoModding );
        break;
        case Toggles::noPets:
            Rules::setRule( serverName, state, RuleKeys::NoPets );
        break;
        case Toggles::noPK:
            Rules::setRule( serverName, state, RuleKeys::NoPK );
            if ( state )
            {
                if ( getCheckedState( Toggles::allPK ) )
                    this->toggleRulesModel( Toggles::allPK );
            }
        break;
        case Toggles::arenaPK:
            Rules::setRule( serverName, state, RuleKeys::ArenaPK );
        break;
    }
}
