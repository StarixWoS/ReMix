
//Class includes.
#include "motdwidget.hpp"
#include "ui_motdwidget.h"

//ReMix includes.
#include "settings.hpp"
#include "server.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QTimer>

QHash<Server*, MOTDWidget*> MOTDWidget::motdWidgets;

MOTDWidget::MOTDWidget() :
    ui(new Ui::MOTDWidget)
{
    ui->setupUi(this);
    this->setWindowModality( Qt::ApplicationModal );

    //Update the MOTD file after the timer has elapsed.
    motdUpdate.setInterval( 2000 ); //Update the file after 2seconds.
    motdUpdate.setSingleShot( true );

    QObject::connect( &motdUpdate, &QTimer::timeout, &motdUpdate,
    [=, this]()
    {
        QString strVar{ ui->motdEdit->toPlainText() };
        Helper::stripNewlines( strVar );

        Settings::setSetting( strVar, SKeys::Setting, SSubKeys::MOTD, serverName );
    } );
}

MOTDWidget::~MOTDWidget()
{
    delete ui;
}

MOTDWidget* MOTDWidget::getInstance(Server* server)
{
    MOTDWidget* widget{ motdWidgets.value( server ) };
    if ( widget == nullptr )
    {
        widget = new MOTDWidget();
        widget->setServerName( server->getServerName() );
        QObject::connect( server, &Server::serverNameChangedSignal, widget, &MOTDWidget::nameChangedSlot );
        motdWidgets.insert( server, widget );
    }
    return widget;
}

void MOTDWidget::deleteInstance(Server* server)
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
    QString text{ Settings::getSetting( SKeys::Setting, SSubKeys::MOTD, name ).toString() };
    if ( text.isEmpty() )
    {
        text = ui->motdEdit->toPlainText();
        Settings::setSetting( text, SKeys::Setting, SSubKeys::MOTD, name );
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

void MOTDWidget::nameChangedSlot(const QString& name)
{
    this->setServerName( name );
}

void MOTDWidget::on_motdEdit_textChanged()
{
    motdUpdate.start();
}
