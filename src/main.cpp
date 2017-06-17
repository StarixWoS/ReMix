
#include "includes.hpp"
#include "remix.hpp"

#include <QApplication>

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
    qApp->setStyle( QStyleFactory::create( "Fusion" ) );

    ReMix w;
    w.setMouseTracking( true );
    w.show();

    return a.exec();
}
