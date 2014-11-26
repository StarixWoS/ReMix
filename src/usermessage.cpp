
#include "usermessage.hpp"
#include "ui_usermessage.h"

#include "helper.hpp"

UserMessage::UserMessage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserMessage)
{
    ui->setupUi(this);

    //this->setWindowModality( Qt::WindowModal );
}

UserMessage::~UserMessage()
{
    delete ui;
}

void UserMessage::newUserCommentSlot(QString& sernum, QString& alias, QString& message)
{
    QTextEdit* obj = ui->msgView;
    if ( obj == nullptr )
        return;

    quint64 date = QDateTime::currentDateTime().toTime_t();
    QString comment = QString( "\r\n --- \r\n"
                               "%1 \r\n"
                               "SerNum: %2 \r\n"
                               "%3: %4"
                               "\r\n --- \r\n" )
                          .arg( QDateTime::fromTime_t( date ).toString( "ddd MMM dd HH:mm:ss yyyy" ) )
                          .arg( sernum )
                          .arg( alias )
                          .arg( message );

    int curScrlPosMax = obj->verticalScrollBar()->maximum();
    int selStart = 0;
    int selEnd = 0;

    QTextCursor cursor( obj->textCursor() );
    if ( cursor.hasSelection() )
    {
        selStart = cursor.selectionStart();
        selEnd = cursor.selectionEnd();
    }
    cursor.movePosition( QTextCursor::End );
    cursor.insertText( comment );

    if ( selStart && selEnd )
    {
        cursor.setPosition( selStart );
        cursor.setPosition( selEnd, QTextCursor::KeepAnchor );
        obj->setTextCursor( cursor );
    }

    //Detect when the user is scrolling upwards.
    if ( obj->verticalScrollBar()->sliderPosition() == curScrlPosMax )
        obj->verticalScrollBar()->setSliderPosition( obj->verticalScrollBar()->maximum() );

    if ( Helper::getLogComments() )
    {
        ;   //Log incoming comments to: mixComments/DATE_Comments.txt
    }

    //Show the Dialog when a new comment is recieved.
    if ( !this->isVisible() )
        this->show();
}
