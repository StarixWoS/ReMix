
#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include <QCryptographicHash>
#include <QDateTime>
#include <QSettings>
#include <QString>
#include <QDebug>
#include <QFile>

//C++/11 Standard Headers
#include <random>

namespace Helper
{
    enum Keys{ Options = 0, WrongIP = 1, General = 2 };
    const QString keys[ 3 ]{ "options", "wrongIPs", "General" };

    enum SubKeys{ Extension = 0, Password = 1, AutoBan = 2, AllowIdle = 3, ReqSernum = 4, AllowDupe = 5, AllowSSV = 6,
                  BanDupes = 7, ReqPassword = 8, MOTD = 9, BanishMsg = 10, Rules = 11 };
    const QString subKeys[ 12 ]{ "extension", "myPassword", "autoBanish", "discIdle", "requireSernum", "dupeOK", "serverSupportsVariables",
                                 "banishDupes", "requirePassword", "MOTD", "BANISHED", "RULES" };

    int genRandNum(int min = 0, int max = 32767);
    QString intToStr(int val, int base = 10, int fill = 0, QChar filler = '0');
    QString intToStr(QString val, int base = 16, int fill = 0, QChar filler = '0');
    int strToInt(QString& str, int base = 16);

    QString serNumToHexStr(QString sernum, int fillAmt = 8);
    QString serNumToIntStr(QString sernum);

    void setSetting(const QString& key, const QString& subKey, QVariant& value);
    QVariant getSetting(const QString& key, const QString& subKey);

    void setMOTDMessage(QVariant& value);
    QString getMOTDMessage();

    void setBanishMesage(QVariant& value);
    QString getBanishMesage();

    bool setPassword(QVariant& value, bool isHashed);
    QString getPassword();

    void setRequirePassword(QVariant& value);
    bool getRequirePassword();
    bool cmpPassword(QString& value);

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

    void setServerID(QVariant& value);
    int getServerID();

    bool isInvalidIPAddress(const QString& value);

    template<typename T>
    bool readBinaryFile(T& Struct, QString file)
    {
        QFile data( file );
        if ( data.open( QFile::ReadOnly ) )
        {
            if ( !data.read( (char*)&Struct, sizeof( T ) ) )
                return false;    //Return false if it wasn't.

            data.close( );   //If we get here, close the stream.
            return true;
        }
        return false;
    }

    template<typename T>
    bool writeBinaryFile(T& Struct, QString file)
    {
        QFile data( file );
        if ( data.open( QFile::WriteOnly ) )
        {
            if ( !data.write( (char*)&Struct, sizeof( T ) ) )
                return false;    //Return false if it wasn't.

            data.close( );   //If we get here, close the stream.
            return true;
        }
        return false;
    }
}

#endif // PREFERENCES_HPP
