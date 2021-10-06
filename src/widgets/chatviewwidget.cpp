
//Class includes.
#include "chatviewwidget.hpp"
#include "ui_chatviewwidget.h"

//ReMix includes.
#include "packetforge.hpp"
#include "settings.hpp"
#include "server.hpp"
#include "helper.hpp"
#include "rules.hpp"

//Qt Includes.
#include <QScrollBar>
#include <QtCore>

QString ChatViewWidget::bleepList[31]
{
    "fudgepack",
    "fuck",
    "f*ck",
    "f.uck",
    "f uck",
    "fuc",
    "phuck",
    "cunt",
    "shit",
    "sh1t",
    "asshole",
    "nigger",
    "nigga",
    "clit",
    "bitch",
    "biatch",
    "cock",
    "piss",
    "penis",
    "vagina",
    "pussy",
    "tits",
    "ass",
    "gay",
    "fag",
    "cum",
    "goddam",
    "wtf",
    "damn",
    "hell",
    "ass"
};

ChatViewWidget::ChatViewWidget(QWidget* parent, ServerInfo* svr) :
    QWidget(parent),
    ui(new Ui::ChatViewWidget)
{
    ui->setupUi(this);
    server = svr;

    pktForge = PacketForge::getInstance();
}

ChatViewWidget::~ChatViewWidget()
{
    delete ui;
}

void ChatViewWidget::setTitle(const QString& name)
{
    if ( !name.isEmpty() )
        this->setWindowTitle( "Chat View: [ " % name % " ]" );
}

void ChatViewWidget::setGameID(const Games& gID)
{
    gameID = gID;
    if ( gameID == Games::W97 )
    {
        //Warpath, we can't send Master comments, disable chat interface.
        ui->chatInput->setEnabled( false );
        ui->chatInput->setText( "Unable to interact with Warpath Players!" );
    }
}

Games ChatViewWidget::getGameID() const
{
    return gameID;
}

void ChatViewWidget::parsePacket(const QByteArray& packet, const QString& alias)
{
    //We were unable to load our PacketForge library, return.
    if ( pktForge == nullptr )
        return;

    QString pkt{ packet };
    if ( this->getGameID() != Games::W97 )
    {
        //WoS and Arcadia distort Packets in the same manner.
        pkt = pktForge->decryptPacket( packet );
        if ( !pkt.isEmpty() )
        {
            //WoS and Arcadia both use the opCode 'C' at position '3'
            //in the packet to denote Chat packets.
            if ( pkt.at( 3 ) == 'C' )
            {
                //Remove checksum from Arcadia chat packet.
                if ( this->getGameID() == Games::ToY )
                {
                    //Arcadia Packets have a longer checksum than WoS packets.
                    //Remove the extra characters.
                    pkt = pkt.left( pkt.length() - 4 );
                }
                this->parseChatEffect( pkt );
            }
        }
    }
    else //Handle Warpath97 and Warpath 21st Century Chat.
    {
        pkt = pkt.trimmed();

        //Warpath denotes Chat Packets with opCode 'D' at position '7'.
        if ( pkt.at( 7 ) == 'D' )
        {
            //Remove the packet header.
            pkt = pkt.mid( 8 );

            //Remove the checksum.
            pkt = pkt.left( pkt.length() - 2 );

            this->insertChat( alias % ": ",
                              Colors::Name, true );
            this->insertChat( pkt,
                              Colors::Chat, false );
        }
    }
}

void ChatViewWidget::parseChatEffect(const QString& packet)
{
    QString srcSerNum = packet.left( 12 ).mid( 4 );
            srcSerNum = Helper::serNumToIntStr( srcSerNum );

    QString fltrCode = packet.mid( 13 ).left( 2 );
    qint32 code{ (fltrCode.at( 0 ).toLatin1() - 'A') & 0xFF };
    if ( code == 3 || code == 5 || code == 6 || code == 10 )
    {
        //0 = Normal Chat.
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

        //TODO: Change into something more complex and better.

        //Quick and dirty word replacement.
        if ( server != nullptr )
        {
            //Check if the bleeping rule is set.
            //There's no pint in censoring our chat if we aren't censoring chat
            //for other people.
            if ( Rules::getNoCursing( server->getName() ) )
                this->bleepChat( message );
        }

        QChar type{ packet.at( 31 ) };

        if ( type == '\'' )
        {
            message = message.mid( 1 );
            this->insertChat( srcSerNum % " gossips: " % message,
                              Colors::Gossip, true );
        }
        else if ( type == '!' )
        {
            message = message.mid( 1 );
            this->insertChat( srcSerNum % " shouts: " % message,
                              Colors::Shout, true );
        }
        else if ( type == '/' )
        {
            message = message.mid( 2 );
            this->insertChat( srcSerNum % message,
                              Colors::Emote, true );
        }
        else
        {
            this->insertChat( srcSerNum % ": ",
                              Colors::Name, true );
            this->insertChat( message,
                              Colors::Chat, false );
        }
    }
}

void ChatViewWidget::bleepChat(QString& message)
{
    for ( int i = 0; i <= 30; ++i )
    {
        message = message.replace( bleepList[ i ], "bleep",
                                   Qt::CaseInsensitive );
    }
}

void ChatViewWidget::insertChat(const QString& msg, const Colors& color,
                          const bool& newLine)
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
                    format.setForeground( Theme::getThemeColor( color ) );
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

void ChatViewWidget::on_chatInput_returnPressed()
{
    QString message{ ui->chatInput->text() };
    if ( Helper::strStartsWithStr( message, "/" ) )
    {
        if ( Helper::cmpStrings( message, "/clear" ) )
        {
            ui->chatView->clear();
            ui->chatInput->clear();
            return;
        }
    }

    this->insertChat( "Owner: ",
                      Colors::OwnerName, true );
    this->insertChat( message,
                      Colors::OwnerChat, false );

    if ( gameID == Games::W97 )
    {
        //TODO: Emulate a Warpath Chat packet.
    }
    else
        message.prepend( "Owner: " );

    emit this->sendChat( message );
    ui->chatInput->clear();
}
