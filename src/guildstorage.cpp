
//Class includes.
#include "guildstorage.hpp"
#include "helper.hpp"
#include "player.hpp"

//Qt Includes.
#include <QSettings>
#include <QObject>
#include <QMutex>
#include <QtCore>

QSettings* GuildStorage::guildData{ nullptr };
GuildStorage* GuildStorage::instance{ nullptr };
QMutex GuildStorage::mutex;

GuildStorage::GuildStorage()
{

}

GuildStorage::~GuildStorage()
{
    guildData->sync();
    guildData->deleteLater();
}

void GuildStorage::setInstance(GuildStorage* value)
{
    instance = value;
}

GuildStorage* GuildStorage::getInstance()
{
    if ( instance == nullptr )
        instance = new GuildStorage();

    return instance;
}
