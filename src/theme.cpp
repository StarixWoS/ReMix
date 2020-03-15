
//Class includes.
#include "theme.hpp"

//Required Qt Includes.
#include <QStyleFactory>
#include <QApplication>
#include <QPalette>
#include <QObject>
#include <QBrush>
#include <QFont>

//Declare Class Static Objects.
QVector<QStringList> Theme::themeColors =
{
    {   //Light.
        "darkgreen",      //Valid.
        "darkorange",     //Invisible.
        "darkred",        //Invalid.
        "darkgreen",      //Owner name.
        "darkgreen",      //Name.
        "darkcyan",       //Owner Chat.
        "darkblue",       //Chat.
        "goldenrod",      //Gossip.
        "darkgoldenrod",  //Shout.
        "darkolivegreen", //Emote;
        "goldenrod",      //Golden Soul.
    },
    {   //Dark.
        "limegreen", //Valid.
        "orange",    //Invisible.
        "red",       //Invalid.
        "limegreen", //Owner name.
        "limegreen", //Name.
        "cyan",      //Owner Chat.
        "yellow",    //Chat.
        "goldenrod", //Gossip.
        "sienna",    //Shout.
        "seagreen",  //Emote;
        "goldenrod", //Golden Soul.
    }
};

Themes Theme::themeType{ Themes::Light };

void Theme::applyTheme(const Themes& type)
{
    QPalette customPalette;

#ifndef Q_OS_WIN
    qApp->setFont( QFont( "Lucida Grande", 8 ) );
#else
    qApp->setFont( QFont( "Segoe UI", 8 ) );
#endif

    qApp->font().setFixedPitch( true );
    qApp->setStyle( QStyleFactory::create( "Fusion" ) );

    if ( type == Themes::Dark )
    {
        //Activated Color Roles
        customPalette.setColor( QPalette::All, QPalette::WindowText, QColor( 231, 231, 231 ) );
        customPalette.setColor( QPalette::All, QPalette::Text, QColor( 231, 231, 231 ) );
        customPalette.setColor( QPalette::All, QPalette::Base, QColor( 51, 51, 51 ) );
        customPalette.setColor( QPalette::All, QPalette::Window, QColor( 51, 51, 51 ) );
        customPalette.setColor( QPalette::All, QPalette::Shadow, QColor( 105, 105, 105 ) );
        customPalette.setColor( QPalette::All, QPalette::Midlight, QColor( 227, 227, 227 ) );
        customPalette.setColor( QPalette::All, QPalette::Button, QColor( 35, 35, 35 ) );
        customPalette.setColor( QPalette::All, QPalette::Light, QColor( 255, 255, 255 ) );
        customPalette.setColor( QPalette::All, QPalette::Dark, QColor( 35, 35, 35 ) );
        customPalette.setColor( QPalette::All, QPalette::Mid, QColor( 160, 160, 160 ) );
        customPalette.setColor( QPalette::All, QPalette::BrightText, QColor( 255, 255, 255 ) );
        customPalette.setColor( QPalette::All, QPalette::ButtonText, QColor( 231, 231, 231 ) );
        customPalette.setColor( QPalette::All, QPalette::HighlightedText, QColor( 255, 255, 255 ) );
        customPalette.setColor( QPalette::All, QPalette::Link, QColor( 0, 122, 144 ) );
        customPalette.setColor( QPalette::All, QPalette::LinkVisited, QColor( 165, 122, 255 ) );
        customPalette.setColor( QPalette::All, QPalette::AlternateBase, QColor( 81, 81, 81 ) );
        customPalette.setColor( QPalette::All, QPalette::ToolTipText, QColor( 231, 231, 231 ) );

        //Disabled Color Roles
        customPalette.setColor( QPalette::Disabled, QPalette::Button, QColor( 35, 35, 35 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::WindowText, QColor( 255, 255, 255 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::Text, QColor( 255, 255, 255 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::Base, QColor( 68, 68, 68 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::Window, QColor( 68, 68, 68 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::Shadow, QColor( 0, 0, 0 ) );
        customPalette.setColor( QPalette::Disabled, QPalette::Midlight, QColor( 247, 247, 247 ) );

        qApp->setPalette( customPalette );
    }
    //Light theme, keep standard palette.
}

Themes Theme::getThemeType()
{
    return themeType;
}

void Theme::setThemeType(const Themes& value)
{
    themeType = value;
    applyTheme( themeType );
}

QBrush Theme::getThemeColor(const Colors& color)
{
    //Return the default palette text color.
    if ( color == Colors::Default)
        return qApp->palette().windowText();

    return QBrush( QColor( themeColors[ static_cast<int>( getThemeType() ) ][ static_cast<int>( color ) ] ) );
}
