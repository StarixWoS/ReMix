
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QDialog>

namespace Ui {
    class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

    QHash<QString, MessagesWidget*> msgWidgets;
    QHash<QString, RulesWidget*> ruleWidgets;

    SettingsWidget* settings;
    QTabWidget* tabWidget{ nullptr };

    public:
        explicit Settings(QWidget *parent = 0);
        ~Settings();

        void addTabObjects(MessagesWidget* msgWidget, RulesWidget* ruleWidget,
                           QString& svrID);
        void remTabObjects(QString& svrID);
        void updateTabBar(QString& svrID);

        enum SubKeys{ Extension = 0, Password = 1, AutoBan = 2, AllowIdle = 3,
                      ReqSerNum = 4, AllowDupe = 5, AllowSSV = 6, BanDupes = 7,
                      ReqPassword = 8, MOTD = 9, BanishMsg = 10,
                      ReqAdminAuth = 11, LogComments = 12, FwdComments = 13,
                      InformAdminLogin = 14, EchoComments = 15,
                      MinimizeToTray = 16, SaveWindowPositions = 17,
                      IsRunning = 18 };

        enum Keys{ Setting = 0, WrongIP = 1, Messages = 2, Positions = 3,
                   Rules = 4 };

        static QSettings* prefs;

        static const QString keys[ SETTINGS_KEY_COUNT ];
        static const QString subKeys[ SETTINGS_SUBKEY_COUNT ];

    public: //Static-Free functions. Ported from Helper and Admin.
        static void setSetting(const QString& key, const QString& subKey,
                               QVariant& value);

        static QVariant getSetting(const QString& key, const QString& subKey);

        static void setServerSetting(const QString& key, const QString& subKey,
                                     QVariant& value, QString& svrID);

        static QVariant getServerSetting(const QString& key,
                                         const QString& subKey, QString& svrID);

        static void setReqAdminAuth(QVariant& value);
        static bool getReqAdminAuth();

        static void setPassword(QString& value);
        static QString getPassword();

        static void setRequirePassword(QVariant& value);
        static bool getRequirePassword();
        static bool cmpServerPassword(QString& value);

        static void setAllowDupedIP(QVariant& value);
        static bool getAllowDupedIP();

        static void setBanDupedIP(QVariant& value);
        static bool getBanDupedIP();

        static void setBanHackers(QVariant& value);
        static bool getBanHackers();

        static void setReqSernums(QVariant& value);
        static bool getReqSernums();

        static void setDisconnectIdles(QVariant& value);
        static bool getDisconnectIdles();

        static void setAllowSSV(QVariant& value);
        static bool getAllowSSV();

        static void setLogComments(QVariant& value);
        static bool getLogComments();

        static void setFwdComments(QVariant& value);
        static bool getFwdComments();

        static void setInformAdminLogin(QVariant& value);
        static bool getInformAdminLogin();

        static void setEchoComments(QVariant& value);
        static bool getEchoComments();

        static void setMinimizeToTray(QVariant& value);
        static bool getMinimizeToTray();

        static void setSaveWindowPositions(QVariant& value);
        static bool getSaveWindowPositions();

        static void setWindowPositions(QByteArray geometry, const char* dialog);
        static QByteArray getWindowPositions(const char* dialog);

        static bool getIsInvalidIPAddress(const QString& value);

        static void setMOTDMessage(QVariant& value, QString& svrID);
        static QString getMOTDMessage(QString& svrID);

        static void setBanishMesage(QVariant& value, QString& svrID);
        static QString getBanishMesage(QString& svrID);

        static void setServerID(QVariant& value, QString& svrID);
        static int getServerID(QString& svrID);

        static void setServerRunning(QVariant value, QString svrID);
        static bool getServerRunning(QString& svrID);

    private:
        Ui::Settings *ui;
};

#endif // SETTINGS_HPP
