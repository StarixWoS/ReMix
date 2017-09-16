
//Class includes.
#include "motdwidget.hpp"
#include "ui_motdwidget.h"

//ReMix includes.
#include "settings.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QTimer>

MOTDWidget::MOTDWidget() :
    ui(new Ui::MOTDWidget)
{
    ui->setupUi(this);

    //Update the MOTD file after the timer has elapsed.
    motdUpdate.setInterval( 10000 ); //Update the file after 10seconds.
    motdUpdate.setSingleShot( true );

    QObject::connect( &motdUpdate, &QTimer::timeout, &motdUpdate,
    [=]()
    {
        QString strVar{ ui->motdEdit->toPlainText() };
        Helper::stripNewlines( strVar );

        QVariant var{ strVar };
        Settings::setMOTDMessage( var, serverName );
    });
}

MOTDWidget::~MOTDWidget()
{
    delete ui;
}

void MOTDWidget::setServerName(QString name)
{
    QVariant text = Settings::getMOTDMessage( name );
    if ( text.toString().isEmpty() )
    {
        text = ui->motdEdit->toPlainText();
        Settings::setMOTDMessage( text, name );
    }
    else
        ui->motdEdit->setText( text.toString() );

    serverName = name;
}

void MOTDWidget::on_motdEdit_textChanged()
{
    motdUpdate.start();
}
