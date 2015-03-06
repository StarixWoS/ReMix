#ifndef RULES_HPP
#define RULES_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QObject>

class Rules : public QObject
{
    Q_OBJECT

    public:
        explicit Rules(QObject *parent = 0);
        ~Rules();

    signals:

    public slots:
};

#endif // RULES_HPP
