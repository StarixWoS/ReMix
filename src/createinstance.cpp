
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
    QStringList oldServers{ Settings::prefs->childGroups() };

    QString name{ "" };
    bool skip{ false };
    for ( int i = 0; i < oldServers.count(); ++i )
    {
        name = oldServers.at( i );
        for ( int i = 0; i < SETTINGS_KEY_COUNT; ++i )
        {
            if ( name.compare( Settings::keys[ i ],
                               Qt::CaseInsensitive ) == 0 )
            {
                skip = true;
            }
        }

        if ( !skip )
            ui->oldServers->addItem( name );

        skip = false;
    }
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

QString CreateInstance::getServerName() const
{
    return ui->serverName->text();
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

void CreateInstance::on_oldServers_currentIndexChanged(int)
{
    ui->serverName->setText( ui->oldServers->currentText() );
}
