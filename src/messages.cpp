
#include "messages.hpp"
#include "ui_messages.h"

#include "helper.hpp"

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

        this->setWindowModality( Qt::WindowModal );
    }

    //Load settings from: Preferences.ini
    QObject::connect( this, &Messages::finished,
                      this, &Messages::on_reloadSettings_clicked );
    emit ui->reloadSettings->clicked();
}

Messages::~Messages()
{
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
    Helper::setMOTDMessage( motd );

    QVariant banMsg = ui->banishedEdit->toPlainText();
    Helper::setBanishMesage( banMsg );

    QVariant rules = ui->rulesEdit->toPlainText();
    Helper::setServerRules( rules );

    if ( this->isVisible() )
        this->hide();
}

void Messages::on_reloadSettings_clicked()
{
    QString var = Helper::getMOTDMessage();
    if ( !var.isEmpty() )
        ui->motdEdit->setText( var );

    var = Helper::getBanishMesage();
    if ( !var.isEmpty() )
        ui->banishedEdit->setText( var );

    var = Helper::getServerRules();
    if ( !var.isEmpty() )
        ui->rulesEdit->setText( var );

    if ( this->isVisible() )
        this->hide();
}
