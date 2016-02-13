
#include "includes.hpp"
#include "settingswidget.hpp"
#include "ui_settingswidget.h"

SettingsWidget::SettingsWidget(QWidget *parent, QString svrID) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    serverID = svrID;

    //Load Settings from file.
    pwdCheckState = Settings::getRequirePassword( serverID );
    this->setCheckedState( Toggles::REQPWD,
                           pwdCheckState );

    this->setCheckedState( Toggles::REQADMINPWD,
                           Settings::getReqAdminAuth( serverID ) );

    this->setCheckedState( Toggles::ALLOWDUPEDIP,
                           Settings::getAllowDupedIP( serverID ) );

    this->setCheckedState( Toggles::BANDUPEDIP,
                           Settings::getBanDupedIP( serverID ) );

    this->setCheckedState( Toggles::BANHACKERS,
                           Settings::getBanHackers( serverID ) );

    this->setCheckedState( Toggles::REQSERNUM,
                           Settings::getReqSernums( serverID ) );

    this->setCheckedState( Toggles::DISCONNECTIDLES,
                           Settings::getDisconnectIdles( serverID ) );

    this->setCheckedState( Toggles::ALLOWSSV,
                           Settings::getAllowSSV( serverID ) );

    this->setCheckedState( Toggles::LOGCOMMENTS,
                           Settings::getLogComments( serverID ) );

    this->setCheckedState( Toggles::FWDCOMMENTS,
                           Settings::getFwdComments( serverID ) );

    this->setCheckedState( Toggles::INFORMADMINLOGIN,
                           Settings::getInformAdminLogin( serverID ) );

    this->setCheckedState( Toggles::ECHOCOMMENTS,
                           Settings::getEchoComments( serverID ) );

    this->setCheckedState( Toggles::MINIMIZETOTRAY,
                           Settings::getMinimizeToTray( serverID ) );

    this->setCheckedState( Toggles::SAVEWINDOWPOSITIONS,
                           Settings::getSaveWindowPositions( serverID ) );
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::setCheckedState(Toggles option, bool val)
{
    Qt::CheckState state;
    if ( val )
        state = Qt::Checked;
    else
        state = Qt::Unchecked;

    ui->settingsView->item( option, 0 )->setCheckState( state );
}

void SettingsWidget::on_settingsView_itemClicked(QTableWidgetItem *item)
{
    if ( item != nullptr )
    {
        if ( item->checkState() == Qt::Checked
          || item->checkState() == Qt::Unchecked )
        {
            this->toggleSettings( item->row(), item->checkState() );
        }
    }
}

void SettingsWidget::on_settingsView_doubleClicked(const QModelIndex &index)
{
    int row = index.row();

    Qt::CheckState val = ui->settingsView->item( row, 0 )->checkState();
    ui->settingsView->item( row, 0 )->setCheckState( val == Qt::Checked
                                                     ? Qt::Unchecked
                                                     : Qt::Checked );

    val = ui->settingsView->item( row, 0 )->checkState();
    this->toggleSettings( row, val );
}

void SettingsWidget::toggleSettings(quint32 row, Qt::CheckState value)
{
    QVariant state = value == Qt::Checked;

    QString title{ "" };
    QString prompt{ "" };

    switch ( row )
    {
        case Toggles::REQPWD:
            {
                QString pwd{ Settings::getPassword( serverID ) };

                bool reUse{ false };
                bool ok{ false };

                Settings::setRequirePassword( state, serverID );
                if ( state.toBool() != pwdCheckState )
                {
                    if ( Settings::getRequirePassword( serverID ) )
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
                            pwd = Helper::getTextResponse( this, title,
                                                           prompt, &ok, 0 );
                        }

                        if (( !pwd.isEmpty()
                           && ok )
                          || reUse )
                        {
                            if ( !reUse )
                                Settings::setPassword( pwd, serverID );
                        }
                        else
                        {
                            ui->settingsView->item( row, 0 )->setCheckState(
                                        Qt::Unchecked );

                            state = false;
                            Settings::setRequirePassword( state, serverID );
                        }
                    }
                    else if ( !Settings::getRequirePassword( serverID )
                              && !pwd.isEmpty() )
                    {
                        title = "Remove Password:";
                        prompt = "Do you wish to erase the stored Password?";

                        if ( Helper::confirmAction( this, title, prompt ) )
                        {
                            pwd.clear();
                            Settings::setPassword( pwd, serverID );
                        }
                    }
                }
                pwdCheckState = state.toBool();
            }
        break;
        case Toggles::REQADMINPWD:
            Settings::setReqAdminAuth( state, serverID );
        break;
        case Toggles::ALLOWDUPEDIP:
            Settings::setAllowDupedIP( state, serverID );
        break;
        case Toggles::BANDUPEDIP:
            Settings::setBanDupedIP( state, serverID );
        break;
        case Toggles::BANHACKERS:
            Settings::setBanHackers( state, serverID );
        break;
        case Toggles::REQSERNUM:
            Settings::setReqSernums( state, serverID );
        break;
        case Toggles::DISCONNECTIDLES:
            Settings::setDisconnectIdles( state, serverID );
        break;
        case Toggles::ALLOWSSV:
            Settings::setAllowSSV( state, serverID );
        break;
        case Toggles::LOGCOMMENTS:
            Settings::setLogComments( state, serverID );
        break;
        case Toggles::FWDCOMMENTS:
            Settings::setFwdComments( state, serverID );
        break;
        case Toggles::ECHOCOMMENTS:
            Settings::setEchoComments( state, serverID );
        break;
        case Toggles::INFORMADMINLOGIN:
            Settings::setInformAdminLogin( state, serverID );
        break;
        case Toggles::MINIMIZETOTRAY:
            Settings::setMinimizeToTray( state, serverID );
        break;
        case Toggles::SAVEWINDOWPOSITIONS:
            Settings::setSaveWindowPositions( state, serverID );
        break;
        default:
            qDebug() << "Unknown Option, doing nothing!";
        break;
    }
}
