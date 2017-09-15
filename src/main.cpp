
#include "includes.hpp"
#include "themechange.hpp"
#include "remix.hpp"
#include "runguard.hpp"

#include <QApplication>

int main(int argc, char *[])
{
    RunGuard guard( "ReMix_Game_Server" );
    if ( !guard.tryToRun() )
        return 0;

    QApplication a(argc, 0);
                 a.setQuitOnLastWindowClosed( false );

    //Remove the "Help" button from the window title bars
    //with an eventfilter at the QApplication level.
    a.installEventFilter( new AppEventFilter() );

#ifndef Q_OS_WIN
    qApp->setFont( QFont( "Lucida Grande", 8 ) );
#else
    qApp->setFont( QFont( "Segoe UI", 8 ) );
#endif

    qApp->font().setFixedPitch( true );
    qApp->setStyle( QStyleFactory::create( "Fusion" ) );

    if ( Settings::getDarkMode() )
        ThemeChange::applyTheme( Themes::DARK );

    ReMix w;
    w.show();

    return a.exec();
}
