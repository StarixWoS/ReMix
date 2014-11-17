
#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include <QSettings>
#include <QString>

namespace Preferences
{
    enum Keys{ Options = 0, WrongIP = 1, General = 2 };
    const QString keys[ 3 ]{ "options", "wrongIPs", "General" };

    enum SubKeys{ Extension = 0, Password = 1, AutoBan = 2, NoIdle = 3, ReqSernum = 4, AllowDupe = 5, AllowSSV = 6,
                  BanDupes = 7, ReqPassword = 8, MOTD = 9, BanishMsg = 10, Rules = 11 };
    const QString subKeys[ 12 ]{ "extension", "myPassword", "autoBanish", "discIdle", "requireSernum", "dupeOK", "serverSupportsVariables",
                                 "banishDupes", "requirePassword", "MOTD", "BANISHED", "RULES" };

    void setSetting(const QString& key, const QString& subKey, QVariant& value);
    QVariant getSetting(const QString& key, const QString& subKey);
    QVariant getSetting(const QString& key);

    void setMOTDMessage(QVariant& value);
    QString getMOTDMessage();

    void setBanishMesage(QVariant& value);
    QString getBanishMesage();

    void setPassword(QVariant& value);
    QString getPassword();

    void setRequirePassword(QVariant& value);
    bool getRequirePassword();

    void setServerRules(QVariant& value);
    QString getServerRules();

    bool isInvalidIPAddress(const QString& value);
}

#endif // PREFERENCES_HPP
