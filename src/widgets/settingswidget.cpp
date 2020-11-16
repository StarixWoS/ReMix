
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

SettingsWidget* SettingsWidget::instance{ nullptr };

SettingsWidget::SettingsWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);

    //Load Settings from file.
    this->setCheckedState( SToggles::SaveWindowPositions, Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() );
    this->setCheckedState( SToggles::InformAdminLogin, Settings::getSetting( SKeys::Setting, SSubKeys::InformAdminLogin ).toBool() );
    this->setCheckedState( SToggles::DCBlueCode, Settings::getSetting( SKeys::Setting, SSubKeys::DCBlueCodedSerNums ).toBool() );
    this->setCheckedState( SToggles::MinimizeToTray, Settings::getSetting( SKeys::Setting, SSubKeys::MinimizeToTray ).toBool() );
    this->setCheckedState( SToggles::EchoComments, Settings::getSetting( SKeys::Setting, SSubKeys::EchoComments ).toBool() );
    this->setCheckedState( SToggles::FwdComments, Settings::getSetting( SKeys::Setting, SSubKeys::FwdComments ).toBool() );
    this->setCheckedState( SToggles::LogComments, Settings::getSetting( SKeys::Logger, SSubKeys::LogComments ).toBool() );
    this->setCheckedState( SToggles::AllowDupeIP, Settings::getSetting( SKeys::Setting, SSubKeys::AllowDupe ).toBool() );
    this->setCheckedState( SToggles::ReqSerNum, Settings::getSetting( SKeys::Setting, SSubKeys::ReqSerNum ).toBool() );
    this->setCheckedState( SToggles::BanDupeIP, Settings::getSetting( SKeys::Setting, SSubKeys::BanDupes ).toBool() );
    this->setCheckedState( SToggles::AllowSSV, Settings::getSetting( SKeys::Setting, SSubKeys::AllowSSV ).toBool() );
    this->setCheckedState( SToggles::DCIdles, Settings::getSetting( SKeys::Setting, SSubKeys::AllowIdle ).toBool() );
    this->setCheckedState( SToggles::LogFiles, Settings::getSetting( SKeys::Logger, SSubKeys::LogFiles ).toBool() );

    QString dir{ Settings::getSetting( SKeys::Setting, SSubKeys::WorldDir ).toString() };
    QString rowText{ "World Dir: [ %1 ]" };
    if ( !dir.isEmpty() )
        rowText = rowText.arg( dir );
    else
        rowText = rowText.arg( "" );

    ui->settingsView->item( static_cast<int>( SToggles::WorldDir ), 0 )->setText( rowText );
    this->setCheckedState( SToggles::WorldDir, !dir.isEmpty() );

    QString masterIPOverride{ Settings::getSetting( SKeys::Setting, SSubKeys::OverrideMasterIP ).toString() };
    rowText = "Master IP Address: [ %1 ]";

    if ( !masterIPOverride.isEmpty() )
        rowText = rowText.arg( masterIPOverride );
    else
        rowText = rowText.arg( "" );

    ui->settingsView->item( static_cast<int>( SToggles::OverrideMaster ), 0 )->setText( rowText );
    this->setCheckedState( SToggles::OverrideMaster, !masterIPOverride.isEmpty() );

    QString masterAddressOverride{ Settings::getSetting( SKeys::Setting, SSubKeys::OverrideMasterHost ).toString() };
    rowText = "Master Host Address: [ %1 ]";

    if ( !masterAddressOverride.isEmpty() )
        rowText = rowText.arg( masterAddressOverride );
    else
        rowText = rowText.arg( "" );

    ui->settingsView->item( static_cast<int>( SToggles::OverrideMasterHost ), 0 )->setText( rowText );
    this->setCheckedState( SToggles::OverrideMasterHost, !masterAddressOverride.isEmpty() );
}

SettingsWidget::~SettingsWidget()
{
    instance->disconnect();
    instance->deleteLater();

    delete ui;
}

