#include "chatview.hpp"
#include "includes.hpp"
#include "ui_chatview.h"

ChatView::ChatView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatView)
{
    ui->setupUi(this);

    pktForge = PacketForge::getInstance();

    //Remove the "Help" button from the window title bars.
    {
        QIcon icon = this->windowIcon();
        Qt::WindowFlags flags = this->windowFlags();
        flags &= ~Qt::WindowContextHelpButtonHint;

        this->setWindowFlags( flags );
        this->setWindowIcon( icon );
    }

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

ChatView::~ChatView()
{
    if ( Settings::getSaveWindowPositions() )
    {
        Settings::setWindowPositions( this->saveGeometry(),
                                      this->metaObject()->className() );
    }
    delete ui;
}

void ChatView::setTitle(QString name)
{
    if ( !name.isEmpty() )
        this->setWindowTitle( "Chat View: [ " % name % " ]" );
}

void ChatView::parsePacket(QString& packet)
{
    QString pkt{ pktForge->decryptPacket( packet ) };
    if ( !pkt.isEmpty() )
    {
        if ( pkt.at( 3 ) == 'C' )
        {
            this->parseChatEffect( pkt );
        }
    }
}

void ChatView::parseChatEffect(QString packet)
{
    QString srcSerNum = packet.left( 12 ).mid( 4 );
            srcSerNum = Helper::serNumToIntStr( srcSerNum );

    QString fltrCode = packet.mid( 13 ).left( 2 );
    qint32 code{ (fltrCode.at( 0 ).toLatin1() - 'A') & 0xFF };
    if ( code == 3 || code == 5 || code == 6 || code == 10 )
    {
        //1 = Level-Up and Dice Roll
        //3 = Learn Spell.
        //6 = Pet Command.
        //5 = Unknown
        //10 = Scene Message
        //11 = PK Attack
        return;
    }

    if ( packet.at( 3 ) == 'C' )
    {
        QString message{ packet.mid( 31 ) };
        QChar type{ packet.at( 31 ) };

        if ( type == '\'' )
        {
            message = message.mid( 1 );
            this->insertChat( srcSerNum % " gossips: " % message,
                              "goldenrod", true );
        }
        else if ( type == '!' )
        {
            message = message.mid( 1 );
            this->insertChat( srcSerNum % " shouts: " % message,
                              "sienna", true );
        }
        else if ( type == '/' )
        {
            message = message.mid( 2 );
            this->insertChat( srcSerNum % message,
                              "seagreen", true );
        }
        else
        {
            this->insertChat( srcSerNum % ": ",
                              "limegreen", true );
            this->insertChat( message,
                              "yellow", false );
        }
    }
}

void ChatView::insertChat(QString msg, QString color, bool newLine)
{
    QTextEdit* obj{ ui->chatView };
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

    QTextCharFormat format;
                    format.setForeground( QBrush( QColor( color ) ) );
    cursor.setCharFormat( format );
    if ( newLine )
        cursor.insertText( "\r\n" );

    cursor.insertText( msg );

    if ( selStart && selEnd )
    {
        cursor.setPosition( selStart );
        cursor.setPosition( selEnd, QTextCursor::KeepAnchor );
        obj->setTextCursor( cursor );
    }

    //Detect when the user is scrolling upwards.
    if ( obj->verticalScrollBar()->sliderPosition() == curScrlPosMax )
    {
        if ( selStart == 0 && selEnd == 0 )
        {
            obj->verticalScrollBar()->setSliderPosition(
                        obj->verticalScrollBar()->maximum() );
        }
    }
}

void ChatView::on_chatInput_returnPressed()
{
    QString message{ ui->chatInput->text() };
    if ( message.startsWith( "/" ) )
    {
        if ( message.compare( "/clear", Qt::CaseInsensitive ) == 0 )
        {
            ui->chatView->clear();
            ui->chatInput->clear();
            return;
        }
    }

    this->insertChat( "Owner: ",
                      "limegreen", true );
    this->insertChat( message,
                      "cyan", false );

    message.prepend( "Owner: " );
    emit this->sendChat( message );
    ui->chatInput->clear();
}
