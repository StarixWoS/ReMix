#ifndef RULES_HPP
#define RULES_HPP

#include "prototypes.hpp"

//Required Qt includes.
#include <QObject>
#include <QMutex>

class Rules
{
    public:
        static const QStringList ruleKeys;

        static QString makeRulePath(const QString& serverName, const RuleKeys& key);

        static void removeRule(const QString& path);
        static bool canRemoveRule(const QVariant& value);

        static QVariant getRule(const QString& serverName, const RuleKeys& key);
        static void setRule(const QString& serverName, const QVariant& value, const RuleKeys& key);

        static QString getRuleSet(const QString& svrID);

        static bool cmpServerPassword(const QString& serverName, const QString& value);

    private:
        static QMutex mutex;
};

#endif // RULES_HPP
