
#include "includes.hpp"
#include "messages.hpp"
#include "ui_messages.h"

Messages::Messages(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Messages)
{
    ui->setupUi(this);

    //Remove the "Help" button from the window title bars.
    {
        QIcon icon = this->windowIcon();
        Qt::WindowFlags flags = this->windowFlags();
        flags &= ~Qt::WindowContextHelpButtonHint;

        this->setWindowFlags( flags );
        this->setWindowIcon( icon );

        //this->setWindowModality( Qt::WindowModal );
    }

    if ( Settings::getSaveWindowPositions() )
    {
        this->restoreGeometry( Settings::getWindowPositions(
                                   this->metaObject()->className() ) );
    }

    //Load settings from: Preferences.ini
    QObject::connect( this, &Messages::finished,
                      this, &Messages::on_reloadSettings_clicked );
    emit ui->reloadSettings->clicked();
}

Messages::~Messages()
{
    if ( Settings::getSaveWindowPositions() )
    {
        Settings::setWindowPositions( this->saveGeometry(),
                                      this->metaObject()->className() );
    }
    delete ui;
}

QString Messages::getServerMOTD()
{
    return ui->motdEdit->toPlainText();
}

QString Messages::getBanishMsg()
{
    return ui->banishedEdit->toPlainText();
}

void Messages::on_saveSettings_clicked()
{
    QVariant motd = ui->motdEdit->toPlainText();
    Settings::setMOTDMessage( motd );

    QVariant banMsg = ui->banishedEdit->toPlainText();
    Settings::setBanishMesage( banMsg );

    QVariant rules = ui->rulesEdit->toPlainText();
    Settings::setServerRules( rules );

    if ( this->isVisible() )
        this->hide();
}

void Messages::on_reloadSettings_clicked()
{
    QString var = Settings::getMOTDMessage();
    if ( !var.isEmpty() )
        ui->motdEdit->setText( var );

    var = Settings::getBanishMesage();
    if ( !var.isEmpty() )
        ui->banishedEdit->setText( var );

    var = Settings::getServerRules();
    if ( !var.isEmpty() )
        ui->rulesEdit->setText( var );

    if ( this->isVisible() )
        this->hide();
}
