
//Class includes.
#include "chatview.hpp"
#include "ui_chatview.h"

//ReMix includes.
#include "packethandler.hpp"
#include "packetforge.hpp"
#include "serverinfo.hpp"
#include "cmdhandler.hpp"
#include "settings.hpp"
#include "server.hpp"
#include "logger.hpp"
#include "player.hpp"
#include "helper.hpp"
#include "rules.hpp"

//Qt Includes.
#include <QScrollBar>
#include <QtCore>

QStringList ChatView::bleepList
{
    "4r5e", "5h1t", "5hit", "a55", "anal", "anus", "ar5e", "arrse", "arse",
    "ass", "ass-fucker", "asses", "assfucker", "assfukka", "asshole",
    "assholes", "asswhole", "a_s_s", "b!tch", "b00bs", "b17ch", "b1tch",
    "ballbag", "ballsack", "bastard", "beastial", "beastiality", "bestial",
    "bestiality", "bi+ch", "biatch", "bitch", "bitcher", "bitchers", "bitches",
    "bitchin", "bitching", "blow job", "blowjob", "blowjobs", "boiolas",
    "bollock", "bollok", "boner", "boob", "boobs", "booobs", "boooobs",
    "booooobs", "booooooobs", "breasts", "buceta", "bunny fucker", "butthole",
    "buttmuch", "buttplug", "c0ck", "c0cksucker", "carpet muncher", "cawk",
    "chink", "cipa", "cl1t", "clit", "clitoris", "clits", "cnut", "cock",
    "cock-sucker", "cockface", "cockhead", "cockmunch", "cockmuncher", "cocks",
    "cocksuck", "cocksucked", "cocksucker", "cocksucking", "cocksucks",
    "cocksuka", "cocksukka", "cokmuncher", "coksucka", "coon", "cox", "crap",
    "cum", "cummer", "cumming", "cums", "cumshot", "cunilingus", "cunillingus",
    "cunnilingus", "cunt", "cuntlick", "cuntlicker", "cuntlicking", "cunts",
    "cyalis", "cyberfuc", "cyberfuck", "cyberfucked", "cyberfucker",
    "cyberfuckers", "cyberfucking", "d1ck", "damn", "dick", "dickhead",
    "dildo", "dildos", "dink", "dinks", "dirsa", "dlck", "dog-fucker", "doggin",
    "dogging", "donkeyribber", "doosh", "duche", "dyke", "ejaculate",
    "ejaculated", "ejaculates", "ejaculating", "ejaculatings", "ejaculation",
    "ejakulate", "f u c k", "f u c k e r", "f4nny", "fag", "fagging", "faggitt",
    "faggot", "faggs", "fagot", "fagots", "fags", "fanny", "fannyflaps",
    "fannyfucker", "fanyy", "fatass", "fcuk", "fcuker", "fcuking", "feck",
    "fecker", "felching", "fellate", "fellatio", "fingerfuck", "fingerfucked",
    "fingerfucker", "fingerfuckers", "fingerfucking", "fingerfucks",
    "fistfuck", "fistfucked", "fistfucker", "fistfuckers", "fistfucking",
    "fistfuckings", "fistfucks", "fook", "fooker", "fuck", "fucka", "fucked",
    "fucker", "fuckers", "fuckhead", "fuckheads", "fuckin", "fucking",
    "fuckings", "fuckme", "fucks", "fuckwhit", "fuckwit", "fudge packer",
    "fudgepacker", "fuk", "fuker", "fukker", "fukkin", "fuks", "fukwhit",
    "fukwit", "fux", "fux0r", "f_u_c_k", "gangbang", "gangbanged",
    "gangbangs", "gaylord", "gaysex", "goatse", "god-dam", "god-damned",
    "goddamn", "goddamned", "hell ", "heshe", "hoar", "hoare",
    "hoer", "homo", "hore", "horniest", "horny", "hotsex", "jack-off",
    "jackoff", "jap", "jerk-off", "jism", "jiz", "jizm", "jizz", "kawk",
    "knob", "knobead", "knobed", "knobend", "knobhead", "knobjocky",
    "knobjokey", "kock", "kondum", "kondums", "kum", "kummer", "kumming",
    "kums", "kunilingus", "l3i+ch", "l3itch", "labia", "lmfao", "lust",
    "lusting", "m0f0", "m0fo", "m45terbate", "ma5terb8", "ma5terbate",
    "masochist", "master-bate", "masterb8", "masterbat*", "masterbat3",
    "masterbate", "masterbation", "masterbations", "masturbate", "mo-fo",
    "mof0", "mofo", "mothafuck", "mothafucka", "mothafuckas", "mothafuckaz",
    "mothafucked", "mothafucker", "mothafuckers", "mothafuckin",
    "mothafucking", "mothafuckings", "mothafucks", "mother fucker",
    "motherfuck", "motherfucked", "motherfucker", "motherfuckers",
    "motherfuckin", "motherfucking", "motherfuckings", "motherfuckka",
    "motherfucks", "muff", "mutha", "muthafecker", "muthafuckker", "muther",
    "mutherfucker", "n1gga", "n1gger", "nazi", "nigg3r", "nigg4h", "nigga",
    "niggah", "niggas", "niggaz", "nigger", "niggers", "nob", "nob jokey",
    "nobhead", "nobjocky", "nobjokey", "numbnuts", "nutsack", "orgasim",
    "orgasims", "orgasm", "orgasms", "p0rn", "pawn", "pecker", "penis",
    "penisfucker", "phonesex", "phuck", "phuk", "phuked", "phuking", "phukked",
    "phukking", "phuks", "phuq", "pigfucker", "pimpis", "piss", "pissed",
    "pisser", "pissers", "pisses", "pissflaps", "pissin", "pissing",
    "pissoff", "poop", "porn", "porno", "pornography", "pornos", "prick",
    "pricks", "pron", "pube", "pusse", "pussi", "pussies", "pussy", "pussys",
    "rectum", "retard", "rimjaw", "rimming", "s hit", "s.o.b.", "sadist",
    "schlong", "screwing", "scroat", "scrote", "scrotum", "semen", "sex",
    "sh!+", "sh!t", "sh1t", "shag", "shagger", "shaggin", "shagging", "shemale",
    "shi+", "shit", "shitdick", "shite", "shited", "shitey", "shitfuck",
    "shitfull", "shithead", "shiting", "shitings", "shits", "shitted",
    "shitter", "shitters", "shitting", "shittings", "shitty", "skank", "slut",
    "sluts", "smegma", "smut", "snatch", "son-of-a-bitch", "spac", "spunk",
    "s_h_i_t", "t1tt1e5", "t1tties", "teets", "teez", "testical", "testicle",
    "tit", "titfuck", "tits", "titt", "tittie5", "tittiefucker", "titties",
    "tittyfuck", "tittywank", "titwank", "tosser", "turd", "tw4t", "twat",
    "twathead", "twatty", "twunt", "twunter", "v14gra", "v1gra", "vagina",
    "viagra", "vulva", "w00se", "wang", "wank", "wanker", "wanky", "whoar",
    "whore", "willies"
};

