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

        static void setRule(const QString& key, QVariant& value,
                            QString& svrID);
        static QVariant getRule(const QString& key, QString& svrID);

        static QString getRuleSet(QString& svrID);

        static bool getRequireWorld(QString& svrID);
        static void setWorldName(QString& value, QString& svrID);
        static QString getWorldName(QString& svrID);

        static bool getRequireURL(QString& svrID);
        static void setURLAddress(QString& value, QString& svrID);
        static QString getURLAddress(QString& svrID);

        static void setAllPKing(QVariant& value, QString& svrID);
        static bool getAllPKing(QString& svrID);

        static bool getRequireMaxPlayers(QString& svrID);
        static void setMaxPlayers(quint32 value, QString& svrID);
        static quint32 getMaxPlayers(QString& svrID);

        static bool getRequireMaxAFK(QString& svrID);
        static void setMaxAFK(quint32 value, QString& svrID);
        static quint32 getMaxAFK(QString& svrID);

        static bool getRequireMinVersion(QString& svrID);
        static void setMinVersion(QString& value, QString& svrID);
        static QString getMinVersion(QString& svrID);

        static void setReportLadder(QVariant& value, QString& svrID);
        static bool getReportLadder(QString& svrID);

        static void setNoCursing(QVariant& value, QString& svrID);
        static bool getNoCursing(QString& svrID);

        static void setNoCheating(QVariant& value, QString& svrID);
        static bool getNoCheating(QString& svrID);

        static void setNoEavesdropping(QVariant& value, QString& svrID);
        static bool getNoEavesdropping(QString& svrID);

        static void setNoMigrating(QVariant& value, QString& svrID);  //Poor imigration policy..
        static bool getNoMigrating(QString& svrID);

        static void setNoModding(QVariant& value, QString& svrID);
        static bool getNoModding(QString& svrID);

        static void setNoPets(QVariant& value, QString& svrID);
        static bool getNoPets(QString& svrID);

        static void setNoPKing(QVariant& value, QString& svrID);
        static bool getNoPKing(QString& svrID);

        static void setArenaPKing(QVariant& value, QString& svrID);
        static bool getArenaPKing(QString& svrID);
};

#endif // RULES_HPP
