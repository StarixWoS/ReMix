
//Class includes.
#include "theme.hpp"

//Required Qt Includes.
#include <QStyleFactory>
#include <QApplication>
#include <QPixmapCache>
#include <QPalette>
#include <QObject>
#include <QBrush>
#include <QFile>
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
        "#800080",        //Time Stamp
        "#0000ff",        //Comments
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
        "#ff8080",   //Time Stamp
        "#d6c540",   //Comments
    }
};

QPalette Theme::currentPal;
QStyle* Theme::themeStyle{ QStyleFactory::create( "Fusion" ) };
Themes Theme::themeType{ Themes::Light };
Theme* Theme::instance{ nullptr };

Theme::Theme(QWidget*)
{
}

Theme::~Theme()
{
    if ( themeStyle != nullptr )
    {
        themeStyle->deleteLater();
        themeStyle = nullptr;
    }
}

void Theme::applyTheme(const Themes& type)
{
    if ( themeStyle == nullptr )
    {
        themeStyle = QStyleFactory::create( "Fusion" );
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
        customPalette.setColor( QPalette::Dark, QColor( 35, 35, 35 ) );
        customPalette.setColor( QPalette::Base, QColor( 42, 42, 42 ) );
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
        currentPal = themeStyle->standardPalette();

    qApp->setPalette( currentPal );

    QPixmapCache::clear();
}

Themes Theme::getThemeType()
{
    return themeType;
}

void Theme::setThemeType(const Themes& value)
{
    themeType = value;
    applyTheme( themeType );

    emit this->themeChangedSignal( themeType );
}

QBrush Theme::getColor(const Colors& color)
{
    //Return the default palette text color.
    if ( color == Colors::Default )
        return qApp->palette().windowText();

    return QBrush( QColor( themeColors[ static_cast<int>( getThemeType() ) ][ static_cast<int>( color ) ] ) );
}

QColor Theme::getColor(const Themes& theme, const Colors& color)
{
    return QColor( themeColors[ static_cast<int>( theme ) ][ static_cast<int>( color ) ] );
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
