
#include "includes.hpp"
#include "ruleswidget.hpp"
#include "ui_ruleswidget.h"

RulesWidget::RulesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RulesWidget)
{
    ui->setupUi(this);

    QString rowText{ "" };

    //Load Rules from file.
    worldCheckState = !Rules::getWorldName().isEmpty();
    this->setCheckedState( Toggles::world,
                           worldCheckState );

    rowText = "World Name: [ %1 ]";
    rowText = rowText.arg( Rules::getWorldName() );
    ui->rulesView->item( Toggles::world, 0 )->setText( rowText );

    urlCheckState = !Rules::getURLAddress().isEmpty();
    this->setCheckedState( Toggles::url,
                           urlCheckState );

    rowText = "Server Home: [ %1 ]";
    rowText = rowText.arg( Rules::getURLAddress() );
    ui->rulesView->item( Toggles::url, 0 )->setText( rowText );

    this->setCheckedState( Toggles::allPK,
                           Rules::getAllPKing() );

    maxPlayersCheckState = !Rules::getRequireMaxPlayers();
    this->setCheckedState( Toggles::maxP,
                           maxPlayersCheckState );

    rowText = "Max Players: [ %1 ]";
    rowText = rowText.arg( Rules::getMaxPlayers() );
    ui->rulesView->item( Toggles::maxP, 0 )->setText( rowText );

    maxAFKCheckState = !Rules::getRequireMaxAFK();
    this->setCheckedState( Toggles::maxAFK,
                           maxAFKCheckState );

    rowText = "Max AFK: [ %1 ] Minutes";
    rowText = rowText.arg( Rules::getMaxAFK() );
    ui->rulesView->item( Toggles::maxAFK, 0 )->setText( rowText );

    minVersionCheckState = !Rules::getMinVersion().isEmpty();
    this->setCheckedState( Toggles::minV,
                           minVersionCheckState );

    rowText = "Min Version: [ %1 ]";
    rowText = rowText.arg( Rules::getMinVersion() );
    ui->rulesView->item( Toggles::minV, 0 )->setText( rowText );

    this->setCheckedState( Toggles::ladder,
                           Rules::getReportLadder() );

    this->setCheckedState( Toggles::noBleep,
                           Rules::getNoCursing());

    this->setCheckedState( Toggles::noCheat,
                           Rules::getNoCheating() );

    this->setCheckedState( Toggles::noEavesdrop,
                           Rules::getNoEavesdropping() );

    this->setCheckedState( Toggles::noMigrate,
                           Rules::getNoMigrating() );

    this->setCheckedState( Toggles::noMod,
                           Rules::getNoModding() );

    this->setCheckedState( Toggles::noPets,
                           Rules::getNoPets() );

    this->setCheckedState( Toggles::noPK,
                           Rules::getNoPKing() );

    this->setCheckedState( Toggles::arenaPK,
                           Rules::getArenaPKing() );
}

RulesWidget::~RulesWidget()
{
    delete ui;
}

void RulesWidget::setCheckedState(Toggles option, bool val)
{
    Qt::CheckState state;
    if ( val )
        state = Qt::Checked;
    else
        state = Qt::Unchecked;

    ui->rulesView->item( option, 0 )->setCheckState( state );
}

void RulesWidget::on_rulesView_itemClicked(QTableWidgetItem *item)
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

void RulesWidget::on_rulesView_doubleClicked(const QModelIndex &index)
{
    int row = index.row();

    Qt::CheckState val = ui->rulesView->item( row, 0 )->checkState();
    ui->rulesView->item( row, 0 )->setCheckState( val == Qt::Checked
                                                ? Qt::Unchecked
                                                : Qt::Checked );

    val = ui->rulesView->item( row, 0 )->checkState();
    this->toggleRules( row, val );
}

