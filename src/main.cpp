
#include "remix.hpp"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
                 a.setQuitOnLastWindowClosed( false );

#ifndef Q_OS_WIN
    qApp->setFont( QFont( "Lucida Grande", 8 ) );
#else
    qApp->setFont( QFont( "Segoe UI", 8 ) );
#endif
    qApp->font().setFixedPitch( true );

#if defined( USE_DARK_FUSION_UI )
    qApp->setStyle( QStyleFactory::create( "Fusion" ) );
    QPalette darkPalette;
             darkPalette.setColor( QPalette::Window, QColor( 53,53,53 ) );
             darkPalette.setColor( QPalette::WindowText, Qt::white );
             darkPalette.setColor( QPalette::Base, QColor( 25,25,25 ) );
             darkPalette.setColor( QPalette::AlternateBase, QColor( 53,53,53 ) );
             darkPalette.setColor( QPalette::ToolTipBase, Qt::white );
             darkPalette.setColor( QPalette::ToolTipText, Qt::white );
             darkPalette.setColor( QPalette::Text, Qt::white );
             darkPalette.setColor( QPalette::Button, QColor( 53,53,53 ) );
             darkPalette.setColor( QPalette::ButtonText, Qt::white );
             darkPalette.setColor( QPalette::BrightText, Qt::red );
             darkPalette.setColor( QPalette::Link, QColor( 42, 130, 218 ) );
             darkPalette.setColor( QPalette::Highlight, QColor( 42, 130, 218 ) );
             darkPalette.setColor( QPalette::HighlightedText, Qt::black );
    qApp->setPalette( darkPalette );
#endif

    ReMix w;
    w.show();

    return a.exec();
}
