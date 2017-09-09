
#include "includes.hpp"
#include "remix.hpp"

#include <QApplication>

int main(int argc, char *[])
{
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

    ReMix w;
    w.show();

    return a.exec();
}
