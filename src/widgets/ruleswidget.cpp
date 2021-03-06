
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
    QVariant state_v = value == Qt::Checked;
    bool state_b{ state_v.toBool() };

    QSettings rules( "preferences.ini", QSettings::IniFormat );
              rules.beginGroup( Settings::keys[ Settings::Rules ] );

    QString prompt{ "" };
    QString title{ "" };

    bool removeKey{ false };
    quint32 key{ row };

    QString rowText{ "" };

    switch ( key )
    {
        case Toggles::world:
            {
                QString world{ Rules::getWorldName() };
                bool ok{ false };

                if ( state_b != worldCheckState )
                {
                    if (( Rules::getRequireWorld()
                       || !worldCheckState )
                      && state_b )
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

                            state_v = false;
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
                            state_v = true;
                        }
                        else
                            rules.remove( Rules::subKeys[ Rules::world ] );
                    }
                }
                rowText = "World Name: [ %1 ]";
                rowText = rowText.arg( Rules::getWorldName() );

                ui->rulesView->item( row, 0 )->setText( rowText );

                worldCheckState = state_v.toBool();
            }
        break;
        case Toggles::url:
            {
                QString url{ Rules::getURLAddress() };
                bool ok{ false };

                if ( state_b != urlCheckState )
                {
                    if (( Rules::getRequireURL()
                       || !urlCheckState )
                      && state_b )
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

                            state_v = false;
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
                            state_v = true;
                        }
                        else
                            rules.remove( Rules::subKeys[ Rules::url ] );
                    }
                }
                rowText = "Server Home: [ %1 ]";
                rowText = rowText.arg( Rules::getURLAddress() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                urlCheckState = state_v.toBool();
            }
        break;
        case Toggles::allPK:
            if ( !state_b )
                removeKey = true;
            else
                Rules::setAllPKing( state_v );
        break;
        case Toggles::maxP:
            {
                quint32 maxPlrs{ Rules::getMaxPlayers() };
                bool ok{ false };

                if ( state_b != maxPlayersCheckState )
                {
                    if (( Rules::getRequireMaxPlayers()
                       || !maxPlayersCheckState )
                      && state_b )
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

                            state_v = false;
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
                            state_v = true;
                        }
                        else
                            rules.remove( Rules::subKeys[ Rules::maxP ] );
                    }
                }
                rowText = "Max Players: [ %1 ]";
                rowText = rowText.arg( Rules::getMaxPlayers() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                maxPlayersCheckState = state_v.toBool();
            }
        break;
        case Toggles::maxAFK:
            {
                quint32 maxAFK{ Rules::getMaxAFK() };
                bool ok{ false };

                if ( state_b != maxAFKCheckState )
                {
                    if (( Rules::getRequireMaxAFK()
                       || !maxAFKCheckState )
                      && state_b )
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

                            state_v = false;
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
                            state_v = true;
                        }
                        else
                            rules.remove( Rules::subKeys[ Rules::maxAFK ] );
                    }
                }
                rowText = "Max AFK: [ %1 ] Minutes";
                rowText = rowText.arg( Rules::getMaxAFK() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                maxAFKCheckState = state_v.toBool();
            }
        break;
        case Toggles::minV:
            {
                QString version{ Rules::getMinVersion() };
                bool ok{ false };

                if ( state_b != minVersionCheckState )
                {
                    if (( Rules::getRequireMinVersion()
                       || !minVersionCheckState)
                      && state_b )
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

                            state_v = false;
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
                            state_v = true;
                        }
                        else
                            rules.remove( Rules::subKeys[ Rules::minV ] );
                    }
                }
                rowText = "Min Version: [ %1 ]";
                rowText = rowText.arg( Rules::getMinVersion() );
                ui->rulesView->item( row, 0 )->setText( rowText );

                minVersionCheckState = state_v.toBool();
            }
        break;
        case Toggles::ladder:
            if ( !state_b )
                removeKey = true;
            else
                Rules::setReportLadder( state_v );
        break;
        case Toggles::noBleep:
            if ( !state_b )
                removeKey = true;
            else
                Rules::setNoCursing( state_v );
        break;
        case Toggles::noCheat:
            if ( !state_b )
                removeKey = true;
            else
                Rules::setNoCheating( state_v );
        break;
        case Toggles::noEavesdrop:
            if ( !state_b )
                removeKey = true;
            else
                Rules::setNoEavesdropping( state_v );
        break;
        case Toggles::noMigrate:
            if ( !state_b )
                removeKey = true;
            else
                Rules::setNoMigrating( state_v );
        break;
        case Toggles::noMod:
            if ( !state_b )
                removeKey = true;
            else
                Rules::setNoModding( state_v );
        break;
        case Toggles::noPets:
            if ( !state_b )
                removeKey = true;
            else
                Rules::setNoPets( state_v );
        break;
        case Toggles::noPK:
            if ( !state_b )
                removeKey = true;
            else
                Rules::setNoPKing( state_v );
        break;
        case Toggles::arenaPK:
            if ( !state_b )
                removeKey = true;
            else
                Rules::setArenaPKing( state_v );
         break;
        default:
            qDebug() << "Unknown Rule, doing nothing!";
        break;
    }

    if ( removeKey )
        rules.remove( Rules::subKeys[ key ] );
}
