
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

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &Comments::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        this->restoreGeometry( Settings::getSetting( SKeys::Positions, this->metaObject()->className() ).toByteArray() );
}

Comments::~Comments()
{
    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        Settings::setSetting( this->saveGeometry(), SKeys::Positions, this->metaObject()->className() );

    delete ui;
}

void Comments::setTitle(const QString& name)
{
    if ( !name.isEmpty() )
        this->setWindowTitle( "Server Comments: [ " % name % " ]" );
}

void Comments::newUserCommentSlot(const QString& sernum, const QString& alias, const QString& message)
{
    QTextEdit* obj{ ui->msgView };
    if ( obj == nullptr )
        return;

    quint64 date{ QDateTime::currentDateTimeUtc().toTime_t() };

    QString comment{ "%1 [ %2 ]: %3\r\n" };
            comment = comment.arg( alias )
                             .arg( sernum )
                             .arg( message );
    QString time{ "[ " % Helper::getTimeAsString( date ) % " ] " };

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
    cursor.insertText( time % comment );

    if ( selStart && selEnd )
    {
        cursor.setPosition( selStart );
        cursor.setPosition( selEnd, QTextCursor::KeepAnchor );
        obj->setTextCursor( cursor );
    }

    //Detect when the user is scrolling upwards.
    if ( obj->verticalScrollBar()->sliderPosition() == curScrlPosMax )
        obj->verticalScrollBar()->setSliderPosition( obj->verticalScrollBar()->maximum() );

    //Log comments only when enabled.
    if ( Settings::getSetting( SKeys::Logger, SSubKeys::LogComments ).toBool() )
        emit this->insertLogSignal( server->getServerName(), comment, LogTypes::COMMENT, true, false );

    emit this->newUserCommentSignal( comment.simplified() );
    //Show the Dialog when a new comment is received.
    if ( !this->isVisible() )
        this->show();
}
