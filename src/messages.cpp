#include "messages.hpp"
#include "ui_messages.h"

#include <QDebug>
#include "preferences.hpp"

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

bool Messages::isPasswordEnabled()
{
    return ui->reqPasword->isEnabled();
}

bool Messages::cmpPassword(QString& value)
{
    return ui->pwdEdit->text() != value;
}

void Messages::on_saveSettings_clicked()
{
    QVariant motd = ui->motdEdit->toPlainText();
    Preferences::setMOTDMessage( motd );

    QVariant banMsg = ui->banishedEdit->toPlainText();
    Preferences::setBanishMesage( banMsg );

    QVariant pwd = ui->pwdEdit->text();
    Preferences::setPassword( pwd );

    QVariant reqPwd = ui->reqPasword->isChecked();
    Preferences::setRequirePassword( reqPwd );

    QVariant rules = ui->rulesEdit->toPlainText();
    Preferences::setServerRules( rules );

    if ( this->isVisible() )
        this->hide();
}

void Messages::on_reloadSettings_clicked()
{
    QObject* sender = QObject::sender();
    if ( sender != nullptr )
        qDebug() << "reloadSettings" << sender;

    ui->motdEdit->setText( Preferences::getMOTDMessage() );
    ui->banishedEdit->setText( Preferences::getBanishMesage() );
    ui->pwdEdit->setText( Preferences::getPassword() );
    ui->reqPasword->setChecked( Preferences::getRequirePassword() );
    ui->rulesEdit->setText( Preferences::getServerRules() );

    if ( this->isVisible() )
        this->hide();
}
