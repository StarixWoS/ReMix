#ifndef GUILDTORAGE_HPP
#define GUILDTORAGE_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QObject>
#include <QMutex>

class GuildStorage : public QObject
{
    Q_OBJECT

    static QSettings* guildData;
    static GuildStorage* instance;
    static QMutex mutex;

    public:
        GuildStorage();
        ~GuildStorage();
        void setInstance(GuildStorage* value);
        static GuildStorage* getInstance();
};

#endif // GUILDTORAGE_HPP
