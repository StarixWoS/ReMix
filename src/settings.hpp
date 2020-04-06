
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

    static SettingsWidget* settings;
    static QTabWidget* tabWidget;
    static Settings* instance;

    public:
        explicit Settings(QWidget* parent = nullptr);
        ~Settings() override;

        static Settings* getInstance();
        static void setInstance(Settings* value);

        static void updateTabBar(ServerInfo* server);
        static void copyServerSettings(ServerInfo* server, const QString& newName);

        static QSettings* prefs;

        static const QStringList keys;
        static const QStringList subKeys;

    public: //Static-Free functions. Ported from Helper and Admin.
        static void setSettingFromPath(const QString& path, const QVariant& value);
        static QVariant getSettingFromPath(const QString& path);

        static QString makeSettingPath(const SettingKeys& key, const SettingSubKeys& subKey, const QVariant& childSubKey);
        static QString makeSettingPath(const SettingKeys& key, const SettingSubKeys& subKey);
        static QString makeSettingPath(const SettingKeys& key, const QVariant& subKey);

        static void removeSetting(const QString& path);
        static bool canRemoveSetting(const QVariant& value);

        static void setSetting(const QVariant& value, const SettingKeys& key, const SettingSubKeys& subKey, const QVariant& childSubKey);
        static void setSetting(const QVariant& value, const SettingKeys& key, const SettingSubKeys& subKey);
        static void setSetting(const QVariant& value, const SettingKeys& key, const QVariant& subKey);

        static QVariant getSetting(const SettingKeys& key, const SettingSubKeys& subKey, const QVariant& childSubKey);
        static QVariant getSetting(const SettingKeys& key, const SettingSubKeys& subKey);
        static QVariant getSetting(const SettingKeys& key, const QString& subKey);

        static void setServerSetting(const QString& key, const QString& subKey, const QVariant& value, const QString& svrID);
        static QVariant getServerSetting(const QString& key, const QString& subKey, const QString& svrID);

        static QString getServerID(const QString& svrID);

    private:
        Ui::Settings* ui;
        static QMutex mutex;
};

#endif // SETTINGS_HPP
