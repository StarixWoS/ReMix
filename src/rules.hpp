#ifndef RULES_HPP
#define RULES_HPP

#include "prototypes.hpp"

class Rules
{
    public:
        enum SubKeys{ world = 0, url = 1, allPK = 2, maxP = 3, maxAFK = 4,
                      minV = 5, ladder = 6, noBleep = 7, noCheat = 8,
                      noEavesdrop = 9, noMigrate = 10, noMod = 11, noPets = 12,
                      noPK = 13, arenaPK = 14 };

        static const QString subKeys[ RULES_SUBKEY_COUNT ];

        static void setRule(const QString& key, QVariant& value);
        static QVariant getRule(const QString& key);

        static QString getRuleSet();

        static bool getRequireWorld();
        static void setWorldName(QString& value);
        static QString getWorldName();

        static bool getRequireURL();
        static void setURLAddress(QString& value);
        static QString getURLAddress();

        static void setAllPKing(QVariant& value);
        static bool getAllPKing();

        static bool getRequireMaxPlayers();
        static void setMaxPlayers(quint32 value);
        static quint32 getMaxPlayers();

        static bool getRequireMaxAFK();
        static void setMaxAFK(quint32 value);
        static quint32 getMaxAFK();

        static bool getRequireMinVersion();
        static void setMinVersion(QString& value);
        static QString getMinVersion();

        static void setReportLadder(QVariant& value);
        static bool getReportLadder();

        static void setNoCursing(QVariant& value);
        static bool getNoCursing();

        static void setNoCheating(QVariant& value);
        static bool getNoCheating();

        static void setNoEavesdropping(QVariant& value);
        static bool getNoEavesdropping();

        static void setNoMigrating(QVariant& value);  //Poor imigration policy..
        static bool getNoMigrating();

        static void setNoModding(QVariant& value);
        static bool getNoModding();

        static void setNoPets(QVariant& value);
        static bool getNoPets();

        static void setNoPKing(QVariant& value);
        static bool getNoPKing();

        static void setArenaPKing(QVariant& value);
        static bool getArenaPKing();
};

#endif // RULES_HPP
