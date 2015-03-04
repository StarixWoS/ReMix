
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

    static const QString keys[ SETTINGS_KEY_COUNT ];
    static const QString subKeys[ SETTINGS_SUBKEY_COUNT ];

    bool pwdCheckState{ false };

    enum Keys{ Setting = 0, WrongIP = 1, Messages = 2, Positions = 3 };
    enum SubKeys{ Extension = 0, Password = 1, AutoBan = 2, AllowIdle = 3,
                  ReqSerNum = 4, AllowDupe = 5, AllowSSV = 6, BanDupes = 7,
                  ReqPassword = 8, MOTD = 9, BanishMsg = 10, Rules = 11,
                  ReqAdminAuth = 12, LogComments = 13, FwdComments = 14,
                  InformAdminLogin = 15, EchoComments = 16,
                  MinimizeToTray = 17, SaveWindowPositions = 18 };

    enum Toggles{ REQPWD = 0, REQADMINPWD = 1, ALLOWDUPEDIP = 2, BANDUPEDIP = 3,
                  BANHACKERS = 4, REQSERNUM = 5, DISCONNECTIDLES = 6,
                  ALLOWSSV = 7, LOGCOMMENTS = 8, FWDCOMMENTS = 9,
                  ECHOCOMMENTS = 10, INFORMADMINLOGIN = 11,
                  MINIMIZETOTRAY = 12, SAVEWINDOWPOSITIONS = 13 };

    public:
        explicit Settings(QWidget *parent = 0);
        ~Settings();

        void setCheckedState(Toggles option, bool val);

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

        static void setServerRules(QVariant& value);
        static QString getServerRules();

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
        void toggleSettings(quint32 row, Qt::CheckState value);

    private slots:
        void on_settingsView_doubleClicked(const QModelIndex &index);
        void on_settingsView_itemClicked(QTableWidgetItem *item);

    private:
        Ui::Settings *ui;
};

#endif // SETTINGS_HPP
