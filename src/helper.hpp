
#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QInputDialog>
//#include <QtCore>

class Helper
{
    public:
        static QInputDialog* createInputDialog(QWidget* parent, QString& label,
                                               QInputDialog::InputMode mode,
                                               int width, int height);

        template<typename T>
        static QString intToStr(T val, int base = 10, int fill = 0,
                                QChar filler = '0')
        {
            return QString( "%1" ).arg( val, fill, base, filler ).toUpper();
        }

        static QString intSToStr(QString val, int base = 16, int fill = 0,
                                 QChar filler = '0');
        static qint32 strToInt(QString str, int base = 16);

        static QString getStrStr(const QString& str, QString indStr,
                                 QString mid, QString left);

        static void stripNewlines(QString& string);
        static void stripSerNumHeader(QString& sernum);
        static QString sanitizeSerNum(const QString& value);
        static QString serNumToHexStr(QString sernum, int fillAmt = 8);
        static QString serNumToIntStr(QString sernum);
        static qint32 serNumtoInt(QString& sernum);

        static void logToFile(const QString& file, const QString& text,
                              const bool& timeStamp = false,
                              const bool& newLine = false);

        static bool confirmAction(QWidget* parent, QString& title,
                                  QString& prompt);

        static qint32 warningMessage(QWidget* parent, QString& title,
                                     QString& prompt );

        static QString getTextResponse(QWidget* parent, QString& title,
                                       QString& prompt, bool* ok, int type = 0);

        static QString getBanishReason(QWidget* parent = nullptr);
        static QString getDisconnectReason(QWidget* parent = nullptr);

        static QString hashPassword(QString& password);
        static QString genPwdSalt(RandDev* randGen,
                                  const qint32& length = SALT_LENGTH);

        static bool validateSalt(QString& salt);

        static bool naturalSort(QString& left, QString& right,
                                bool& result);
        static void delay(const qint32& time);

        static QHostAddress getPrivateIP();
        static void getSynRealData(ServerInfo* svr);

        static bool strStartsWithStr(const QString& strA, const QString& strB);
        static bool strContainsStr(const QString& strA, const QString& strB);
        static bool cmpStrings(const QString& strA, const QString& strB );
        static qint32 getStrIndex(const QString& strA, const QString& strB);
};

#endif // PREFERENCES_HPP
