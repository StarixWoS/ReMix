
//Class includes.
#include "settingswidget.hpp"
#include "ui_settingswidget.h"

//ReMix includes.
#include "settings.hpp"
#include "helper.hpp"
#include "remix.hpp"

//Qt Includes.
#include <QNetworkAccessManager>
#include <QFileDialog>
#include <QtCore>

SettingsWidget::SettingsWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);

    //Load Settings from file.
    this->setCheckedState( Toggles::SAVEWINDOWPOSITIONS, Settings::getSetting( SettingKeys::Setting, SettingSubKeys::SaveWindowPositions ).toBool() );
    this->setCheckedState( Toggles::DCBLUECODEDSERNUMS, Settings::getSetting( SettingKeys::Setting, SettingSubKeys::DCBlueCodedSerNums ).toBool() );
    this->setCheckedState( Toggles::INFORMADMINLOGIN, Settings::getSetting( SettingKeys::Setting, SettingSubKeys::InformAdminLogin ).toBool() );
    this->setCheckedState( Toggles::MINIMIZETOTRAY, Settings::getSetting( SettingKeys::Setting, SettingSubKeys::MinimizeToTray ).toBool() );
    this->setCheckedState( Toggles::DISCONNECTIDLES, Settings::getSetting( SettingKeys::Setting, SettingSubKeys::AllowIdle ).toBool() );
    this->setCheckedState( Toggles::ECHOCOMMENTS, Settings::getSetting( SettingKeys::Setting, SettingSubKeys::EchoComments ).toBool() );
    this->setCheckedState( Toggles::FWDCOMMENTS, Settings::getSetting( SettingKeys::Setting, SettingSubKeys::FwdComments ).toBool() );
    this->setCheckedState( Toggles::ALLOWDUPEDIP, Settings::getSetting( SettingKeys::Setting, SettingSubKeys::AllowDupe ).toBool() );
    this->setCheckedState( Toggles::LOGCOMMENTS, Settings::getSetting( SettingKeys::Logger, SettingSubKeys::LogComments ).toBool() );
    this->setCheckedState( Toggles::BANDUPEDIP, Settings::getSetting( SettingKeys::Setting, SettingSubKeys::BanDupes ).toBool() );
    this->setCheckedState( Toggles::REQSERNUM, Settings::getSetting( SettingKeys::Setting, SettingSubKeys::ReqSerNum ).toBool() );
    this->setCheckedState( Toggles::ALLOWSSV, Settings::getSetting( SettingKeys::Setting, SettingSubKeys::AllowSSV ).toBool() );
    this->setCheckedState( Toggles::LOGFILES, Settings::getSetting( SettingKeys::Logger, SettingSubKeys::LogFiles ).toBool() );

    QString dir{ Settings::getSetting( SettingKeys::Setting, SettingSubKeys::WorldDir ).toString() };
    QString rowText{ "World Dir: [ %1 ]" };
    if ( !dir.isEmpty() )
        rowText = rowText.arg( dir );
    else
        rowText = rowText.arg( "" );

    ui->settingsView->item( Toggles::WORLDDIR, 0 )->setText( rowText );
    this->setCheckedState( Toggles::WORLDDIR, !dir.isEmpty() );
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::setCheckedState(const Toggles& option, const bool& val)
{
    Qt::CheckState state;
    if ( val )
        state = Qt::Checked;
    else
        state = Qt::Unchecked;

    QTableWidgetItem* item = ui->settingsView->item( static_cast<int>( option ), 0 );
    if ( item != nullptr )
        ui->settingsView->item( static_cast<int>( option ), 0 )->setCheckState( state );
}

bool SettingsWidget::getCheckedState(const Toggles& option)
{
    return ui->settingsView->item( option, 0 )->checkState() == Qt::Checked;
}

void SettingsWidget::on_settingsView_itemClicked(QTableWidgetItem* item)
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

void SettingsWidget::on_settingsView_doubleClicked(const QModelIndex& index)
{
    int row = index.row();
    this->toggleSettingsModel( row );

}

void SettingsWidget::toggleSettingsModel(const qint32 &row)
{
    Qt::CheckState val = ui->settingsView->item( row, 0 )->checkState();
    ui->settingsView->item( row, 0 )->setCheckState( val == Qt::Checked ? Qt::Unchecked : Qt::Checked );

    val = ui->settingsView->item( row, 0 )->checkState();
    this->toggleSettings( row, val );
}

