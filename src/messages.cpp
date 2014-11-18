#include "messages.hpp"
#include "ui_messages.h"

#include <QDebug>
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

bool Messages::isPasswordEnabled()
{
    return ui->reqPasword->isEnabled();
}

bool Messages::cmpPassword(QString& value)
{
    value = QCryptographicHash::hash( value.toLatin1(), QCryptographicHash::Sha3_512 ).toHex();
    return Helper::getPassword() != value;
}

void Messages::on_saveSettings_clicked()
{
    QVariant motd = ui->motdEdit->toPlainText();
    Helper::setMOTDMessage( motd );

    QVariant banMsg = ui->banishedEdit->toPlainText();
    Helper::setBanishMesage( banMsg );

    QVariant pwd = ui->pwdEdit->text();
    pwdHashed = Helper::setPassword( pwd, pwdHashed );

    QVariant reqPwd = ui->reqPasword->isChecked();
    Helper::setRequirePassword( reqPwd );

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

    QVariant qVar = Helper::getPassword();
    if ( !qVar.toString().isEmpty() && qVar.toString().length() < 128 )
        pwdHashed = Helper::setPassword( qVar, pwdHashed );

    ui->pwdEdit->setText( Helper::getPassword() );

    var = Helper::getServerRules();
    if ( !var.isEmpty() )
        ui->rulesEdit->setText( Helper::getServerRules() );

    ui->reqPasword->setChecked( Helper::getRequirePassword() );

    if ( this->isVisible() )
        this->hide();
}

void Messages::on_pwdEdit_textEdited(const QString&)
{
    pwdHashed = false;
}
