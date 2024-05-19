#ifndef JSONSETTINGS_HPP
#define JSONSETTINGS_HPP

#include <QObject>

class JsonSettings : public QObject
{
        Q_OBJECT
    public:
        explicit JsonSettings(QObject *parent = nullptr);

    signals:

};

#endif // JSONSETTINGS_HPP
