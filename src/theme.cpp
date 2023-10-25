
//Class includes.
#include "theme.hpp"

//ReMix Includes.
#include "settings.hpp"

//Required Qt Includes.
#include <QStyleFactory>
#include <QApplication>
#include <QPixmapCache>
#include <QPalette>
#include <QObject>
#include <QBrush>
#include <QFile>
#include <QFont>
#include <QMap>

//Declare Class Static Objects.
QVector<QMap<Colors, QString>> Theme::defaultColors =
{
    {   //Light.
        { Colors::GossipTxt,        "#daa521" }, //Gossip Chat.
        { Colors::ShoutTxt,         "#b8850b" }, //Shout Chat.
        { Colors::EmoteTxt,         "#556b2f" }, //Emote Chat.
        { Colors::DeathTxt,         "#b8860b" }, //Death Text.
        { Colors::SpellTxt,         "#b9870b" }, //Learned Spell Text.
        { Colors::DiceAndLevel,     "#ba880b" }, //Level Up and Dice Roll Text.
        { Colors::PlayerTxt,        "#00008b" }, //Player Chat.
        { Colors::AdminTxt,         "#00008c" }, //Admin Chat.
        { Colors::AdminMessage,     "#018b8b" }, //Axmin messages.
        { Colors::OwnerTxt,         "#008b8b" }, //Owner Chat.
        { Colors::CommentTxt,       "#0000ff" }, //Player Comment.
        { Colors::GoldenSoul,       "#daa408" }, //Golden Soul.
        { Colors::WhiteSoul,        "#010101" }, //White Soul.
        { Colors::PlayerName,       "#006401" }, //Player Name.
        { Colors::AdminName,        "#016401" }, //Admin Name.
        { Colors::OwnerName,        "#006400" }, //Owner Name.
        { Colors::TimeStamp,        "#800080" }, //Time Stamp
        { Colors::AdminValid,       "#006402" }, //Admin Valid.
        { Colors::AdminInvalid,     "#8b0606" }, //Admin Invalid.
        { Colors::IPValid,          "#006403" }, //IP Valid.
        { Colors::IPInvalid,        "#8b0505" }, //IP Invalid.
        { Colors::IPQuarantined,    "#8b0506" }, //IP Quarnatined.
        { Colors::IPVanished,       "#ff8c00" }, //IP Vanished.
        { Colors::PartyJoin,        "#8b0404" }, //Party Join.
        { Colors::PKChallenge,      "#8b0303" }, //PK Challenge.
        { Colors::SoulIncarnated,   "#8b0202" }, //Soul Incarnated.
        { Colors::SoulLeftWorld,    "#8b0101" }, //Soul Left World.
    },
    {   //Dark.
        { Colors::GossipTxt,        "#daa520" }, //Gossip Chat.
        { Colors::ShoutTxt,         "#a0522d" }, //Shout Chat.
        { Colors::EmoteTxt,         "#2e8b57" }, //Emote Chat.
        { Colors::DeathTxt,         "#a1522d" }, //Death Text.
        { Colors::SpellTxt,         "#a2522d" }, //Learned Spell Text.
        { Colors::DiceAndLevel,     "#a3532d" }, //Level Up and Dice Roll Text.
        { Colors::PlayerTxt,        "#ffff00" }, //Player Chat.
        { Colors::AdminTxt,         "#ffff01" }, //Admin Chat.
        { Colors::AdminMessage,     "#01ffff" }, //Axmin messages.
        { Colors::OwnerTxt,         "#00ffff" }, //Owner Chat.
        { Colors::CommentTxt,       "#d6c540" }, //Player Comment.
        { Colors::GoldenSoul,       "#dba515" }, //Golden Soul.
        { Colors::WhiteSoul,        "#ffffff" }, //White Soul.
        { Colors::PlayerName,       "#32cd33" }, //Player Name.
        { Colors::AdminName,        "#33cc32" }, //Admin Name.
        { Colors::OwnerName,        "#32cd32" }, //Owner Name.
        { Colors::TimeStamp,        "#ff8080" }, //Time Stamp
        { Colors::AdminValid,       "#32cd35" }, //Admin Valid.
        { Colors::AdminInvalid,     "#ff0606" }, //Admin Invalid.
        { Colors::IPValid,          "#32cd34" }, //IP Valid.
        { Colors::IPInvalid,        "#ff0505" }, //IP Invalid.
        { Colors::IPQuarantined,    "#ff0506" }, //IP Quarnatined.
        { Colors::IPVanished,       "#ffa500" }, //IP Vanished.
        { Colors::PartyJoin,        "#ff0404" }, //Party Join.
        { Colors::PKChallenge,      "#ff0303" }, //PK Challenge.
        { Colors::SoulIncarnated,   "#ff0202" }, //Soul Incarnated.
        { Colors::SoulLeftWorld,    "#ff0101" }, //Soul Left World.
    }
};

QMap<Colors, QString> Theme::darkOverrides;
QMap<Colors, QString> Theme::lightOverrides;

QPalette Theme::currentPal;
QPalette Theme::defaultPal;
QStyle* Theme::themeStyle{ QStyleFactory::create( "Fusion" ) };
Themes Theme::themeType{ Themes::Light };
Theme* Theme::instance{ nullptr };

