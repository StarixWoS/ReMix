#ifndef WRITETHREAD_HPP
#define WRITETHREAD_HPP

#include "prototypes.hpp"

//Required QT Includes.
#include <QUdpSocket>
#include <QThread>
#include <QString>
#include <QFile>

class WriteThread : public QThread
{
    Q_OBJECT

    QStringList logType;

    QString logDate{ "" };
    QFile punishmentLog;
    QFile commentLog;
    QFile usageLog;
    QFile questLog;
    QFile adminLog;
    QFile pktForge;
    QFile upnpLog;
    QFile miscLog;
    QFile chatLog;
    QFile pingLog;

    public:
        WriteThread(const QStringList& types, QObject *parent = nullptr);
        ~WriteThread() override;

        void run() override;

        static WriteThread* getNewWriteThread(const QStringList& types, QObject* parent = nullptr);

        void logToFile(const LogTypes& type, const QString& text, const QString& timeStamp, const bool& newLine);
        bool isLogOpen(const LogTypes& type);
        QFile& getLogFile(const LogTypes& type);
        void openLogFile(const LogTypes& type);
        void closeLogFile(QFile& log);
        void closeAllLogFiles();

    public slots:
        void insertLogSlot(const LogTypes& type, const QString& text, const QString& timeStamp, const bool& newLine);

};

#endif // WRITETHREAD_HPP
