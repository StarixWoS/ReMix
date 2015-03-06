
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

    enum SubKeys{ Extension = 0, Password = 1, AutoBan = 2, AllowIdle = 3,
                  ReqSerNum = 4, AllowDupe = 5, AllowSSV = 6, BanDupes = 7,
                  ReqPassword = 8, MOTD = 9, BanishMsg = 10, ReqAdminAuth = 11,
                  LogComments = 12, FwdComments = 13, InformAdminLogin = 14,
                  EchoComments = 15, MinimizeToTray = 16,
                  SaveWindowPositions = 17 };

    public:
        explicit Settings(QWidget *parent = 0);
        ~Settings();

        enum Keys{ Setting = 0, WrongIP = 1, Messages = 2, Positions = 3,
                   Rules = 4 };

        static const QString keys[ SETTINGS_KEY_COUNT ];
        static const QString subKeys[ SETTINGS_SUBKEY_COUNT ];

    public: //Static-Free functions. Ported from Helper and Admin.

        static void setSetting(const QString& key, const QString& subKey,
                               QVariant& value);

        static QVariant getSetting(const QString& key, const QString& subKey);

        static void setReqAdminAuth(QVariant& value);
        static bool getReqAdminAuth();

        static void setMOTDMessage(QVariant& value);
        static QString getMOTDMessage();

        static void setBanishMesage(QVariant& value);
        static QString getBanishMesage();

        static QString getBanishReason(QWidget* parent = nullptr);
        static QString getDisconnectReason(QWidget* parent = nullptr);

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

        static void setServerID(QVariant& value);
        static int getServerID();

        static bool getIsInvalidIPAddress(const QString& value);

    private:
        Ui::Settings *ui;
};

#endif // SETTINGS_HPP
