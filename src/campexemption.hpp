#ifndef CAMPEXEMPTION_HPP
#define CAMPEXEMPTION_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QObject>
#include <QMutex>

class CampExemption : public QObject
{
    Q_OBJECT

    static QSettings* exemptData;
    static CampExemption* instance;
    static QMutex mutex;

    public:
        explicit CampExemption(QObject *parent = nullptr);
        ~CampExemption() override;

        static void setInstance(CampExemption* value);
        static CampExemption* getInstance();

        static QString makePath(const QString& plrSernum, const QString& key);
        static QString makePath(const QString& plrSernum, const QString& key, const QString& exemptSerNum);

        static void setDataFromPath(const QString& path, const QVariant& value);
        static QVariant getDataFromPath(const QString& path);

        static void removeSettingFromPath(const QString& path);

        static void setIsLocked(const QString& sernum, const bool& state);
        static bool getIsLocked(const QString& sernum);

        static void setAllowCurrent(const QString& sernum, const bool& state);
        static bool getAllowCurrent(const QString& sernum);

        static void setIsWhitelisted(const QString& srcSerNum, const QString& targetSerNum, const bool& state);
        static bool getIsWhitelisted(const QString& srcSerNum, const QString& targetSerNum);

        static QString getWhiteListedUsers(const QString& srcSerNum);

    public slots:
        void hexSerNumSetSlot(QSharedPointer<Player> plr);
};

#endif // CAMPEXEMPTION_HPP
