
#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include "prototypes.hpp"

namespace Helper
{
    QInputDialog* createInputDialog(QWidget* parent, QString& label,
                                    QInputDialog::InputMode mode, int width,
                                    quint32 height);

    template<typename T>
    QString intToStr(T val, int base = 10, int fill = 0, QChar filler = '0')
    {
        return QString( "%1" ).arg( val, fill, base, filler ).toUpper();
    }

    QString intSToStr(QString val, int base = 16, int fill = 0,
                      QChar filler = '0');
    quint32 strToInt(QString str, int base = 16);

    QString getStrStr(const QString& str, QString indStr, QString mid,
                      QString left);
    void stripNewlines(QString& string);
    void stripSerNumHeader(QString& sernum);
    QString sanitizeSerNum(const QString& value);
    QString serNumToHexStr(QString sernum, int fillAmt = 8);
    QString serNumToIntStr(QString sernum);
    quint32 serNumtoInt(QString& sernum);

    void logToFile(QString& file, QString& text, bool timeStamp = false,
                   bool newLine = false);

    bool confirmAction(QWidget* parent, QString& title, QString& prompt);
    qint32 warningMessage(QWidget* parent, QString& title, QString& prompt );

    QString getTextResponse(QWidget* parent, QString& title, QString& prompt,
                            bool* ok, int type = 0);

    QString getBanishReason(QWidget* parent = nullptr);
    QString getDisconnectReason(QWidget* parent = nullptr);

    QString hashPassword(QString& password);
    QString genPwdSalt(RandDev* randGen, qint32 length = SALT_LENGTH);
    bool validateSalt(QString& salt);

    bool naturalSort(QString left, QString right, bool &result);
}

#endif // PREFERENCES_HPP