void SettingsWidget::setCheckedState(const SToggles& option, const bool& val)
{
    Qt::CheckState state;
    if ( val )
        state = Qt::Checked;
    else
        state = Qt::Unchecked;

    QTableWidgetItem* item{ ui->settingsView->item( static_cast<int>( option ), 0 ) };
    if ( item != nullptr )
    {
        ui->settingsView->item( static_cast<int>( option ), 0 )->setCheckState( state );
        stateMap.insert( item, state );
    }
}

bool SettingsWidget::getCheckedState(const SToggles& option)
{
    return ui->settingsView->item( static_cast<int>( option ), 0 )->checkState() == Qt::Checked;
}

SettingsWidget* SettingsWidget::getInstance(QWidget* parent)
{
    if ( instance == nullptr )
        instance = new SettingsWidget( parent );

    return instance;
}

void SettingsWidget::setInstance(SettingsWidget* value)
{
    instance = value;
}

void SettingsWidget::on_settingsView_itemClicked(QTableWidgetItem* item)
{
    if ( item != nullptr )
    {
        if ( stateMap.value( item ) != item->checkState() )
        {
            if ( item->checkState() == Qt::Checked
              || item->checkState() == Qt::Unchecked )
            {
                this->toggleSettings( item->row(), item->checkState() );
                stateMap.insert( item, item->checkState() );
            }
        }
    }
}

void SettingsWidget::on_settingsView_doubleClicked(const QModelIndex& index)
{
    int row{ index.row() };
    this->toggleSettingsModel( row );

}

void SettingsWidget::toggleSettingsModel(const qint32 &row)
{
    QTableWidgetItem* item{ ui->settingsView->item( row, 0 ) };
    if ( item != nullptr )
    {
        Qt::CheckState val{ item->checkState() };
        Qt::CheckState state{ val == Qt::Checked ? Qt::Unchecked : Qt::Checked };

        item->setCheckState( state );
        stateMap.insert( item, item->checkState() );

        val = state;
        this->toggleSettings( row, val );
    }
}

