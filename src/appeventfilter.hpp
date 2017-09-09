#ifndef APPEVENTFILTER_HPP
#define APPEVENTFILTER_HPP

#include "prototypes.hpp"

//Required QT Includes.
#include <QObject>

class AppEventFilter : public QObject
{
    Q_OBJECT

    public:
        explicit AppEventFilter(QObject* parent = nullptr);
        ~AppEventFilter(){}

    signals:

    public slots:
        bool eventFilter(QObject *obj, QEvent *event);
};

#endif // APPEVENTFILTER_HPP
