
//Class includes.
#include "chatview.hpp"
#include "ui_chatviewwidget.h"

//ReMix includes.
#include "campexemption.hpp"
#include "packetforge.hpp"
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

QHash<QSharedPointer<Server>, ChatView*> ChatView::chatViewInstanceMap;
QVector<Colors> ChatView::colors
{
    Colors::GossipTxt,
    Colors::ShoutTxt,
    Colors::EmoteTxt,
    Colors::PlayerTxt,
    Colors::AdminTxt,
    Colors::AdminMessage,
    Colors::OwnerTxt,
    Colors::CommentTxt,
    Colors::GoldenSoul,
    Colors::WhiteSoul,
    Colors::PlayerName,
    Colors::AdminName,
    Colors::OwnerName,
    Colors::TimeStamp,
    Colors::AdminValid,
    Colors::AdminInvalid,
    Colors::IPValid,
    Colors::IPInvalid,
    Colors::IPVanished,
    Colors::PartyJoin,
    Colors::PKChallenge,
    Colors::SoulIncarnated,
    Colors::SoulLeftWorld,
};

ChatView::ChatView(QSharedPointer<Server> svr, QWidget* parent) :
    QWidget(parent),
    server( svr ),
    ui(new Ui::ChatView)
{
    ui->setupUi(this);
    currentTheme = Theme::getThemeType();

    //Connect LogFile Signals to the Logger Class.
    QObject::connect( this, &ChatView::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );

    //Conect Theme Signals to the ChatView Class.
    QObject::connect( Theme::getInstance(), &Theme::themeChangedSignal, this, &ChatView::themeChangedSlot );

    if ( server->getGameId() == Games::W97 )
    {
        //Warpath, we can't send Master comments, disable chat interface.
        ui->chatInput->setEnabled( false );

        static const QString warPathStr{ "Unable to interact with Warpath Players!" };
        ui->chatInput->setText( warPathStr );
    }

    QObject::connect( Theme::getInstance(), &Theme::colorOverrideSignal, this, &ChatView::colorOverrideSlot );

    ui->autoScrollCheckBox->setChecked( Settings::getSetting( SKeys::Setting, SSubKeys::ChatAutoScroll, server->getServerName() ).toBool() );
    ui->timeStampCheckBox->setChecked( Settings::getSetting( SKeys::Setting, SSubKeys::ChatTimeStamp, server->getServerName() ).toBool() );
}

ChatView::~ChatView()
{
    server = nullptr;
    this->disconnect();
    delete ui;
}

QSharedPointer<Server> ChatView::getServer(ChatView* chatView)
{
    return chatViewInstanceMap.key( chatView );
}

ChatView* ChatView::getInstance(QSharedPointer<Server> server)
{
    ChatView* instance{ chatViewInstanceMap.value( server, nullptr ) };
    if ( instance == nullptr )
    {
        instance = new ChatView( server, ReMix::getInstance() );
        if ( instance != nullptr )
            chatViewInstanceMap.insert( server, instance );
    }
    return instance;
}