Theme::Theme(QWidget*)
{
    defaultPal = QApplication::palette();

    this->loadColors();
}

Theme::~Theme()
{
    if ( themeStyle != nullptr )
    {
        themeStyle->deleteLater();
        themeStyle = nullptr;
    }
}

Themes& Theme::getThemeType()
{
    return themeType;
}

void Theme::setThemeType(const Themes& value)
{
    themeType = value;
    applyTheme( themeType );

    emit this->themeChangedSignal( themeType );
}

void Theme::loadColors()
{
    for ( int i = 0; i <= *Colors::ColorCount; ++i )
    {
        QString dark = Settings::getSetting( SKeys::Colors, Themes::Dark, static_cast<Colors>( i ) ).toString();
        QString light = Settings::getSetting( SKeys::Colors, Themes::Light, static_cast<Colors>( i ) ).toString();
        if ( dark.isEmpty() )
            dark = this->getDefaultColor( Themes::Dark, static_cast<Colors>( i ) );

        if ( light.isEmpty() )
            light = this->getDefaultColor( Themes::Light, static_cast<Colors>( i ) );

        darkOverrides.insert( static_cast<Colors>( i ), QColor( dark ).name() );
        lightOverrides.insert( static_cast<Colors>( i ), QColor( light ).name() );
    }
}

void Theme::applyTheme(const Themes& type)
{
    if ( themeStyle == nullptr )
    {
        themeStyle = QStyleFactory::create( "Fusion" );
        defaultPal = QApplication::palette();
    }

    qApp->setStyle( themeStyle );
    QPalette customPalette{ themeStyle->standardPalette() };
    currentPal = customPalette;

#ifndef Q_OS_WIN
    qApp->setFont( QFont( "Lucida Grande", 8 ) );
#else
    qApp->setFont( QFont( "Segoe UI", 8 ) );
#endif

    qApp->font().setFixedPitch( true );
    if ( type == Themes::Dark )
    {
        //Activated Color Roles
        customPalette.setColor( QPalette::AlternateBase, QColor( 66, 66, 66 ) );
        customPalette.setColor( QPalette::ToolTipText, QColor( 53, 53, 53 ) );
        customPalette.setColor( QPalette::Highlight, QColor( 42, 130, 218 ) );
        customPalette.setColor( QPalette::Shadow, QColor( 20, 20, 20 ) );
        customPalette.setColor( QPalette::Button, QColor( 53, 53, 53 ) );
        customPalette.setColor( QPalette::Link, QColor( 42, 130, 218 ) );
        customPalette.setColor( QPalette::Window, QColor( 53, 53, 53 ));
        customPalette.setColor( QPalette::Dark, QColor( 66, 66, 66 ) );
        customPalette.setColor( QPalette::Base, QColor( 60, 60, 60 ) );
        customPalette.setColor( QPalette::HighlightedText, Qt::white);
        customPalette.setColor( QPalette::ToolTipBase, Qt::white );
        customPalette.setColor( QPalette::WindowText, Qt::white );
        customPalette.setColor( QPalette::ButtonText, Qt::white );
        customPalette.setColor( QPalette::BrightText, Qt::red );
        customPalette.setColor( QPalette::Text, Qt::white );

        //Disabled Color Roles
        customPalette.setColor( QPalette::Disabled, QPalette::HighlightedText, QColor( 127, 127, 127 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::WindowText, QColor( 127, 127, 127 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::ButtonText, QColor( 127, 127, 127 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::Highlight, QColor( 80, 80, 80 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::Text, QColor( 127, 127, 127 ) );

        currentPal = customPalette;
    }
    else
        currentPal = defaultPal;//themeStyle->standardPalette();

    qApp->setPalette( currentPal );
}

QString Theme::getDefaultColor(const Themes& theme, const Colors& color)
{
    return QColor( defaultColors[ *theme ].value( color ) ).name();
}

QBrush Theme::getColorBrush(const Colors& color)
{
    //Return the default palette text color.
    if ( color == Colors::Default )
        return qApp->palette().windowText();

    return QBrush( getColor( getThemeType(), color ) );
}

QColor Theme::getColor(const Themes& theme, const Colors& color)
{
    if ( theme == Themes::Dark )
        return QColor( darkOverrides.value( color ) );

    return QColor( lightOverrides.value( color ) );
}

QColor Theme::getColor(const Colors& color)
{
    if ( getThemeType() == Themes::Dark )
        return QColor( darkOverrides.value( color ) );

    return QColor( lightOverrides.value( color ) );;
}

Theme* Theme::getInstance()
{
    if ( instance == nullptr )
        instance = new Theme( nullptr );

    return instance;
}

const QPalette& Theme::getCurrentPal()
{
    return currentPal;
}

const QPalette Theme::getDefaultPal()
{
    return defaultPal;
}

void Theme::colorOverrideSlot(const Colors& colorRole, const QString& color)
{
    QString oldColor{ "" };
    if ( getThemeType() == Themes::Dark )
    {
        oldColor = darkOverrides.value( colorRole );
        darkOverrides.insert( colorRole, color );
    }
    else
    {
        oldColor = lightOverrides.value( colorRole );
        lightOverrides.insert( colorRole, color );
    }

    emit this->colorOverrideSignal( oldColor, color );
    Settings::setSetting( color, SKeys::Colors, getThemeType(), colorRole );
}
