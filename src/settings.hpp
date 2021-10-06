
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QDialog>
#include <QMutex>

namespace Ui {
    class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

    static QTabWidget* tabWidget;
    static Settings* instance;

    public:
        explicit Settings(QWidget* parent = nullptr);
        ~Settings() override;

        static Settings* getInstance();
        static void setInstance(Settings* value);

        static void updateTabBar(Server* server);
        static void copyServerSettings(Server* server, const QString& newName);

        static QHash<QHostAddress, QByteArray> bioHash;
        static QSettings* prefs;

        static const QStringList pKeys;
        static const QStringList sKeys;

    public: //Static-Free functions. Ported from Helper and Admin.
        static void setSettingFromPath(const QString& path, const QVariant& value);
        static QVariant getSettingFromPath(const QString& path);

        static QString makeSettingPath(const SKeys& key, const SSubKeys& subKey, const QVariant& childSubKey);
        static QString makeSettingPath(const SKeys& key, const SSubKeys& subKey);
        static QString makeSettingPath(const SKeys& key, const QVariant& subKey);

        static void removeSetting(const QString& path);
        static bool canRemoveSetting(const QVariant& value);

        static void setSetting(const QVariant& value, const SKeys& key, const SSubKeys& subKey, const QVariant& childSubKey);
        static void setSetting(const QVariant& value, const SKeys& key, const SSubKeys& subKey);
        static void setSetting(const QVariant& value, const SKeys& key, const QVariant& subKey);

        static QVariant getSetting(const SKeys& key, const SSubKeys& subKey, const QVariant& childSubKey);
        static QVariant getSetting(const SKeys& key, const SSubKeys& subKey);
        static QVariant getSetting(const SKeys& key, const QString& subKey);

        static void setServerSetting(const QString& key, const QString& subKey, const QVariant& value, const QString& svrID);
        static QVariant getServerSetting(const QString& key, const QString& subKey, const QString& svrID);

        static QString getServerID(const QString& svrID);

        static QString getRuleSet(const QString& svrID);
        static bool cmpServerPassword(const QString& serverName, const QString& value);

        static void insertBioHash(const QHostAddress& addr, const QByteArray& value);
        static QByteArray getBioHashValue(const QHostAddress& addr);
        static QHostAddress getBioHashKey(const QByteArray& bio);

    private:
        Ui::Settings* ui;
        static QMutex mutex;
};

#endif // SETTINGS_HPP
