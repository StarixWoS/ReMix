
//Class includes.
#include "chatview.hpp"
#include "ui_chatviewwidget.h"

//ReMix includes.
#include "campexemption.hpp"
#include "cmdhandler.hpp"
#include "settings.hpp"
#include "server.hpp"
#include "logger.hpp"
#include "player.hpp"
#include "helper.hpp"
#include "remix.hpp"
#include "user.hpp"

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

QHash<Server*, ChatView*> ChatView::chatViewInstanceMap;

ChatView::ChatView(QWidget* parent, Server* svr) :
    QWidget(parent),
    ui(new Ui::ChatView)
{
    ui->setupUi(this);
    server = svr;

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &ChatView::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );

    //Conect Theme Signals to the ChatView Class.
    QObject::connect( Theme::getInstance(), &Theme::themeChangedSignal, this, &ChatView::themeChangedSlot, Qt::UniqueConnection );

    if ( server->getGameId() == Games::W97 )
    {
        //Warpath, we can't send Master comments, disable chat interface.
        ui->chatInput->setEnabled( false );
        ui->chatInput->setText( "Unable to interact with Warpath Players!" );
    }

    ui->autoScrollCheckBox->setChecked( Settings::getSetting( SKeys::Setting, SSubKeys::ChatAutoScroll, server->getServerName() ).toBool() );
}

ChatView::~ChatView()
{
    this->disconnect();
    delete ui;
}

ChatView* ChatView::getInstance(Server* server)
{
    ChatView* instance{ chatViewInstanceMap.value( server ) };
    if ( instance == nullptr )
    {
        instance = new ChatView( ReMix::getInstance(), server );
        if ( instance != nullptr )
            chatViewInstanceMap.insert( server, instance );
    }
    return instance;
}

void ChatView::deleteInstance(Server* server)
{
    ChatView* instance{ chatViewInstanceMap.take( server ) };
    if ( instance != nullptr )
    {
        instance->disconnect();
        instance->setParent( nullptr );
        instance->deleteLater();
    }
}

bool ChatView::parseChatEffect(const QString& packet)
{
    bool retn{ true };
    QString srcSerNum{ Helper::serNumToIntStr( packet.left( 12 ).mid( 4 ), true ) };
    QString fltrCode{ packet.mid( 13 ).left( 2 ) };

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
        for ( int i = 0; i < static_cast<int>( Globals::MAX_PLAYERS ); ++i )
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
            //Check if the bleeping rule is set. There's no pont in censoring our chat if we aren't censoring chat for other people.
            if ( Settings::getSetting( SKeys::Rules, SSubKeys::NoBleep, server->getServerName() ).toBool() )
                this->bleepChat( message );
        }

        QChar type{ packet.at( 31 ) };
        bool log{ true };

        this->insertChat( this->getTimeStr(), Colors::TimeStamp, true );
        switch ( type.toLatin1() )
        {
            case '\'': //Gossip Chat Effect.
                {
                    message = message.mid( 1 );
                    this->insertChat( plrName % " gossips: " % message, Colors::Gossip, false );
                    message = plrName % " gossips: " % message;
                }
            break;
            case '!': //Shout Chat Effect.
                {
                    message = message.mid( 1 );
                    this->insertChat( plrName % " shouts: " % message, Colors::Shout, false );

                    message = plrName % " shouts: " % message;
                }
            break;
            case '/': //Emote Chat Effect.
                {
                    message = message.mid( 2 );
                    this->insertChat( plrName % message, Colors::Emote, false );
                    message = plrName % message;
                }
            break;
            case '`': //Custom command input.
                {
                    if ( plr != nullptr )
                    {
                        message = message.mid( 1 );
                        CmdHandler::getInstance( server )->parseCommandImpl( plr, message );
                    }
                    log = false;
                    retn = false;
                }
            break;
            default:
                {
                    this->insertChat( plrName % ": ", Colors::Name, false );
                    this->insertChat( message, Colors::Chat, false );

                    message = plrName % ": " % message;
                }
            break;
        }

        if ( !message.isEmpty() && log )
            emit this->insertLogSignal( server->getServerName(), message, LogTypes::CHAT, true, true );
    }
    return retn;
}

void ChatView::bleepChat(QString& message)
{
    for ( const QString& el : bleepList )
    {
        message = message.replace( el, "bleep", Qt::CaseInsensitive );
    }
}

void ChatView::insertChat(const QString& msg, const Colors& color, const bool& newLine)
{
    QTextEdit* obj{ ui->chatView };
    int curScrlPosMax{ obj->verticalScrollBar()->maximum() };
    int selStart{ 0 };
    int selEnd{ 0 };

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
            this->scrollToBottom( true );
    }
    else
        this->scrollToBottom( ui->autoScrollCheckBox->isChecked() );
}

void ChatView::scrollToBottom(const bool& forceScroll)
{
    auto* obj{ ui->chatView };

    //Detect when the user is scrolling upwards. And prevent scrolling.
    if ( obj->verticalScrollBar()->sliderPosition() == obj->verticalScrollBar()->maximum()
      || forceScroll )
    {
        obj->verticalScrollBar()->setSliderPosition( obj->verticalScrollBar()->maximum() );
    }
}

