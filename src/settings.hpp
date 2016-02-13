
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

    QTabWidget* tabWidget{ nullptr };
    SettingsWidget* settings{ nullptr };
    MessagesWidget* messages{ nullptr };
    RulesWidget* rules{ nullptr };
    QString serverID{ "" };

    enum SubKeys{ Extension = 0, Password = 1, AutoBan = 2, AllowIdle = 3,
                  ReqSerNum = 4, AllowDupe = 5, AllowSSV = 6, BanDupes = 7,
                  ReqPassword = 8, MOTD = 9, BanishMsg = 10, ReqAdminAuth = 11,
                  LogComments = 12, FwdComments = 13, InformAdminLogin = 14,
                  EchoComments = 15, MinimizeToTray = 16,
                  SaveWindowPositions = 17 };

    public:
        explicit Settings(QWidget *parent = 0, QString svrID = "0");
        ~Settings();

        enum Keys{ Setting = 0, WrongIP = 1, Messages = 2, Positions = 3,
                   Rules = 4 };

        static QSettings* prefs;
        static const QString keys[ SETTINGS_KEY_COUNT ];
        static const QString subKeys[ SETTINGS_SUBKEY_COUNT ];

    public: //Static-Free functions. Ported from Helper and Admin.

        static void setSetting(const QString& key, const QString& subKey,
                               QVariant& value, QString& svrID);

        static QVariant getSetting(const QString& key, const QString& subKey,
                                   QString& svrID);

        static void setReqAdminAuth(QVariant& value, QString& svrID);
        static bool getReqAdminAuth(QString& svrID);

        static void setMOTDMessage(QVariant& value, QString& svrID);
        static QString getMOTDMessage(QString& svrID);

        static void setBanishMesage(QVariant& value, QString& svrID);
        static QString getBanishMesage(QString& svrID);

        static QString getBanishReason(QWidget* parent = nullptr);
        static QString getDisconnectReason(QWidget* parent = nullptr);

        static void setPassword(QString& value, QString& svrID);
        static QString getPassword(QString& svrID);

        static void setRequirePassword(QVariant& value, QString& svrID);
        static bool getRequirePassword(QString& svrID);
        static bool cmpServerPassword(QString& value, QString& svrID);

        static void setAllowDupedIP(QVariant& value, QString& svrID);
        static bool getAllowDupedIP(QString& svrID);

        static void setBanDupedIP(QVariant& value, QString& svrID);
        static bool getBanDupedIP(QString& svrID);

        static void setBanHackers(QVariant& value, QString& svrID);
        static bool getBanHackers(QString& svrID);

        static void setReqSernums(QVariant& value, QString& svrID);
        static bool getReqSernums(QString& svrID);

        static void setDisconnectIdles(QVariant& value, QString& svrID);
        static bool getDisconnectIdles(QString& svrID);

        static void setAllowSSV(QVariant& value, QString& svrID);
        static bool getAllowSSV(QString& svrID);

        static void setLogComments(QVariant& value, QString& svrID);
        static bool getLogComments(QString& svrID);

        static void setFwdComments(QVariant& value, QString& svrID);
        static bool getFwdComments(QString& svrID);

        static void setInformAdminLogin(QVariant& value, QString& svrID);
        static bool getInformAdminLogin(QString& svrID);

        static void setEchoComments(QVariant& value, QString& svrID);
        static bool getEchoComments(QString& svrID);

        static void setMinimizeToTray(QVariant& value, QString& svrID);
        static bool getMinimizeToTray(QString& svrID);

        static void setSaveWindowPositions(QVariant& value, QString& svrID);
        static bool getSaveWindowPositions(QString& svrID);

        static void setWindowPositions(QByteArray geometry, const char* dialog,
                                       QString& svrID);

        static QByteArray getWindowPositions(const char* dialog, QString& svrID);

        static void setServerID(QVariant& value, QString& svrID);
        static int getServerID(QString& svrID);

        static bool getIsInvalidIPAddress(const QString& value);

    private:
        Ui::Settings *ui;
};

#endif // SETTINGS_HPP
