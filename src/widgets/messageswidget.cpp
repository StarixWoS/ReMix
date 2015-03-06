
#include "includes.hpp"
#include "messageswidget.hpp"
#include "ui_messageswidget.h"

MessagesWidget::MessagesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessagesWidget)
{
    ui->setupUi(this);

    QVariant text = Settings::getMOTDMessage();
    if ( text.toString().isEmpty() )
    {
        text = ui->motdEdit->toPlainText();
        Settings::setMOTDMessage( text );
    }
    else
        ui->motdEdit->setText( text.toString() );

    text = Settings::getBanishMesage();
    if ( text.toString().isEmpty() )
    {
        text = ui->banishedEdit->toPlainText();
        Settings::setBanishMesage( text );
    }
    else
        ui->banishedEdit->setText( text.toString() );

    //Update the MOTD file after the timer has elapsed.
    motdUpdate.setInterval( 10000 ); //Update the file after 10seconds.
    motdUpdate.setSingleShot( true );

    QObject::connect( &motdUpdate, &QTimer::timeout, [=]()
    {
        QVariant var{ ui->motdEdit->toPlainText() };
        Settings::setMOTDMessage( var );
    });

    //Update the MOTD file after the timer has elapsed.
    banMUpdate.setInterval( 10000 ); //Update the file after 10seconds.
    banMUpdate.setSingleShot( true );

    QObject::connect( &banMUpdate, &QTimer::timeout, [=]()
    {
        QVariant var{ ui->banishedEdit->toPlainText() };
        Settings::setBanishMesage( var );
    });
}

MessagesWidget::~MessagesWidget()
{
    delete ui;
}

void MessagesWidget::on_motdEdit_textChanged()
{
    motdUpdate.start();
}

void MessagesWidget::on_banishedEdit_textChanged()
{
    banMUpdate.start();
}
