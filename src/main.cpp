
//ReMix includes.
#include "appeventfilter.hpp"
#include "runguard.hpp"
#include "settings.hpp"
#include "remix.hpp"
#include "theme.hpp"

//Qt Includes.
#include <QApplication>

int main(int argc, char* [])
{
    RunGuard guard( "ReMix_Game_Server" );
    if ( !guard.tryToRun() )
        return 0;

    QApplication a(argc, nullptr);
                 a.setApplicationName( "ReMix" );
                 a.setApplicationVersion( REMIX_VERSION );
                 a.setQuitOnLastWindowClosed( false );

    //Remove the "Help" button from the window title bars
    //with an eventfilter at the QApplication level.
    a.installEventFilter( new AppEventFilter() );

    if ( Settings::getDarkMode() )
        Theme::setThemeType( Themes::DARK );
    else
        Theme::setThemeType( Themes::LIGHT );

    ReMix w;
    w.show();

    return a.exec();
}
