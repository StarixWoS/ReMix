
//Class includes.
#include "appeventfilter.hpp"

//Qt Includes.
#include <QWidget>
#include <QEvent>
#include <QIcon>

AppEventFilter::AppEventFilter(QObject* parent) : QObject(parent)
{
}

bool AppEventFilter::eventFilter(QObject* obj, QEvent* event)
{
    if ( event->type() == QEvent::Create )
    {
        if ( obj->isWidgetType() )
        {
            QWidget* w{ dynamic_cast<QWidget*>( obj ) };
            QIcon icon{ w->windowIcon() };

            w->setWindowFlags( w->windowFlags() & ( ~Qt::WindowContextHelpButtonHint ) );
            w->setWindowModality( Qt::ApplicationModal );
            w->setWindowIcon( icon );
        }
    }
    return QObject::eventFilter( obj, event );
}
