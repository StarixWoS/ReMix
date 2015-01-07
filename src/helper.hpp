
#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include "prototypes.hpp"

namespace Helper
{
    extern const QString keys[ 3 ];
    extern const QString subKeys[ 14 ];

    enum Keys{ Options = 0, WrongIP = 1, General = 2 };
    enum SubKeys{ Extension = 0, Password = 1, AutoBan = 2, AllowIdle = 3, ReqSernum = 4, AllowDupe = 5, AllowSSV = 6,
                  BanDupes = 7, ReqPassword = 8, MOTD = 9, BanishMsg = 10, Rules = 11, ReqAdminAuth = 12, LogComments = 13 };

    QInputDialog* createInputDialog(QWidget* parent, QString& label, QInputDialog::InputMode mode, int width, quint32 height);

    template<typename T>
    QString intToStr(T val, int base = 10, int fill = 0, QChar filler = '0')
    {
        return QString( "%1" ).arg( val, fill, base, filler ).toUpper();
    }

    QString intSToStr(QString val, int base = 16, int fill = 0, QChar filler = '0');
    int strToInt(QString str, int base = 16);

    QString getStrStr(const QString& str, QString indStr, QString mid, QString left);

    QString serNumToHexStr(QString sernum, int fillAmt = 8);
    QString serNumToIntStr(QString sernum);
    qint32 serNumtoInt(QString& sernum);

    void logToFile(QString& file, QString& text, bool timeStamp = false, bool newLine = false);

    bool confirmAction(QWidget* parent, QString& title, QString& prompt);
    qint32 warningMessage(QWidget* parent, QString& title, QString& prompt );

    QString getTextResponse(QWidget* parent, QString& title, QString& prompt, bool* ok, int type = 0);

    void setSetting(const QString& key, const QString& subKey, QVariant& value);
    QVariant getSetting(const QString& key, const QString& subKey);

    void setMOTDMessage(QVariant& value);
    QString getMOTDMessage();

    void setBanishMesage(QVariant& value);
    QString getBanishMesage();

    QString getBanishReason(QWidget* parent = nullptr);
    QString getDisconnectReason(QWidget* parent = nullptr);

    QString hashPassword(QVariant& password);
    bool setPassword(QVariant& value, bool isHashed);
    QString getPassword();

    void setRequirePassword(QVariant& value);
    bool getRequirePassword();
    bool cmpServerPassword(QVariant& value);

    void setServerRules(QVariant& value);
    QString getServerRules();

    void setAllowDupedIP(QVariant& value);
    bool getAllowDupedIP();

    void setBanDupedIP(QVariant& value);
    bool getBanDupedIP();

    void setBanHackers(QVariant& value);
    bool getBanHackers();

    void setReqSernums(QVariant& value);
    bool getReqSernums();

    void setDisconnectIdles(QVariant& value);
    bool getDisconnectIdles();

    void setAllowSSV(QVariant& value);
    bool getAllowSSV();

    void setLogComments(QVariant& value);
    bool getLogComments();

    void setServerID(QVariant& value);
    int getServerID();

    bool isInvalidIPAddress(const QString& value);
}

#endif // PREFERENCES_HPP
