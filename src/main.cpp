
//ReMix includes.
#include "appeventfilter.hpp"
#include "runguard.hpp"
#include "settings.hpp"
#include "remix.hpp"
#include "theme.hpp"

//Qt Includes.
#include <QNetworkAccessManager>
#include <QApplication>
#include <QThread>
#include <QUrl>

int main(int argc, char* [])
{
    qputenv( "QT_QPA_PLATFORM", "windows:darkmode=0" );

    RunGuard a( argc, nullptr );
    if ( a.getIsRunning() )
        return 0;

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::DarkMode ).toBool() )
        Theme::getInstance()->setThemeType( Themes::Dark );
    else
        Theme::getInstance()->setThemeType( Themes::Light );

    ReMix m;
    m.show();

    return a.exec();
}
