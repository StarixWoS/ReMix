
//ReMix includes.
#include "appeventfilter.hpp"
#include "runguard.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "remix.hpp"
#include "theme.hpp"

//Qt Includes.
#include <QNetworkAccessManager>
#include <QApplication>
#include <QUrl>

int main(int argc, char* [])
{
    RunGuard guard( "ReMix_Game_Server" );
    if ( !guard.tryToRun() )
        return 0;

    QApplication a(argc, 0);
                 a.setApplicationName( "ReMix" );
                 a.setApplicationVersion( REMIX_VERSION );
                 a.setQuitOnLastWindowClosed( false );
                 a.installEventFilter( new AppEventFilter() );
    if ( Settings::getDarkMode() )
        Theme::setThemeType( Themes::Dark );
    else
        Theme::setThemeType( Themes::Light );

    ReMix w;
    w.show();

    return a.exec();
}
