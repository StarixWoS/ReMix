#ifndef THEMECHANGE_HPP
#define THEMECHANGE_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QApplication>
#include <QPalette>
#include <QObject>

class ThemeChange
{
    public:
        static void applyTheme(Themes type)
        {
            QPalette customPalette;
            if ( type == Themes::DARK )
            {
                //Activated Color Roles
                customPalette.setColor( QPalette::All, QPalette::WindowText,
                                  QColor( 231, 231, 231 ) );
                customPalette.setColor( QPalette::All, QPalette::Text,
                                  QColor( 231, 231, 231 ) );
                customPalette.setColor( QPalette::All, QPalette::Base,
                                  QColor( 51, 51, 51 ) );
                customPalette.setColor( QPalette::All, QPalette::Window,
                                  QColor( 51, 51, 51 ) );
                customPalette.setColor( QPalette::All, QPalette::Shadow,
                                  QColor( 105, 105, 105 ) );
                customPalette.setColor( QPalette::All, QPalette::Midlight,
                                  QColor( 227, 227, 227 ) );
                customPalette.setColor( QPalette::All, QPalette::Button,
                                  QColor( 35, 35, 35 ) );
                customPalette.setColor( QPalette::All, QPalette::Light,
                                  QColor( 255, 255, 255 ) );
                customPalette.setColor( QPalette::All, QPalette::Dark,
                                  QColor( 35, 35, 35 ) );
                customPalette.setColor( QPalette::All, QPalette::Mid,
                                  QColor( 160, 160, 160 ) );
                customPalette.setColor( QPalette::All, QPalette::BrightText,
                                  QColor( 255, 255, 255 ) );
                customPalette.setColor( QPalette::All, QPalette::ButtonText,
                                  QColor( 231, 231, 231 ) );
                customPalette.setColor( QPalette::All, QPalette::HighlightedText,
                                  QColor( 255, 255, 255 ) );
                customPalette.setColor( QPalette::All, QPalette::Link,
                                  QColor( 0, 122, 144 ) );
                customPalette.setColor( QPalette::All, QPalette::LinkVisited,
                                  QColor( 165, 122, 255 ) );
                customPalette.setColor( QPalette::All, QPalette::AlternateBase,
                                  QColor( 81, 81, 81 ) );
                customPalette.setColor( QPalette::All, QPalette::ToolTipText,
                                  QColor( 231, 231, 231 ) );

                //Disabled Color Roles
                customPalette.setColor( QPalette::Disabled, QPalette::Button,
                                  QColor( 35, 35, 35 ) );
                customPalette.setColor( QPalette::Disabled, QPalette::WindowText,
                                  QColor( 255, 255, 255 ) );
                customPalette.setColor( QPalette::Disabled, QPalette::Text,
                                  QColor( 255, 255, 255 ) );
                customPalette.setColor( QPalette::Disabled, QPalette::Base,
                                  QColor( 68, 68, 68 ) );
                customPalette.setColor( QPalette::Disabled, QPalette::Window,
                                  QColor( 68, 68, 68 ) );
                customPalette.setColor( QPalette::Disabled, QPalette::Shadow,
                                  QColor( 0, 0, 0 ) );
                customPalette.setColor( QPalette::Disabled, QPalette::Midlight,
                                  QColor( 247, 247, 247 ) );
            }
            qApp->setPalette( customPalette );
        }
};

#endif // THEMECHANGE_HPP
