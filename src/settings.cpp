
#include "settings.hpp"
#include "ui_settings.h"

#include "adminhelper.hpp"
#include "helper.hpp"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->setWindowModality( Qt::WindowModal );

    //Load Settings from file.
    this->setCheckedState( Options::ReqPwd,         Helper::getRequirePassword() );
    this->setCheckedState( Options::ReqAdminPwd,    AdminHelper::getReqAdminAuth() );
    this->setCheckedState( Options::AllowDupIP,     Helper::getAllowDupedIP() );
    this->setCheckedState( Options::BanDupIP,       Helper::getBanDupedIP() );
    this->setCheckedState( Options::BanHack,        Helper::getBanHackers() );
    this->setCheckedState( Options::ReqSernum,      Helper::getReqSernums() );
    this->setCheckedState( Options::DisconnectIdle, Helper::getDisconnectIdles() );
    this->setCheckedState( Options::AllowSSV,       Helper::getAllowSSV() );
    this->setCheckedState( Options::LogComments,    Helper::getLogComments() );
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
    ui->settingsView->item( row, 0 )->setCheckState( val == Qt::Checked ? Qt::Unchecked : Qt::Checked );
    val = ui->settingsView->item( row, 0 )->checkState();

    QVariant state = val == Qt::Checked;
    switch ( row )
    {
        case Options::ReqPwd:
            {
                QVariant txt = QString{ "" };
                bool ok;

                Helper::setRequirePassword( state );
                if ( Helper::getPassword().isEmpty()
                  && Helper::getRequirePassword() )
                {
                    txt = QInputDialog::getText( this, "Server Password:",
                                                 "Password:", QLineEdit::PasswordEchoOnEdit,
                                                 "", &ok );
                    if ( ok && !txt.toString().isEmpty() )
                        Helper::setPassword( txt, false );
                    else    //Invalid dialog state or no input Password. Reset the Object's state.
                    {
                        ui->settingsView->item( row, 0 )->setCheckState( Qt::Unchecked );

                        state = false;
                        Helper::setRequirePassword( state );
                    }
                }
                else if ( !Helper::getRequirePassword() )
                {
                    int val = QMessageBox::question( this, "Remove Password:",
                                                     "Do you wish to erase the stored Password hash?",
                                                     QMessageBox::Yes | QMessageBox::No,
                                                     QMessageBox::No );
                    if ( val == QMessageBox::Yes )
                        Helper::setPassword( txt, false );
                }
            }
        break;
        case Options::ReqAdminPwd:
            AdminHelper::setReqAdminAuth( state );
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
