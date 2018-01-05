
//Class includes.
#include "motdwidget.hpp"
#include "ui_motdwidget.h"

//ReMix includes.
#include "serverinfo.hpp"
#include "settings.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QTimer>

QHash<ServerInfo*, MOTDWidget*> MOTDWidget::motdWidgets;

MOTDWidget::MOTDWidget(const QString& name) :
    ui(new Ui::MOTDWidget)
{
    ui->setupUi(this);
    this->setServerName( name );

    //Update the MOTD file after the timer has elapsed.
    motdUpdate.setInterval( 10000 ); //Update the file after 10seconds.
    motdUpdate.setSingleShot( true );

    QObject::connect( &motdUpdate, &QTimer::timeout, &motdUpdate,
    [=]()
    {
        QString strVar{ ui->motdEdit->toPlainText() };
        Helper::stripNewlines( strVar );

        Settings::setMOTDMessage( strVar, serverName );
    });
}

MOTDWidget::~MOTDWidget()
{
    delete ui;
}

MOTDWidget* MOTDWidget::getWidget(ServerInfo* server)
{
    MOTDWidget* widget{ motdWidgets.value( server ) };
    if ( widget == nullptr )
    {
        widget = new MOTDWidget( server->getName() );
        motdWidgets.insert( server, widget );
    }
    return widget;
}

void MOTDWidget::deleteWidget(ServerInfo* server)
{
    MOTDWidget* widget{ motdWidgets.take( server ) };
    if ( widget != nullptr )
    {
        widget->setParent( nullptr );
        widget->deleteLater();
    }
}

void MOTDWidget::setServerName(const QString& name)
{
    QString text = Settings::getMOTDMessage( name );
    if ( text.isEmpty() )
    {
        text = ui->motdEdit->toPlainText();
        Settings::setMOTDMessage( text, name );
    }
    else
        ui->motdEdit->setText( text );

    serverName = name;
}

void MOTDWidget::changeMotD(const QString& message)
{
    ui->motdEdit->setText( message );
    motdUpdate.start();
}

void MOTDWidget::on_motdEdit_textChanged()
{
    motdUpdate.start();
}
