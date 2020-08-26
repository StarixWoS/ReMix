#ifndef CAMPEXEMPTION_HPP
#define CAMPEXEMPTION_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QObject>

class CampExemption : public QObject
{
    Q_OBJECT

    static QSettings* exemptData;
    static CampExemption* instance;

    public:
        explicit CampExemption(QObject *parent = nullptr);
        ~CampExemption() override;

        static void setInstance(CampExemption* value);
        static CampExemption* getInstance();

        static QString makePath(const QString& plrSernum, const QString& exemptSerNum);
        static bool getPlayerExpemption(const QString& plrSernum, const QString& exemptSerNum);
        static bool setPlayerExemption(const QString& plrSernum, const QString& exemptSerNum);

    signals:

};

#endif // CAMPEXEMPTION_HPP
