
#include "includes.hpp"
#include "createinstance.hpp"
#include "ui_createinstance.h"

//Initialize our accepted Command List.
const QString CreateInstance::gameNames[ GAME_NAME_COUNT ] =
{
    "WOS",
    "TOY",
    "RC",
    "WPA"
};

CreateInstance::CreateInstance(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateInstance)
{
    ui->setupUi(this);
    this->show();
}

CreateInstance::~CreateInstance()
{
    this->close();
    this->disconnect();
    this->deleteLater();

    delete ui;
}

QString CreateInstance::getServerArgs() const
{
    return serverArgs;
}

void CreateInstance::setServerArgs(const QString& value)
{
    serverArgs = value;
}

void CreateInstance::on_initializeServer_clicked()
{
    QString svrArgs{ "/Game=%1 /Listen=%2 /Name=%3 /fudge" };
    svrArgs = svrArgs.arg( gameNames[ ui->gameName->currentIndex() ] )
                     .arg( ui->portNumber->text( ))
                     .arg( ui->serverName->text() );
    if ( ui->isPublic->isChecked() )
        svrArgs.append( " /Public");

    this->setServerArgs( svrArgs );
    emit this->accept();
}

void CreateInstance::on_close_clicked()
{
    this->close();
    this->disconnect();
    this->deleteLater();
}

void CreateInstance::closeEvent(QCloseEvent* event)
{
    if ( event == nullptr )
        return;

    if ( event->type() == QEvent::Close )
    {
        this->close();
        this->disconnect();
        this->deleteLater();
    }
    else
        QDialog::closeEvent( event );
}