void SettingsWidget::toggleSettings(const qint32& row, Qt::CheckState value)
{
    bool state{ value == Qt::Checked };

    QString title{ "" };
    QString prompt{ "" };
    QString rowText{ "" };
    QString newMasterIP{ "" };

    switch ( static_cast<SToggles>( row ) )
    {
        case SToggles::AllowDupeIP: //0
            Settings::setSetting( state, SKeys::Setting, SSubKeys::AllowDupe );
            if ( state )
            {
                if ( this->getCheckedState( SToggles::BanDupeIP ) )
                    this->toggleSettingsModel( static_cast<int>( SToggles::BanDupeIP ) );
            }
        break;
        case SToggles::BanDupeIP: //1
            Settings::setSetting( state, SKeys::Setting, SSubKeys::BanDupes );
            if ( state )
            {
                if ( this->getCheckedState( SToggles::AllowDupeIP ) )
                    this->toggleSettingsModel( static_cast<int>( SToggles::AllowDupeIP ) );
            }
        break;
        case SToggles::ReqSerNum: //2
            Settings::setSetting( state, SKeys::Setting, SSubKeys::ReqSerNum );
        break;
        case SToggles::DCBlueCode: //3
            Settings::setSetting( state, SKeys::Setting, SSubKeys::DCBlueCodedSerNums );
        break;
        case SToggles::DCIdles: //4
            Settings::setSetting( state, SKeys::Setting, SSubKeys::AllowIdle );
        break;
        case SToggles::AllowSSV: //5
            Settings::setSetting( state, SKeys::Setting, SSubKeys::AllowSSV );
        break;
        case SToggles::LogComments: //6
            Settings::setSetting( state, SKeys::Logger, SSubKeys::LogComments );
        break;
        case SToggles::FwdComments: //7
            Settings::setSetting( state, SKeys::Setting, SSubKeys::FwdComments );
        break;
        case SToggles::EchoComments: //8
            Settings::setSetting( state, SKeys::Setting, SSubKeys::EchoComments );
        break;
        case SToggles::InformAdminLogin: //9
            Settings::setSetting( state, SKeys::Setting, SSubKeys::InformAdminLogin );
        break;
        case SToggles::MinimizeToTray: //10
            Settings::setSetting( state, SKeys::Setting, SSubKeys::MinimizeToTray );
        break;
        case SToggles::SaveWindowPositions: //11
            Settings::setSetting( state, SKeys::Setting, SSubKeys::SaveWindowPositions );
        break;
        case SToggles::LogFiles: //12
            Settings::setSetting( state, SKeys::Logger, SSubKeys::LogFiles );
        break;
        case SToggles::WorldDir: //13
            {
                QString directory{ Settings::getSetting( SKeys::Setting, SSubKeys::WorldDir ).toString() };
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
                            ui->settingsView->item( static_cast<int>( SToggles::WorldDir ), 0 )->setCheckState( Qt::Checked );
                        }
                    }

                    if ( !state )
                    {
                        directory = "";
                        title = "Invalid Directory:";
                        prompt = "You have selected an invalid world directory. Please try again.";

                        ui->settingsView->item( static_cast<int>( SToggles::WorldDir ), 0 )->setCheckState( Qt::Unchecked );

                        Helper::warningMessage( this, title, prompt );
                    }
                    rowText = rowText.arg( directory );
                    ui->settingsView->item( static_cast<int>( SToggles::WorldDir ), 0 )->setText( rowText );

                    Settings::setSetting( directory, SKeys::Setting, SSubKeys::WorldDir );
                }
            }
        break;
        case SToggles::OverrideMasterHost:
            {
                QString ipAddress{ Settings::getSetting( SKeys::Setting, SSubKeys::OverrideMasterHost ).toString() };
                bool ok{ false };

                if ( state )
                {
                    if ( ipAddress.isEmpty() )
                    {
                        title = "Master Host Address:";
                        prompt = "Url Address:";

                        ipAddress = Helper::getTextResponse( this, title, prompt, "", &ok, MessageBox::SingleLine );
                    }

                    if ( ipAddress.isEmpty() || !ok )
                    {
                        ui->settingsView->item( row, 0 )->setCheckState( Qt::Unchecked );
                    }
                    else
                    {
                        ui->settingsView->item( row, 0 )->setCheckState( Qt::Checked );
                        Settings::setSetting( ipAddress, SKeys::Setting, SSubKeys::OverrideMasterHost );
                    }
                }
                else if ( !ipAddress.isEmpty() )
                {
                    ipAddress = "";

                    ui->settingsView->item( row, 0 )->setCheckState( Qt::Unchecked );
                    Settings::setSetting( ipAddress, SKeys::Setting, SSubKeys::OverrideMasterHost );
                }

                rowText = "Master Host Address: [ %1 ]";
                rowText = rowText.arg( ipAddress );
                ui->settingsView->item( row, 0 )->setText( rowText );

                emit this->masterMixInfoChangedSignal();
            }
        break;
        case SToggles::OverrideMaster:
            {
                QString ipAddress{ Settings::getSetting( SKeys::Setting, SSubKeys::OverrideMasterIP ).toString() };
                bool ok{ false };

                if ( state )
                {
                    if ( ipAddress.isEmpty() )
                    {
                        title = "Master IP Address:";
                        prompt = "IP:Port:";

                        ipAddress = Helper::getTextResponse( this, title, prompt, "", &ok, MessageBox::SingleLine );
                    }

                    if ( ipAddress.isEmpty() || !ok )
                    {
                        ui->settingsView->item( row, 0 )->setCheckState( Qt::Unchecked );
                        state = false;
                    }
                    else
                        Settings::setSetting( ipAddress, SKeys::Setting, SSubKeys::OverrideMasterIP );
                }
                else if ( !ipAddress.isEmpty() )
                {
                    ipAddress = "";

                    ui->settingsView->item( row, 0 )->setCheckState( Qt::Unchecked );
                    Settings::setSetting( ipAddress, SKeys::Setting, SSubKeys::OverrideMasterIP );
                }

                rowText = "Master IP Address: [ %1 ]";
                rowText = rowText.arg( Settings::getSetting( SKeys::Setting, SSubKeys::OverrideMasterIP ).toString() );
                ui->settingsView->item( row, 0 )->setText( rowText );

                emit this->masterMixIPChangedSignal();
            }
        break;
    }
}
