#ifndef WORLDSHUFFLER_HPP
#define WORLDSHUFFLER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QtConcurrent>
#include <QObject>
#include <QFuture>

class WorldShuffler : public QObject
{
    Q_OBJECT

    static WorldShuffler* instance;
    QStringList activeWorlds;

    //Time since the Server List was last refreshed.
    qint32 lastUpdated{ 0 };

    public:
        explicit WorldShuffler(QObject *parent = nullptr);

        static WorldShuffler* getInstance();
        qint32 getLastUpdated();

    private:
        QStringList getActiveWorlds();
        void worldFinder();

    signals:

    public slots:
};

#endif // WORLDSHUFFLER_HPP