QString ChatView::getTimeStr()
{
    quint64 date{ static_cast<quint64>( QDateTime::currentDateTimeUtc().toSecsSinceEpoch() ) };
    return QString( "[ " % Helper::getTimeAsString( date ) % " ] " );
}

void ChatView::insertChatMsgSlot(const QString& msg, const Colors& color, const bool& newLine)
{
    if ( !msg.isEmpty() )
        this->insertChat( msg, color, newLine );
}

void ChatView::newUserCommentSlot(const QString& sernum, const QString& alias, const QString& message)
{
    QString name{ "%1 [ %2 ]: " };
            name = name.arg( alias )
                       .arg( sernum );
    QString comment{ "%1%2" };
            comment = comment.arg( name )
                             .arg( message );

    this->insertChat( this->getTimeStr(), Colors::TimeStamp, true );
    this->insertChat( "USER COMMENT: ", Colors::Comment, false );
    this->insertChat( name, Colors::Name, false );
    this->insertChat( message, Colors::Chat, false );

    //Log comments only when enabled.
    if ( Settings::getSetting( SKeys::Logger, SSubKeys::LogComments ).toBool() )
        emit this->insertLogSignal( server->getServerName(), comment, LogTypes::COMMENT, true, true );

    emit this->newUserCommentSignal( comment.simplified() );
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

    this->insertChat( this->getTimeStr(), Colors::TimeStamp, true );
    this->insertChat( "Owner: ", Colors::OwnerName, false );
    this->insertChat( message, Colors::OwnerChat, false );

    if ( server->getGameId() == Games::W97 )
    {
        //TODO: Emulate a Warpath Chat packet.
    }
    else
        message.prepend( "Owner: " );

    if ( !message.isEmpty() )
        emit this->insertLogSignal( server->getServerName(), message, LogTypes::CHAT, true, true );

    emit this->sendChatSignal( message );
    ui->chatInput->clear();
}

void ChatView::themeChangedSlot(const Themes& theme)
{
    QString txtHtml{ ui->chatView->toHtml() };
    if ( !txtHtml.isEmpty() )
    {
        //enum class Colors: int{ Valid = 0, Invisible, Invalid, OwnerName, Name,
        //OwnerChat, Chat, Gossip, Shout, Emote,
        //GoldenSoul = 10, Default = -1 };

        if ( theme == Themes::Light )
        {
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Dark, Colors::GoldenSoul ).name(),
                                       Theme::getThemeColor( Themes::Light, Colors::GoldenSoul ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Dark, Colors::OwnerName ).name(),
                                       Theme::getThemeColor( Themes::Light, Colors::OwnerName ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Dark, Colors::OwnerChat ).name(),
                                       Theme::getThemeColor( Themes::Light, Colors::OwnerChat ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Dark, Colors::TimeStamp ).name(),
                                       Theme::getThemeColor( Themes::Light, Colors::TimeStamp ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Dark, Colors::Comment ).name(),
                                       Theme::getThemeColor( Themes::Light, Colors::Comment ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Dark, Colors::Gossip ).name(),
                                       Theme::getThemeColor( Themes::Light, Colors::Gossip ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Dark, Colors::Shout ).name(),
                                       Theme::getThemeColor( Themes::Light, Colors::Shout ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Dark, Colors::Emote ).name(),
                                       Theme::getThemeColor( Themes::Light, Colors::Emote ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Dark, Colors::Name ).name(),
                                       Theme::getThemeColor( Themes::Light, Colors::Name ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Dark, Colors::Chat ).name(),
                                       Theme::getThemeColor( Themes::Light, Colors::Chat ).name() );
        }
        else
        {
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Light, Colors::GoldenSoul ).name(),
                                       Theme::getThemeColor( Themes::Dark, Colors::GoldenSoul ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Light, Colors::OwnerName ).name(),
                                       Theme::getThemeColor( Themes::Dark, Colors::OwnerName ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Light, Colors::OwnerChat ).name(),
                                       Theme::getThemeColor( Themes::Dark, Colors::OwnerChat ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Light, Colors::TimeStamp ).name(),
                                       Theme::getThemeColor( Themes::Dark, Colors::TimeStamp ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Light, Colors::Comment ).name(),
                                       Theme::getThemeColor( Themes::Dark, Colors::Comment ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Light, Colors::Gossip ).name(),
                                       Theme::getThemeColor( Themes::Dark, Colors::Gossip ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Light, Colors::Shout ).name(),
                                       Theme::getThemeColor( Themes::Dark, Colors::Shout ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Light, Colors::Emote ).name(),
                                       Theme::getThemeColor( Themes::Dark, Colors::Emote ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Light, Colors::Name ).name(),
                                       Theme::getThemeColor( Themes::Dark, Colors::Name ).name() );
            txtHtml = txtHtml.replace( Theme::getThemeColor( Themes::Light, Colors::Chat ).name(),
                                       Theme::getThemeColor( Themes::Dark, Colors::Chat ).name() );
        }

        ui->chatView->clear();
        ui->chatView->insertHtml( txtHtml );
    }
    this->scrollToBottom( true );
}

void ChatView::on_autoScrollCheckBox_toggled(bool checked)
{
    Settings::setSetting( checked, SKeys::Setting, SSubKeys::ChatAutoScroll, server->getServerName() );
    ui->autoScrollCheckBox->setChecked( checked );
}