void ChatView::deleteInstance(QSharedPointer<Server> server)
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
    Colors msgColor{ Colors::PlayerTxt };

    QSharedPointer<Player> plr{ nullptr };
    QString plrName{ "Unincarnated" };
    QString message{ "" };

    if ( server->getGameId() != Games::W97 )
    {

        const QString fltrCode{ packet.mid( 13 ).left( 2 ) };
        const ChatType code{ static_cast<ChatType>( ( fltrCode.at( 0 ).toLatin1() - 'A' ) & 0xFF ) };
        if ( code == ChatType::Unk3
          || code == ChatType::PetCmd
          || code == ChatType::SceneMsg )
        {
            return true;
        }

        message = packet.mid( 31 );
        QChar type{ packet.at( 31 ) };

        if ( packet.at( 3 ) == 'C' )
        {
            for ( int i = 0; i < server->getMaxPlayerCount(); ++i )
            {
                plr = server->getPlayer( i );
                if ( plr != nullptr )
                {
                    if ( Helper::cmpStrings( plr->getSernum_s(), srcSerNum ) )
                    {
                        plrName = plr->getPlrName();
                        if ( plr->getAdminRank() >= GMRanks::GMaster )
                        {
                            msgColor = Colors::AdminTxt;
                            if ( plr->getAdminRank() > GMRanks::Admin )
                                msgColor = Colors::OwnerTxt;
                        }
                        break;
                    }
                    else
                        plr = nullptr;
                }
            }

            //Quick and dirty word replacement.
            if ( server != nullptr )
            {
                //Check if the bleeping rule is set. There's no pont in censoring our chat if we aren't censoring chat for other people.
                if ( Settings::getSetting( SKeys::Rules, SSubKeys::NoBleep, server->getServerName() ).toBool() )
                    this->bleepChat( message );
            }

            if ( type.toLatin1() != '`' )
            {
                this->insertTimeStamp();
                if ( code != ChatType::DiceAndLevelUp
                  && code != ChatType::LearnSpell
                  && code != ChatType::DeathMsg )
                {
                    this->insertColoredName( plr );
                    this->insertColoredSerNum( plr );
                }
            }

            bool log{ true };
            switch ( type.toLatin1() )
            {
                case '\'': //Gossip Chat Effect.
                    {
                        message = message.mid( 1 );
                        this->insertChat( "gossips: " % message, Colors::GossipTxt, false );
                        message = plrName % " gossips: " % message;
                    }
                break;
                case '!': //Shout Chat Effect.
                    {
                        message = message.mid( 1 );
                        this->insertChat( "shouts: " % message, Colors::ShoutTxt, false );
                        message = plrName % " shouts: " % message;
                    }
                break;
                case '/': //Emote Chat Effect.
                    {
                        message = message.mid( 2 ).simplified();
                        this->insertChat( message, Colors::EmoteTxt, false );
                        message = plrName % message;
                    }
                break;
                case '`': //Custom command input.
                    {
                        if ( plr != nullptr )
                        {
                            if ( !CmdHandler::canParseCommand( plr, message ) )
                            {
                                this->insertTimeStamp();
                                this->insertColoredName( plr );
                                this->insertColoredSerNum( plr );
                                this->insertChat( message, msgColor, false );
                            }
                            else
                                CmdHandler::getInstance( server )->parseCommandImpl( plr, message );
                        }

                        log = false;
                        retn = false;
                    }
                break;
                default:
                    {
                        if ( ( !plrName.isEmpty()
                            && plr->getIsIncarnated() )
                          && server->getGameId() == Games::WoS ) //Only parse these Chat types if on WoS, and if the Player *is* incarnated.
                        {
                            const QString msg{ message.mid( message.indexOf( plrName ) + plrName.length() ).simplified() };
                            switch ( code )
                            {
                                case ChatType::DiceAndLevelUp:
                                    {
                                        this->insertChat( "*** ", Colors::DiceAndLevel, false );
                                        this->insertColoredName( plr );
                                        this->insertColoredSerNum( plr );
                                        this->insertChat( msg, Colors::DiceAndLevel, false );
                                    }
                                break;
                                case ChatType::LearnSpell:
                                    {
                                        this->insertColoredName( plr );
                                        this->insertColoredSerNum( plr );
                                        this->insertChat( msg, Colors::SpellTxt, false );
                                    }
                                break;
                                case ChatType::DeathMsg:
                                    {
                                        this->insertChat( "* ", Colors::DeathTxt, false );
                                        this->insertColoredName( plr );
                                        this->insertColoredSerNum( plr );
                                        this->insertChat( msg, Colors::DeathTxt, false );
                                    }
                                break;
                                default:
                                    this->insertChat( message, msgColor, false );
                                break;
                            }
                        }
                        else
                            this->insertChat( message, msgColor, false );

                        message = plrName % ": " % message;
                    }
                break;
            }

            if ( !message.isEmpty() && log )
                emit this->insertLogSignal( server->getServerName(), message, LKeys::ChatLog, true, true );
        }
    }
    else
    {
        const qint32 plrSlot{ Helper::strToInt( packet.left( 6 ).mid( 4 ) ) };
        for ( int i = 0; i < server->getMaxPlayerCount(); ++i )
        {
            plr = server->getPlayer( i );
            if ( plr != nullptr )
            {
                if ( server->getPlayerSlot( plr ) == plrSlot )
                {
                    if ( plr->getAdminRank() >= GMRanks::GMaster )
                    {
                        msgColor = Colors::AdminTxt;
                        if ( plr->getAdminRank() > GMRanks::Admin )
                            msgColor = Colors::OwnerTxt;
                    }
                    break;
                }
                else
                    plr = nullptr;
            }
        }

        if ( plr != nullptr )
        {
            if ( packet.at( 7 ) == 'D' )
            {
                //Remove the packet header.
                message = packet.mid( 8 );
                if ( packet.at( 8 ) != '`' )
                {
                    this->insertTimeStamp();
                    this->insertColoredName( plr );
                    this->insertColoredSerNum( plr );
                    this->insertChat( message, msgColor, false );
                }
                else
                {
                    if ( CmdHandler::canParseCommand( plr, message ) )
                        CmdHandler::getInstance( server )->parseCommandImpl( plr, message );
                }
            }
        }
    }
    return retn;
}