void SettingsWidget::toggleSettings(const qint32& row, Qt::CheckState value)
{
    bool state = value == Qt::Checked;

    QString title{ "" };
    QString prompt{ "" };

    switch ( static_cast<Toggles>( row ) )
    {
        case Toggles::ALLOWDUPEDIP: //0
            Settings::setSetting( state, SettingKeys::Setting, SettingSubKeys::AllowDupe );
            if ( state )
            {
                if ( this->getCheckedState( Toggles::BANDUPEDIP ) )
                    this->toggleSettingsModel( Toggles::BANDUPEDIP );
            }
        break;
        case Toggles::BANDUPEDIP: //1
            Settings::setSetting( state, SettingKeys::Setting, SettingSubKeys::BanDupes );
            if ( state )
            {
                if ( this->getCheckedState( Toggles::ALLOWDUPEDIP ) )
                    this->toggleSettingsModel( Toggles::ALLOWDUPEDIP );
            }
        break;
        case Toggles::REQSERNUM: //2
            Settings::setSetting( state, SettingKeys::Setting, SettingSubKeys::ReqSerNum );
        break;
        case Toggles::DCBLUECODEDSERNUMS: //3
            Settings::setSetting( state, SettingKeys::Setting, SettingSubKeys::DCBlueCodedSerNums );
        break;
        case Toggles::DISCONNECTIDLES: //4
            Settings::setSetting( state, SettingKeys::Setting, SettingSubKeys::AllowIdle );
        break;
        case Toggles::ALLOWSSV: //5
            Settings::setSetting( state, SettingKeys::Setting, SettingSubKeys::AllowSSV );
        break;
        case Toggles::LOGCOMMENTS: //6
            Settings::setSetting( state, SettingKeys::Logger, SettingSubKeys::LogComments );
        break;
        case Toggles::FWDCOMMENTS: //7
            Settings::setSetting( state, SettingKeys::Setting, SettingSubKeys::FwdComments );
        break;
        case Toggles::ECHOCOMMENTS: //8
            Settings::setSetting( state, SettingKeys::Setting, SettingSubKeys::EchoComments );
        break;
        case Toggles::INFORMADMINLOGIN: //9
            Settings::setSetting( state, SettingKeys::Setting, SettingSubKeys::InformAdminLogin );
        break;
        case Toggles::MINIMIZETOTRAY: //10
            Settings::setSetting( state, SettingKeys::Setting, SettingSubKeys::MinimizeToTray );
        break;
        case Toggles::SAVEWINDOWPOSITIONS: //11
            Settings::setSetting( state, SettingKeys::Setting, SettingSubKeys::SaveWindowPositions );
        break;
        case Toggles::LOGFILES: //12
            Settings::setSetting( state, SettingKeys::Logger, SettingSubKeys::LogFiles );
        break;
        case Toggles::WORLDDIR: //13
            {
                QString directory{ Settings::getSetting( SettingKeys::Setting, SettingSubKeys::WorldDir ).toString() };
                QString rowText{ "World Dir: [ %1 ]" };

                bool reUse{ false };
                if ( !directory.isEmpty() )
                {
                    title = "Re-Select Directory:";
                    prompt = "Do you wish to re-select the world directory?";

                    reUse = Helper::confirmAction( this, title, prompt );
                }

                if ( directory.isEmpty() || reUse )
                {
                    QString title{ "Select WoS Directory" };
                    directory = QFileDialog::getExistingDirectory( this, title, "/worlds", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

                    state = false;
                    if ( Helper::strContainsStr( directory, "worlds" ) )
                    {
                        if ( directory.endsWith( "/worlds", Qt::CaseInsensitive ) )
                        {
                            state = true;
                            ui->settingsView->item( Toggles::WORLDDIR, 0 )->setCheckState( Qt::Checked );
                        }
                    }

                    if ( !state )
                    {
                        directory = "";
                        title = "Invalid Directory:";
                        prompt = "You have selected an invalid world directory. Please try again.";

                        ui->settingsView->item( Toggles::WORLDDIR, 0 )->setCheckState( Qt::Unchecked );

                        Helper::warningMessage( this, title, prompt );
                    }
                    rowText = rowText.arg( directory );
                    ui->settingsView->item( Toggles::WORLDDIR, 0 )->setText( rowText );

                    Settings::setSetting( directory, SettingKeys::Setting, SettingSubKeys::WorldDir );
                }
            }
        break;
    }
}
