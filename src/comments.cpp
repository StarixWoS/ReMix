
//Class includes.
#include "comments.hpp"
#include "ui_comments.h"

//ReMix includes.
#include "serverinfo.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "logger.hpp"

//Qt Includes.
#include <QScrollBar>
#include <QtCore>

Comments::Comments(QWidget* parent, ServerInfo* serverInfo) :
    QDialog(parent),
    ui(new Ui::Comments)
{
    ui->setupUi(this);

    server = serverInfo;

    if ( Settings::getSaveWindowPositions() )
    {
        QByteArray geometry{ Settings::getWindowPositions(
                                    this->metaObject()->className() ) };
        if ( !geometry.isEmpty() )
        {
            this->restoreGeometry( Settings::getWindowPositions(
                                       this->metaObject()->className() ) );
        }
    }
}

Comments::~Comments()
{
    if ( Settings::getSaveWindowPositions() )
    {
        Settings::setWindowPositions( this->saveGeometry(),
                                      this->metaObject()->className() );
    }
    delete ui;
}

void Comments::setTitle(const QString& name)
{
    if ( !name.isEmpty() )
        this->setWindowTitle( "Server Comments: [ " % name % " ]" );
}

void Comments::newUserCommentSlot(const QString& sernum, const QString& alias,
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
                          .arg( Helper::getTimeAsString( date ),
                                sernum,
                                alias,
                                message );

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
    Logger::getInstance()->insertLog( server->getName(), comment,
                                      LogTypes::COMMENT, true, true );

    //Show the Dialog when a new comment is received.
    if ( !this->isVisible() )
        this->show();
}
