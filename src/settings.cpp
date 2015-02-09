
#include "includes.hpp"
#include "settings.hpp"
#include "ui_settings.h"

Settings::Settings(QWidget *parent, Admin* aDlg) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    adminDialog = aDlg;

    //Remove the "Help" button from the window title bars.
    {
        QIcon icon = this->windowIcon();
        Qt::WindowFlags flags = this->windowFlags();
        flags &= ~Qt::WindowContextHelpButtonHint;

        this->setWindowFlags( flags );
        this->setWindowIcon( icon );

        this->setWindowModality( Qt::WindowModal );
    }

    //Load Settings from file.
    this->setCheckedState( Options::ReqPwd,
                           Helper::getRequirePassword() );

    this->setCheckedState( Options::ReqAdminPwd,
                           adminDialog->getReqAdminAuth() );

    this->setCheckedState( Options::AllowDupIP,
                           Helper::getAllowDupedIP() );

    this->setCheckedState( Options::BanDupIP,
                           Helper::getBanDupedIP() );

    this->setCheckedState( Options::BanHack,
                           Helper::getBanHackers() );

    this->setCheckedState( Options::ReqSernum,
                           Helper::getReqSernums() );

    this->setCheckedState( Options::DisconnectIdle,
                           Helper::getDisconnectIdles() );

    this->setCheckedState( Options::AllowSSV,
                           Helper::getAllowSSV() );

    this->setCheckedState( Options::LogComments,
                           Helper::getLogComments() );
}

Settings::~Settings()
{
    delete ui;
}

void Settings::setCheckedState(Options option, bool val)
{
    Qt::CheckState state;
    if ( val )
        state = Qt::Checked;
    else
        state = Qt::Unchecked;

    ui->settingsView->item( option, 0 )->setCheckState( state );
}

void Settings::on_settingsView_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    if ( row < 0 )
        return;

    Qt::CheckState val = ui->settingsView->item( row, 0 )->checkState();
    ui->settingsView->item( row, 0 )->setCheckState(
                val == Qt::Checked ? Qt::Unchecked : Qt::Checked );

    val = ui->settingsView->item( row, 0 )->checkState();

    QVariant state = val == Qt::Checked;

    QString title{ "" };
    QString prompt{ "" };

    switch ( row )
    {
        case Options::ReqPwd:
            {
                QString txt{ "" };
                bool ok;

                Helper::setRequirePassword( state );
                if ( Helper::getPassword().isEmpty()
                  && Helper::getRequirePassword() )
                {
                    title = "Server Password:";
                    prompt = "Password:";

                    txt = Helper::getTextResponse( this, title,
                                                   prompt, &ok, 0 );
                    if ( ok && !txt.isEmpty() )
                    {
                        Helper::setPassword( txt );
                    }
                    else
                    {   //Invalid dialog state or no input Password.
                        //Reset the Object's state.
                        ui->settingsView->item( row, 0 )->setCheckState(
                                    Qt::Unchecked );

                        state = false;
                        Helper::setRequirePassword( state );
                    }
                }
                else if ( !Helper::getRequirePassword() )
                {
                    title = "Remove Password:";
                    prompt = "Do you wish to erase the stored Password hash?";

                    if ( Helper::confirmAction( this, title, prompt ) )
                        Helper::setPassword( txt );
                }
            }
        break;
        case Options::ReqAdminPwd:
            adminDialog->setReqAdminAuth( state );
        break;
        case Options::AllowDupIP:
            Helper::setAllowDupedIP( state );
        break;
        case Options::BanDupIP:
            Helper::setBanDupedIP( state );
        break;
        case Options::BanHack:
            Helper::setBanHackers( state );
        break;
        case Options::ReqSernum:
            Helper::setReqSernums( state );
        break;
        case Options::DisconnectIdle:
            Helper::setDisconnectIdles( state );
        break;
        case Options::AllowSSV:
            Helper::setAllowSSV( state );
        break;
        case Options::LogComments:
            Helper::setLogComments( state );
        break;
        default:
            qDebug() << "Unknown Option, doing nothing!";
        break;
    }
}