void ChatView::bleepChat(QString& message)
{
    static const QString bleepStr{ "bleep" };
    for ( const QString& el : bleepList )
    {
        message = message.replace( el, bleepStr, Qt::CaseInsensitive );
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
                    format.setForeground( Theme::getColor( color ) );

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
    const quint64 date{ static_cast<quint64>( QDateTime::currentDateTimeUtc().toSecsSinceEpoch() ) };
    return QString( "[ " % Helper::getTimeAsString( date ) % " ] " );
}

void ChatView::insertColoredSerNum(QSharedPointer<Player> plr)
{
    if ( plr != nullptr )
    {
        Colors color{ Colors::WhiteSoul };
        if ( plr->getIsGoldenSerNum() )
            color = Colors::GoldenSoul;

        this->insertChat( " [ " % plr->getSernum_s() % " ] ", color, false );
    }
}

void ChatView::insertColoredName(QSharedPointer<Player> plr)
{
    if ( plr != nullptr )
    {
        Colors color{ Colors::PlayerName };
        if ( plr->getAdminRank() >= GMRanks::GMaster )
            color = Colors::AdminName;

        if ( plr->getAdminRank() > GMRanks::Admin )
            color = Colors::OwnerName;

        this->insertChat( plr->getPlrName(), color, false );
    }
}

void ChatView::insertTimeStamp()
{
    if ( ui->timeStampCheckBox->isChecked() )
        this->insertChat( this->getTimeStr(), Colors::TimeStamp, true );
    else
        this->insertChat( "", Colors::Default, true );

}

void ChatView::insertChatMsgSlot(const QString& msg, const Colors& color, const bool& newLine)
{
    if ( !msg.isEmpty() )
    {
        if ( color == Colors::TimeStamp )
            this->insertTimeStamp();
        else
            this->insertChat( msg, color, newLine );
    }
}

void ChatView::insertAdminMessageSlot(const QString& msg, const bool& toAll, QSharedPointer<Player> admin, QSharedPointer<Player> target)
{
    if ( msg.isEmpty() )
        return;

    if ( admin == nullptr )
        return;

    if ( toAll == false )
    {
        if ( target == nullptr )
            return;
    }

    this->insertTimeStamp();
    this->insertChat( "Admin < ", Colors::AdminMessage, false );
    this->insertColoredName( admin );
    this->insertColoredSerNum( admin );
    this->insertChat( ">", Colors::AdminMessage, false );

    if ( target != nullptr
      && toAll == false )
    {
        this->insertChat( " to User < ", Colors::AdminMessage, false );

        this->insertColoredName( target );
        this->insertColoredSerNum( target );
        this->insertChat( ">: ", Colors::AdminMessage, false );
    }
    else
        this->insertChat( " to [ Everyone ]: ", Colors::AdminMessage, false );

    this->insertChat( msg, Colors::CommentTxt, false );
}

void ChatView::newUserCommentSlot(QSharedPointer<Player> plr, const QString& message)
{
    if ( plr == nullptr )
        return;

    QString name{ "%1 [ %2 ]: " };
            name = name.arg( plr->getPlrName() )
                       .arg( plr->getSernum_s() );

    QString comment{ "%1%2" };
            comment = comment.arg( name )
                             .arg( message );

    this->insertTimeStamp();
    this->insertChat( "USER COMMENT: ", Colors::CommentTxt, false );
    this->insertColoredName( plr );
    this->insertColoredSerNum( plr );
    this->insertChat( message, Colors::CommentTxt, false );

    //Log comments only when enabled.
    if ( Settings::getSetting( SKeys::Logger, SSubKeys::LogComments ).toBool() )
        emit this->insertLogSignal( server->getServerName(), comment, LKeys::CommentLog, true, true );

    emit this->newUserCommentSignal( comment.simplified() );
}

void ChatView::colorOverrideSlot(const QString& oldColor, const QString& newColor)
{
    QString txtHtml{ ui->chatView->toHtml() };
            txtHtml = txtHtml.replace( oldColor, newColor );

    ui->chatView->clear();
    ui->chatView->insertHtml( txtHtml );
    this->scrollToBottom( true );
}

void ChatView::insertMasterMessageSlot(const QString& message, QSharedPointer<Player> target, const bool& toAll)
{
    static const QString ownerStr{ "Owner: " };
    static const QString ownerToStr{ "Owner to User < " };

    QString msg{ message };

    this->insertTimeStamp();
    if ( target != nullptr
      && toAll == false )
    {
        this->insertChat( ownerToStr, Colors::OwnerName, false );
        this->insertColoredName( target );
        this->insertColoredSerNum( target );
        this->insertChat( " >: ", Colors::OwnerName, false );

        msg = ownerToStr % target->getPlrName() % " [ " % target->getSernum_s() % " ] >: " % message;
    }
    else
    {
        this->insertChat( ownerStr, Colors::OwnerName, false );
        msg.prepend( ownerStr );
    }

    this->insertChat( message, Colors::OwnerTxt, false );

    if ( !msg.isEmpty() )
        emit this->insertLogSignal( server->getServerName(), msg, LKeys::ChatLog, true, true );

    QString packet{ ":;oCFFFFFB2EDLFFFFFFB2E00000000" % msg };
    QString forgedPacket = PacketForge::getInstance()->encryptPacket( packet.toLatin1(), 0, server->getGameId() );
    emit this->sendChatSignal( forgedPacket, target, toAll );
}

void ChatView::on_chatInput_returnPressed()
{
    this->insertMasterMessageSlot( ui->chatInput->text(), nullptr, true );
    ui->chatInput->clear();
}

void ChatView::themeChangedSlot(const Themes& theme)
{
    if ( theme != currentTheme )
    {
        QString txtHtml{ ui->chatView->toHtml() };
        if ( !txtHtml.isEmpty() )
        {
            for ( const Colors& color : colors )
            {
                txtHtml = txtHtml.replace( Theme::getColor( currentTheme, color ).name(),
                                           Theme::getColor( theme, color ).name() );
            }

            ui->chatView->clear();
            ui->chatView->insertHtml( txtHtml );
        }
        this->scrollToBottom( true );
        currentTheme = theme;

        auto pal{ Theme::getCurrentPal() };
        ui->autoScrollCheckBox->setPalette( pal );
        ui->chatInput->setPalette( pal );
        ui->chatView->setPalette( pal );
        ui->label->setPalette( pal );
    }
}

void ChatView::on_autoScrollCheckBox_toggled(bool checked)
{
    Settings::setSetting( checked, SKeys::Setting, SSubKeys::ChatAutoScroll, server->getServerName() );
    ui->autoScrollCheckBox->setChecked( checked );
}

void ChatView::on_clearChat_clicked()
{
    static const QString title{ "Clear Chat:" };
    static const QString prompt{ "Do you wish to clear the Chat View Text?" };

    if ( Helper::confirmAction( this, title, prompt ) )
    {
        ui->chatView->clear();
        ui->chatInput->clear();
    }
}

void ChatView::on_timeStampCheckBox_clicked(bool checked)
{
    Settings::setSetting( checked, SKeys::Setting, SSubKeys::ChatTimeStamp, server->getServerName() );
    ui->timeStampCheckBox->setChecked( checked );
}
