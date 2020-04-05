
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
        static QString makeSettingPath(const SettingKeys& key, const SettingSubKeys& subKey);
        static QString makeSettingPath(const SettingKeys& key, const SettingSubKeys& subKey, const QVariant& childSubKey);

        static void removeSetting(const QString& path);
        static bool canRemoveSetting(const QVariant& value);

        static void setSetting(const QVariant& value, const SettingKeys& key, const SettingSubKeys& subKey);
        static void setSetting(const QVariant& value, const SettingKeys& key, const SettingSubKeys& subKey, const QVariant& childSubKey);

        static QVariant getSetting(const SettingKeys& key, const SettingSubKeys& subKey);
        static QVariant getSetting(const SettingKeys& key, const SettingSubKeys& subKey, const QVariant& childSubKey);

        static void setSetting(const QString& key, const QString& subKey, const QVariant& value);
        static QVariant getSetting(const QString& key, const QString& subKey);

        static void setServerSetting(const QString& key, const QString& subKey, const QVariant& value, const QString& svrID);
        static QVariant getServerSetting(const QString& key, const QString& subKey, const QString& svrID);

        static void setReqAdminAuth(const bool& value);
        static bool getReqAdminAuth();

        static void setPassword(const QString& value);
        static QString getPassword();

        static void setAllowDupedIP(const bool& value);
        static bool getAllowDupedIP();

        static void setBanDupedIP(const bool& value);
        static bool getBanDupedIP();

        static void setBanHackers(const bool& value);
        static bool getBanDeviants();

        static void setReqSernums(const bool& value);
        static bool getReqSernums();

        static void setDisconnectIdles(const bool& value);
        static bool getDisconnectIdles();

        static void setAllowSSV(const bool& value);
        static bool getAllowSSV();

        static void setLogComments(const bool& value);
        static bool getLogComments();

        static void setLogFiles(const bool& value);
        static bool getLogFiles();

        static void setDarkMode(const bool& value);
        static bool getDarkMode();

        static void setFwdComments(const bool& value);
        static bool getFwdComments();

        static void setInformAdminLogin(const bool& value);
        static bool getInformAdminLogin();

        static void setEchoComments(const bool& value);
        static bool getEchoComments();

        static void setMinimizeToTray(const bool& value);
        static bool getMinimizeToTray();

        static void setSaveWindowPositions(const bool& value);
        static bool getSaveWindowPositions();

        static void setCheckForUpdates(const bool& value);
        static bool getCheckForUpdates();

        static void setDCBlueCodedSerNums(const bool& value);
        static bool getDCBlueCodedSerNums();

        static void setWindowPositions(const QByteArray& geometry, const char* dialog);
        static QByteArray getWindowPositions(const char* dialog);

        static void setIsInvalidIPAddress(const QString& value);
        static bool getIsInvalidIPAddress(const QString& value);

        static void setMOTDMessage(const QString& value, const QString& svrID);
        static QString getMOTDMessage(const QString& svrID);

        static void setServerID(const qint32& value, const QString& svrID);
        static QString getServerID(const QString& svrID);

        static void setServerRunning(const bool& value, const QString& svrID);
        static bool getServerRunning(const QString& svrID);

        static void setWorldDir(const QString& value);
        static QString getWorldDir();

        static void setPortNumber(const quint16& value, const QString& svrID);
        static QString getPortNumber(const QString& svrID);

        static void setIsPublic(const bool& value, const QString& svrID);
        static bool getIsPublic(const QString& svrID);

        static void setUseUPNP(const bool& value, const QString& svrID);
        static bool getUseUPNP(const QString& svrID);

        static void setGameName(const QString& value, const QString& svrID);
        static QString getGameName(const QString& svrID);

        static void setLoggerAutoScroll(const bool& value);
        static bool getLoggerAutoScroll();

    private:
        Ui::Settings* ui;
        static QMutex mutex;
};

#endif // SETTINGS_HPP
