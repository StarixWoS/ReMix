
#include "messages.hpp"
#include "ui_messages.h"

#include "helper.hpp"

Messages::Messages(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Messages)
{
    ui->setupUi(this);

    //Load settings from: Preferences.ini
    QObject::connect( this, &Messages::finished,
                      this, &Messages::on_reloadSettings_clicked );
    emit ui->reloadSettings->clicked();

    this->setWindowModality( Qt::WindowModal );
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
        ui->motdEdit->setText( Helper::getMOTDMessage() );

    var = Helper::getBanishMesage();
    if ( !var.isEmpty() )
        ui->banishedEdit->setText( Helper::getBanishMesage() );

    var = Helper::getServerRules();
    if ( !var.isEmpty() )
        ui->rulesEdit->setText( Helper::getServerRules() );

    if ( this->isVisible() )
        this->hide();
}
