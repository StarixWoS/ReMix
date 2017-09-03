
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

    QHash<QString, MOTDWidget*> msgWidgets;
    QHash<QString, RulesWidget*> ruleWidgets;

    SettingsWidget* settings;
    QTabWidget* tabWidget{ nullptr };

    public:
        explicit Settings(QWidget *parent = 0);
        ~Settings();

        void addTabObjects(MOTDWidget* msgWidget, RulesWidget* ruleWidget,
                           QString& svrID);
        void remTabObjects(QString& svrID);
        void updateTabBar(QString& svrID);

        enum SubKeys{ Extension = 0, Password, AutoBan, AllowIdle, ReqSerNum,
                      AllowDupe, AllowSSV, BanDupes,ReqPassword, MOTD,
                      ReqAdminAuth, LogComments, FwdComments, InformAdminLogin,
                      EchoComments, MinimizeToTray, SaveWindowPositions,
                      IsRunning, WorldDir, PortNumber, IsPublic,
                      GameName, LogFiles = 22 };

        enum Keys{ Setting = 0, WrongIP, Messages, Positions, Rules = 4 };

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
        static bool getBanDeviants();

        static void setReqSernums(QVariant& value);
        static bool getReqSernums();

        static void setDisconnectIdles(QVariant& value);
        static bool getDisconnectIdles();

        static void setAllowSSV(QVariant& value);
        static bool getAllowSSV();

        static void setLogComments(QVariant& value);
        static bool getLogComments();

        static void setLogFiles(QVariant& value);
        static bool getLogFiles();

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

        static void setIsInvalidIPAddress(const QString& value);
        static bool getIsInvalidIPAddress(const QString& value);

        static void setMOTDMessage(QVariant& value, QString& svrID);
        static QString getMOTDMessage(QString& svrID);

        static void setServerID(QVariant& value, QString& svrID);
        static QString getServerID(QString& svrID);

        static void setServerRunning(QVariant value, QString svrID);
        static bool getServerRunning(QString& svrID);

        static void setWorldDir(QString& value);
        static QString getWorldDir();

        static void setPortNumber(QVariant value, QString svrID);
        static QString getPortNumber(QString& svrID);

        static void setIsPublic(QVariant value, QString svrID);
        static bool getIsPublic(QString& svrID);

        static void setGameName(QVariant value, QString svrID);
        static QString getGameName(QString& svrID);

    private:
        Ui::Settings *ui;
};

#endif // SETTINGS_HPP
