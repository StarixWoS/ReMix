
//Class includes.
#include "thread/writethread.hpp"

//ReMix Includes
#include "settings.hpp"
#include "helper.hpp"
#include "logger.hpp"

//Qt Includes.
#include <QDateTime>
#include <QtCore>
#include <QDebug>

WriteThread::WriteThread(const QMap<LKeys, QString>& types, QObject *parent)
    : QThread(parent), logType( types )
{
}

WriteThread::~WriteThread()
{
    this->closeAllLogFiles();
    this->disconnect();
    this->deleteLater();
}

void WriteThread::run()
{
    punishmentLog.moveToThread( this->thread() );
    commentLog.moveToThread( this->thread() );
    masterMix.moveToThread( this->thread() );
    usageLog.moveToThread( this->thread() );
    questLog.moveToThread( this->thread() );
    adminLog.moveToThread( this->thread() );
    upnpLog.moveToThread( this->thread() );
    miscLog.moveToThread( this->thread() );
    chatLog.moveToThread( this->thread() );
    pingLog.moveToThread( this->thread() );

    this->exec();
}

WriteThread* WriteThread::getNewWriteThread(const QMap<LKeys, QString>& types, QObject* parent)
{
    return new WriteThread( types, parent );
}

void WriteThread::logToFile(const LKeys& type, const QString& text, const QString& timeStamp, const bool& newLine)
{
    QString logTxt{ text };

    if ( Settings::getSetting( SKeys::Logger, SSubKeys::LogFiles ).toBool() )
    {
        QString date{ QDate::currentDate().toString( "[yyyy-MM-dd]" ) };
        bool close{ !Helper::cmpStrings( logDate, date ) };

        if ( close )
        {
            this->closeAllLogFiles();
            logDate = date;
        }

        if ( !this->isLogOpen( type ) )
            this->openLogFile( type );

        if ( !timeStamp.isEmpty() )
            logTxt.prepend( "[ " % timeStamp % " ] " );

        if ( newLine )
            logTxt.prepend( "\r\n" );

        QTextStream stream( &this->getLogFile( type ) );
        stream << logTxt;
    }
}

bool WriteThread::isLogOpen(const LKeys& type)
{
    switch ( type )
    {
        case LKeys::PunishmentLog: return punishmentLog.isOpen();
        case LKeys::MasterMixLog: return masterMix.isOpen();
        case LKeys::CommentLog: return commentLog.isOpen();
        case LKeys::ClientLog: return usageLog.isOpen();
        case LKeys::SSVLog: return questLog.isOpen();
        case LKeys::AdminLog: return adminLog.isOpen();
        case LKeys::UPNPLog: return upnpLog.isOpen();
        case LKeys::MiscLog: return miscLog.isOpen();
        case LKeys::ChatLog: return chatLog.isOpen();
        case LKeys::PingLog: return pingLog.isOpen();
        case LKeys::AllLogs: break;
    }
    return false;
}

QFile& WriteThread::getLogFile(const LKeys& type)
{
    QFile invalid;
    switch ( type )
    {
        case LKeys::PunishmentLog: return punishmentLog;
        case LKeys::MasterMixLog: return masterMix;
        case LKeys::CommentLog: return commentLog;
        case LKeys::ClientLog: return usageLog;
        case LKeys::SSVLog: return questLog;
        case LKeys::AdminLog: return adminLog;
        case LKeys::UPNPLog: return upnpLog;
        case LKeys::MiscLog: return miscLog;
        case LKeys::ChatLog: return chatLog;
        case LKeys::PingLog: return pingLog;
        case LKeys::AllLogs: break;
    }
    return miscLog; //Use Misc log as fallthrough.
}

void WriteThread::openLogFile(const LKeys& type)
{
    QString log{ "logs/%1/%2.txt" };
            log = log.arg( logDate )
                     .arg( logType.value( type ) );

    QFileInfo logInfo( log );
    if ( !logInfo.dir().exists() )
        logInfo.dir().mkpath( "." );

    switch ( type )
    {
        case LKeys::PunishmentLog:
        {
            punishmentLog.setFileName( log );
            punishmentLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LKeys::CommentLog:
        {
            commentLog.setFileName( log );
            commentLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LKeys::MasterMixLog:
        {
            masterMix.setFileName( log );
            masterMix.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LKeys::ClientLog:
        {
            usageLog.setFileName( log );
            usageLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LKeys::SSVLog:
        {
            questLog.setFileName( log );
            questLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LKeys::AdminLog:
        {
            adminLog.setFileName( log );
            adminLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LKeys::UPNPLog:
        {
            upnpLog.setFileName( log );
            upnpLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LKeys::MiscLog:
        {
            miscLog.setFileName( log );
            miscLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LKeys::ChatLog:
        {
            chatLog.setFileName( log );
            chatLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LKeys::PingLog:
        {
            pingLog.setFileName( log );
            pingLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LKeys::AllLogs: break;
    }
}

void WriteThread::closeLogFile(QFile& log)
{
    if ( log.isOpen() )
    {
        log.flush();
        log.close();
    }
}

void WriteThread::closeAllLogFiles()
{
    this->closeLogFile( punishmentLog );
    this->closeLogFile( commentLog );
    this->closeLogFile( masterMix );
    this->closeLogFile( usageLog );
    this->closeLogFile( questLog );
    this->closeLogFile( adminLog );
    this->closeLogFile( upnpLog );
    this->closeLogFile( miscLog );
    this->closeLogFile( chatLog );
    this->closeLogFile( pingLog );
}

//Slots
void WriteThread::insertLogSlot(const LKeys& type, const QString& text, const QString& timeStamp, const bool& newLine)
{
    this->logToFile( type, text, timeStamp, newLine );
}
