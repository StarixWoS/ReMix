
//Class includes.
#include "commentviewwidget.hpp"
#include "ui_commentviewwidget.h"

//ReMix includes.
#include "settings.hpp"
#include "helper.hpp"
#include "logger.hpp"

//Qt Includes.
#include <QScrollBar>
#include <QtCore>

CommentViewWidget::CommentViewWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CommentViewWidget)
{
    ui->setupUi(this);
}

CommentViewWidget::~CommentViewWidget()
{
    delete ui;
}

void CommentViewWidget::setTitle(const QString& name)
{
    if ( !name.isEmpty() )
        this->setWindowTitle( "Server Comments: [ " % name % " ]" );
}

void CommentViewWidget::insertComment(const QString& sernum,
                                      const QString& alias,
                                      const QString& message)
{
    QTextEdit* obj = ui->msgView;
    if ( obj == nullptr )
        return;

    uint date = QDateTime::currentDateTime()
                     .toTime_t();
    QString comment = QString( "\r\n --- \r\n"
                               "%1 \r\n"
                               "SerNum: %2 \r\n"
                               "%3: %4"
                               "\r\n --- \r\n" )
                          .arg( Helper::getTimeAsString( date ) )
                          .arg( sernum )
                          .arg( alias )
                          .arg( message );

    int curScrlPosMax = obj->verticalScrollBar()->maximum();
    int selStart{ 0 };
    int selEnd{ 0 };

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
    {
        obj->verticalScrollBar()->setSliderPosition(
                    obj->verticalScrollBar()->maximum() );
    }

    //Helper::logToFile( Helper::COMMENT, comment, false, false );
}