
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
QVector<QStringList> Theme::defaultColors =
{
    {   //Light.
        "#daa521", //Gossip Chat.
        "#b8860b", //Shout Chat.
        "#556b2f", //Emote Chat.
        "#00008b", //Player Chat.
        "#008b8b", //Owner Chat.
        "#0000ff", //Player Comment.
        "#daa408", //Golden Soul.
        "#000000", //White Soul.
        "#006401", //Player Name.
        "#006400", //Owner Name.
        "#800080", //Time Stamp
        "#006402", //Admin Valid.
        "#8b0606", //Admin Invalid.
        "#006403", //IP Valid.
        "#8b0505", //IP Invalid.
        "#ff8c00", //IP Vanished.
        "#8b0404", //Party Join.
        "#8b0303", //PK Challenge.
        "#8b0202", //Soul Incarnated.
        "#8b0101", //Soul Left World.
    },
    {   //Dark.
        "#daa520", //Gossip Chat.
        "#a0522d", //Shout Chat.
        "#2e8b57", //Emote Chat.
        "#ffff00", //Player Chat.
        "#00ffff", //Owner Chat.
        "#d6c540", //Player Comment.
        "#dba515", //Golden Soul.
        "#ffffff", //White Soul.
        "#32cd33", //Player Name.
        "#32cd32", //Owner Name.
        "#ff8080", //Time Stamp
        "#32cd35", //Admin Valid.
        "#ff0606", //Admin Invalid.
        "#32cd34", //IP Valid.
        "#ff0505", //IP Invalid.
        "#ffa500", //IP Vanished.
        "#ff0404", //Party Join.
        "#ff0303", //PK Challenge.
        "#ff0202", //Soul Incarnated.
        "#ff0101", //Soul Left World.
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
    for ( int i = 0; i <= static_cast<int>( Colors::ColorCount ); ++i )
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
        defaultPal = themeStyle->standardPalette();
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

QString Theme::getDefaultColor(const Themes& theme, const Colors& color)
{
    return QColor( defaultColors[ static_cast<int>( theme ) ][ static_cast<int>( color ) ] ).name();
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