ChatView::ChatView(QWidget* parent, ServerInfo* svr) :
    QDialog(parent),
    ui(new Ui::ChatView)
{
    ui->setupUi(this);
    server = svr;

    pktForge = PacketForge::getInstance();

    if ( Settings::getSaveWindowPositions() )
    {
        QByteArray geometry{ Settings::getWindowPositions( this->metaObject()->className() ) };
        if ( !geometry.isEmpty() )
            this->restoreGeometry( Settings::getWindowPositions( this->metaObject()->className() ) );
    }
}

ChatView::~ChatView()
{
    if ( Settings::getSaveWindowPositions() )
        Settings::setWindowPositions( this->saveGeometry(), this->metaObject()->className() );

    delete ui;
}

void ChatView::setTitle(const QString& name)
{
    if ( !name.isEmpty() )
        this->setWindowTitle( "Chat View: [ " % name % " ]" );
}

void ChatView::setGameID(const Games& gID)
{
    gameID = gID;
    if ( gameID == Games::W97 )
    {
        //Warpath, we can't send Master comments, disable chat interface.
        ui->chatInput->setEnabled( false );
        ui->chatInput->setText( "Unable to interact with Warpath Players!" );
    }
}

Games ChatView::getGameID() const
{
    return gameID;
}

bool ChatView::parsePacket(const QByteArray& packet, Player* plr)
{
    //We were unable to load our PacketForge library, return.
    if ( pktForge == nullptr )
        return false;

    //The Player object is invalid, return.
    if ( plr == nullptr )
        return false;

    bool retn{ true };
    QString pkt{ packet };
    if ( this->getGameID() != Games::W97 )
    {
        //WoS and Arcadia distort Packets in the same manner.
        pkt = pktForge->decryptPacket( packet );
        if ( !pkt.isEmpty() )
        {
            //WoS and Arcadia both use the opCode 'C' at position '3'
            //in the packet to denote Chat packets.

            //Remove checksum from Arcadia chat packet.
            if ( this->getGameID() == Games::ToY )
            {
                //Arcadia Packets have a longer checksum than WoS packets.
                //Remove the extra characters.
                pkt = pkt.left( pkt.length() - 4 );
            }

            if ( pkt.at( 3 ) == 'C' )
            {
                plr->chatPacketFound();
                retn = this->parseChatEffect( pkt );
            }
            else if ( pkt.at( 3 ) == '3'
                   || ( ( this->getGameID() == Games::ToY ) && ( pkt.at( 3 ) == 'N' ) ) )
            {
                QStringList varList;
                if ( this->getGameID() == Games::ToY )
                    varList = pkt.mid( 39 ).split( "," );
                else
                    varList = pkt.mid( 47 ).split( "," );

                QString plrName{ varList.at( 0 ) };
                if ( !plrName.isEmpty() )
                    plr->setPlrName( plrName );

                //Check that the User is actually incarnating.
                int type{ pkt.at( 14 ).toLatin1() - 0x41 };
                if ( type >= 1 && type != 4 )
                {
                    //Send Camp packets to the newly connecting User.
                    if ( this->getGameID() == Games::WoS )
                    {
                        Player* tmpPlr{ nullptr };
                        for ( int i = 0; i < MAX_PLAYERS; ++i )
                        {
                            tmpPlr = server->getPlayer( i );
                            if ( tmpPlr != nullptr
                                 && plr != tmpPlr )
                            {
                                if ( plr->getSceneHost() != tmpPlr->getSernum_i()
                                  || plr->getSceneHost() <= 0 )
                                {
                                    if ( !tmpPlr->getCampPacket().isEmpty()
                                      && tmpPlr->getTargetType() == Player::ALL )
                                    {
                                        tmpPlr->setTargetSerNum( plr->getSernum_i() );
                                        tmpPlr->setTargetType( Player::PLAYER );
                                        tmpPlr->forceSendCampPacket();
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if ( this->getGameID() == Games::WoS )
            {
                //Save the User's camp packet. --Send to newly connecting Users.
                if ( pkt.at( 3 ) == 'F' )
                {
                    if ( plr->getCampPacket().isEmpty() )
                    {
                        qint32 sceneID{ Helper::strToInt( pkt.left( 17 ).mid( 13 ) ) };
                        if ( sceneID >= 1 ) //If is 0 then it is the well scene and we can ignore the 'camp' packet.
                            plr->setCampPacket( packet );
                    }
                }  //User un-camp. Remove camp packet.
                else if ( pkt.at( 3 ) == 'f' )
                {
                    if ( !plr->getCampPacket().isEmpty() )
                        plr->setCampPacket( "" );
                }
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

            this->insertChat( plr->getPlrName() % ": ", Colors::Name, true );
            this->insertChat( pkt, Colors::Chat, false );

            plr->chatPacketFound();
        }
        else if ( pkt.at( 7 ) == '4' )
        {
            QString plrName{ pkt.mid( 20 ) };
                    plrName = plrName.left( plrName.length() - 2 );
            if ( !plrName.isEmpty() )
                plr->setPlrName( plrName );
        }
    }
    return retn;
}

bool ChatView::parseChatEffect(const QString& packet)
{
    bool retn{ true };
    bool log{ true };
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
        return true;
    }

    if ( packet.at( 3 ) == 'C' )
    {
        QString plrName{ "Unincarnated [ %1 ]" };
        Player* plr{ nullptr };
        for ( int i = 0; i < MAX_PLAYERS; ++i )
        {
            plr = server->getPlayer( i );
            if ( plr != nullptr )
            {
                if ( Helper::cmpStrings( plr->getSernum_s(), srcSerNum ) )
                {
                    plrName = plr->getPlrName().append( " [ %1 ]" );
                    break;
                }
                else
                    plr = nullptr;
            }
        }

        plrName = plrName.arg( srcSerNum );

        QString message{ packet.mid( 31 ) };

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
        switch ( type.toLatin1() )
        {
            case '\'': //Gossip Chat Effect.
                {
                    message = message.mid( 1 );
                    this->insertChat( plrName % " gossips: " % message, Colors::Gossip, true );
                    message = plrName % " gossips: " % message;
                }
            break;
            case '!': //Shout Chat Effect.
                {
                    message = message.mid( 1 );
                    this->insertChat( plrName % " shouts: " % message, Colors::Shout, true );

                    message = plrName % " shouts: " % message;
                }
            break;
            case '/': //Emote Chat Effect.
                {
                    message = message.mid( 2 );
                    this->insertChat( plrName % message, Colors::Emote, true );
                    message = plrName % message;
                }
            break;
            case '`': //Custom command input.
                {
                    auto* cHandle{ this->getCmdHandle() };
                    if ( cHandle != nullptr )
                    {
                        if ( plr != nullptr )
                        {
                            message = message.mid( 1 );
                            cHandle->parseCommandImpl( plr, message );
                        }
                    }
                    log = false;
                    retn = false;
                }
            break;
            default:
                {
                    this->insertChat( plrName % ": ", Colors::Name, true );
                    this->insertChat( message, Colors::Chat, false );

                    message = plrName % ": " % message;
                }
            break;
        }

        if ( !message.isEmpty() && log )
            Logger::getInstance()->insertLog( server->getName(), message, LogTypes::CHAT, true, true );
    }
    return retn;
}

void ChatView::bleepChat(QString& message)
{
    for ( const auto& el : bleepList )
    {
        message = message.replace( el, "bleep", Qt::CaseInsensitive );
    }
}

void ChatView::insertChat(const QString& msg, const Colors& color,
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

CmdHandler* ChatView::getCmdHandle() const
{
    if ( cmdHandle == nullptr )
        return nullptr;

    return cmdHandle;
}

void ChatView::setCmdHandle(CmdHandler* value)
{
    cmdHandle = value;
}

void ChatView::on_chatInput_returnPressed()
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

    if ( !message.isEmpty() )
        Logger::getInstance()->insertLog( server->getName(), message, LogTypes::CHAT, true, true );

    emit this->sendChat( message );
    ui->chatInput->clear();
}