void RulesWidget::toggleRules(quint32 row, Qt::CheckState value)
{
    QVariant state = value == Qt::Checked;
    QSettings rules( "preferences.ini", QSettings::IniFormat );
              rules.beginGroup( Settings::keys[ Settings::Rules ] );

    QString prompt{ "" };
    QString title{ "" };

    QString rowText{ "" };

    switch ( row )
    {
        case Toggles::world:
            {
                QString world{ Rules::getWorldName() };
                bool ok{ false };

                if ( state.toBool() != worldCheckState )
                {
                    if ( Rules::getRequireWorld()
                      || !worldCheckState )
                    {
                        if ( world.isEmpty() )
                        {
                            title = "Server World:";
                            prompt = "World:";
                            world = Helper::getTextResponse( this, title,
                                                             prompt, &ok, 0 );
                        }

                        if ( !world.isEmpty() && !ok )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState(
                                        Qt::Unchecked );

                            state = false;
                        }
                        else
                            Rules::setWorldName( world );
                    }
                    else if ( !Rules::getRequireWorld()
                           && !world.isEmpty() )
                    {
                        title = "Remove World:";
                        prompt = "Do you wish to erase the stored World Name?";

                        if ( !Helper::confirmAction( this, title, prompt ) )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState(
                                        Qt::Checked );
                            state = true;
                        }
                        else
                            rules.remove( Rules::subKeys[ Rules::world ] );
                    }
                }
                rowText = "World Name: [ %1 ]";
                rowText = rowText.arg( Rules::getWorldName() );

                ui->rulesView->item( row, 0 )->setText( rowText );

                worldCheckState = state.toBool();
            }
        break;
        case Toggles::url:
            {
                QString url{ Rules::getURLAddress() };
                bool ok{ false };

                if ( state.toBool() != urlCheckState )
                {
                    if ( Rules::getRequireURL()
                      || !urlCheckState )
                    {
                        if ( url.isEmpty() )
                        {
                            title = "Server URL:";
                            prompt = "URL:";
                            url = Helper::getTextResponse( this, title,
                                                           prompt, &ok, 0 );
                        }

                        if ( url.isEmpty() && !ok  )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState(
                                        Qt::Unchecked );

                            state = false;
                        }
                        else
                            Rules::setURLAddress( url );
                    }
                    else if ( !Rules::getRequireURL()
                           && !url.isEmpty() )
                    {
                        title = "Remove URL:";
                        prompt = "Do you wish to erase the stored URL Address?";

                        if ( !Helper::confirmAction( this, title, prompt ) )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState(
                                        Qt::Checked );
                            state = true;
                        }
                        else
                            rules.remove( Rules::subKeys[ Rules::url ] );
                    }
                }
                rowText = "Server Home: [ %1 ]";
                rowText = rowText.arg( Rules::getURLAddress() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                urlCheckState = state.toBool();
            }
        break;
        case Toggles::allPK:
            Rules::setAllPKing( state );
        break;
        case Toggles::maxP:
            {
                quint32 maxPlrs{ Rules::getMaxPlayers() };
                bool ok{ false };

                if ( state.toBool() != maxPlayersCheckState )
                {
                    if ( Rules::getRequireMaxPlayers()
                      || !maxPlayersCheckState )
                    {
                        if ( maxPlrs == 0 )
                        {
                            title = "Max-Players:";
                            prompt = "Value:";
                            maxPlrs = Helper::getTextResponse( this, title,
                                                               prompt, &ok, 0 )
                                                     .toUInt();
                        }

                        if ( maxPlrs == 0 && !ok  )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState(
                                        Qt::Unchecked );

                            state = false;
                        }
                        else
                            Rules::setMaxPlayers( maxPlrs );
                    }
                    else if ( !Rules::getRequireMaxPlayers()
                           && maxPlrs != 0 )
                    {
                        title = "Remove Max-Players:";
                        prompt = "Do you wish to erase the stored Max-Players "
                                 "Value?";

                        if ( !Helper::confirmAction( this, title, prompt ) )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState(
                                        Qt::Checked );
                            state = true;
                        }
                        else
                            rules.remove( Rules::subKeys[ Rules::maxP ] );
                    }
                }
                rowText = "Max Players: [ %1 ]";
                rowText = rowText.arg( Rules::getMaxPlayers() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                maxPlayersCheckState = state.toBool();
            }
        break;
        case Toggles::maxAFK:
            {
                quint32 maxAFK{ Rules::getMaxAFK() };
                bool ok{ false };

                if ( state.toBool() != maxAFKCheckState )
                {
                    if ( Rules::getRequireMaxAFK()
                      || !maxAFKCheckState )
                    {
                        if ( maxAFK == 0 )
                        {
                            title = "Max-AFK:";
                            prompt = "Value:";
                            maxAFK = Helper::getTextResponse( this, title,
                                                              prompt, &ok, 0 )
                                                    .toUInt();
                        }

                        if ( maxAFK == 0 && !ok  )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState(
                                        Qt::Unchecked );

                            state = false;
                        }
                        else
                            Rules::setMaxAFK( maxAFK );
                    }
                    else if ( !Rules::getRequireMaxAFK()
                           || maxAFK == 0 )
                    {
                        title = "Remove Max-AFK:";
                        prompt = "Do you wish to erase the stored Max-AFK "
                                 "Value?";

                        if ( !Helper::confirmAction( this, title, prompt ) )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState(
                                        Qt::Checked );
                            state = true;
                        }
                        else
                            rules.remove( Rules::subKeys[ Rules::maxAFK ] );
                    }
                }
                rowText = "Max AFK: [ %1 ] Minutes";
                rowText = rowText.arg( Rules::getMaxAFK() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                maxAFKCheckState = state.toBool();
            }
        break;
        case Toggles::minV:
            {
                QString version{ Rules::getMinVersion() };
                bool ok{ false };

                if ( state.toBool() != minVersionCheckState )
                {
                    if ( Rules::getRequireMinVersion()
                      || !minVersionCheckState )
                    {
                        if ( version.isEmpty() )
                        {
                            title = "Min-Version:";
                            prompt = "Version:";
                            version = Helper::getTextResponse( this, title,
                                                               prompt, &ok, 0 );
                        }

                        if ( version.isEmpty() && !ok  )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState(
                                        Qt::Unchecked );

                            state = false;
                        }
                        else
                            Rules::setMinVersion( version );
                    }
                    else if ( !Rules::getRequireMinVersion()
                           && !version.isEmpty() )
                    {
                        title = "Remove Min-Version:";
                        prompt = "Do you wish to erase the stored Min-Version?";

                        if ( !Helper::confirmAction( this, title, prompt ) )
                        {
                            ui->rulesView->item( row, 0 )->setCheckState(
                                        Qt::Checked );
                            state = true;
                        }
                        else
                            rules.remove( Rules::subKeys[ Rules::minV ] );
                    }
                }
                rowText = "Min Version: [ %1 ]";
                rowText = rowText.arg( Rules::getMinVersion() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                minVersionCheckState = state.toBool();
            }
        break;
        case Toggles::ladder:
            Rules::setReportLadder( state );
        break;
        case Toggles::noBleep:
            Rules::setNoCursing( state );
        break;
        case Toggles::noCheat:
            Rules::setNoCheating( state );
        break;
        case Toggles::noEavesdrop:
            Rules::setNoEavesdropping( state );
        break;
        case Toggles::noMigrate:
            Rules::setNoMigrating( state );
        break;
        case Toggles::noMod:
            Rules::setNoModding( state );
        break;
        case Toggles::noPets:
            Rules::setNoPets( state );
        break;
        case Toggles::noPK:
            Rules::setNoPKing( state );
        break;
        case Toggles::arenaPK:
            Rules::setArenaPKing( state );
         break;
        default:
            qDebug() << "Unknown Rule, doing nothing!";
        break;
    }
}
