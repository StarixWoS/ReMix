
//ReMix includes.
#include "appeventfilter.hpp"
#include "runguard.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "remix.hpp"
#include "theme.hpp"

//Qt Includes.
#include <QNetworkAccessManager>
#include <QApplication>
#include <QThread>
#include <QUrl>

int main(int argc, char* [])
{
//This RunGuard implementation does not function on Linux. Presumably it also does not work on OSX.
#if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
    RunGuard guard( "ReMix_Game_Server" );
    if ( !guard.tryToRun() )
        return 0;
#endif

    QApplication a(argc, nullptr);
                 a.setApplicationName( "ReMix" );
                 a.setApplicationVersion( REMIX_VERSION );
                 a.setQuitOnLastWindowClosed( false );
                 a.installEventFilter( new AppEventFilter() );

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::DarkMode ).toBool() )
        Theme::getInstance()->setThemeType( Themes::Dark );
    else
        Theme::getInstance()->setThemeType( Themes::Light );

    ReMix m;
    m.show();
    //ReMix::getInstance()->show();

    return a.exec();
}
