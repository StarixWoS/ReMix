#ifndef WRITETHREAD_HPP
#define WRITETHREAD_HPP

#include "prototypes.hpp"

//Required QT Includes.
#include <QStringList>
#include <QUdpSocket>
#include <QThread>
#include <QString>
#include <QFile>

class WriteThread : public QThread
{
    Q_OBJECT

    static const QMap<LKeys, QString> logType;

    QString logDate{ "" };
    QFile punishmentLog;
    QFile commentLog;
    QFile masterMix;
    QFile usageLog;
    QFile questLog;
    QFile adminLog;
    QFile upnpLog;
    QFile miscLog;
    QFile chatLog;
    QFile pingLog;

    public:
        WriteThread(QObject *parent = nullptr);
        ~WriteThread() override;

        void run() override;

        static WriteThread* getNewWriteThread(QObject* parent = nullptr);

        void logToFile(const LKeys& type, const QString& text, const QString& timeStamp, const bool& newLine);
        bool isLogOpen(const LKeys& type);
        QFile& getLogFile(const LKeys& type);
        bool openLogFile(const LKeys& type);
        void closeLogFile(QFile& log);
        void closeAllLogFiles();

    public slots:
        void insertLogSlot(const LKeys& type, const QString& text, const QString& timeStamp, const bool& newLine);

};

#endif // WRITETHREAD_HPP
