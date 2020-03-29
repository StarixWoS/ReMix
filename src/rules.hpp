#ifndef RULES_HPP
#define RULES_HPP

#include "prototypes.hpp"

//Required Qt includes.
#include <QObject>
#include <QMutex>

class Rules
{
    public:
        enum SubKeys{ world = 0, url = 1, allPK = 2, maxP = 3, maxAFK = 4,
                      minV = 5, ladder = 6, noBleep = 7, noCheat = 8,
                      noEavesdrop = 9, noMigrate = 10, noMod = 11, noPets = 12,
                      noPK = 13, arenaPK = 14 };

        static const QStringList subKeys;

        static void setRule(const QString& key, const QVariant& value, const QString& svrID);
        static QVariant getRule(const QString& key, const QString& svrID);

        static void removeRule(const QString& key, const QString& svrID);

        static QString getRuleSet(const QString& svrID);

        static bool getRequireWorld(const QString& svrID);
        static void setWorldName(const QString& value, const QString& svrID);
        static QString getWorldName(const QString& svrID);

        static bool getRequireURL(const QString& svrID);
        static void setURLAddress(const QString& value, const QString& svrID);
        static QString getURLAddress(const QString& svrID);

        static void setAllPKing(const bool& value, const QString& svrID);
        static bool getAllPKing(const QString& svrID);

        static bool getRequireMaxPlayers(const QString& svrID);
        static void setMaxPlayers(const quint32& value, const QString& svrID);
        static quint32 getMaxPlayers(const QString& svrID);

        static bool getRequireMaxAFK(const QString& svrID);
        static void setMaxAFK(const quint32& value, const QString& svrID);
        static quint32 getMaxAFK(const QString& svrID);

        static bool getRequireMinVersion(const QString& svrID);
        static void setMinVersion(const QString& value, const QString& svrID);
        static QString getMinVersion(const QString& svrID);

        static void setReportLadder(const bool& value, const QString& svrID);
        static bool getReportLadder(const QString& svrID);

        static void setNoCursing(const bool& value, const QString& svrID);
        static bool getNoCursing(const QString& svrID);

        static void setNoCheating(const bool& value, const QString& svrID);
        static bool getNoCheating(const QString& svrID);

        static void setNoEavesdropping(const bool& value, const QString& svrID);
        static bool getNoEavesdropping(const QString& svrID);

        //Poor imigration policy..
        static void setNoMigrating(const bool& value, const QString& svrID);
        static bool getNoMigrating(const QString& svrID);

        static void setNoModding(const bool& value, const QString& svrID);
        static bool getNoModding(const QString& svrID);

        static void setNoPets(const bool& value, const QString& svrID);
        static bool getNoPets(const QString& svrID);

        static void setNoPKing(const bool& value, const QString& svrID);
        static bool getNoPKing(const QString& svrID);

        static void setArenaPKing(const bool& value, const QString& svrID);
        static bool getArenaPKing(const QString& svrID);

    private:
        static QMutex mutex;
};

#endif // RULES_HPP
