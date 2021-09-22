
#include "includes.hpp"
#include "messageswidget.hpp"
#include "ui_messageswidget.h"

MessagesWidget::MessagesWidget(QString svrID) :
    ui(new Ui::MessagesWidget)
{
    ui->setupUi(this);
    serverID = svrID;

    QVariant text = Settings::getMOTDMessage( serverID );
    if ( text.toString().isEmpty() )
    {
        text = ui->motdEdit->toPlainText();
        Settings::setMOTDMessage( text, serverID );
    }
    else
        ui->motdEdit->setText( text.toString() );

    text = Settings::getBanishMesage( serverID );
    if ( text.toString().isEmpty() )
    {
        text = ui->banishedEdit->toPlainText();
        Settings::setBanishMesage( text, serverID );
    }
    else
        ui->banishedEdit->setText( text.toString() );

    //Update the MOTD file after the timer has elapsed.
    motdUpdate.setInterval( 10000 ); //Update the file after 10seconds.
    motdUpdate.setSingleShot( true );

    QObject::connect( &motdUpdate, &QTimer::timeout, [=, this]()
    {
        QString strVar{ ui->motdEdit->toPlainText() };
        Helper::stripNewlines( strVar );

        QVariant var{ strVar };
        Settings::setMOTDMessage( var, serverID );
    });

    //Update the MOTD file after the timer has elapsed.
    banMUpdate.setInterval( 10000 ); //Update the file after 10seconds.
    banMUpdate.setSingleShot( true );

    QObject::connect( &banMUpdate, &QTimer::timeout, [=, this]()
    {
        QString strVar{ ui->motdEdit->toPlainText() };
        Helper::stripNewlines( strVar );

        QVariant var{ strVar };
        Settings::setBanishMesage( var, serverID );
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
