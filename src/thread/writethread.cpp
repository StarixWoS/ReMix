
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

WriteThread::WriteThread(const QStringList& types, QObject *parent)
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

WriteThread* WriteThread::getNewWriteThread(const QStringList& types, QObject* parent)
{
    return new WriteThread( types, parent );
}

void WriteThread::logToFile(const LogTypes& type, const QString& text, const QString& timeStamp, const bool& newLine)
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

bool WriteThread::isLogOpen(const LogTypes& type)
{
    switch ( type )
    {
        case LogTypes::PUNISHMENT: return punishmentLog.isOpen();
        case LogTypes::MASTERMIX: return masterMix.isOpen();
        case LogTypes::COMMENT: return commentLog.isOpen();
        case LogTypes::CLIENT: return usageLog.isOpen();
        case LogTypes::QUEST: return questLog.isOpen();
        case LogTypes::ADMIN: return adminLog.isOpen();
        case LogTypes::UPNP: return upnpLog.isOpen();
        case LogTypes::MISC: return miscLog.isOpen();
        case LogTypes::CHAT: return chatLog.isOpen();
        case LogTypes::PING: return pingLog.isOpen();
        case LogTypes::ALL: break;
    }
    return false;
}

QFile& WriteThread::getLogFile(const LogTypes& type)
{
    QFile invalid;
    switch ( type )
    {
        case LogTypes::PUNISHMENT: return punishmentLog;
        case LogTypes::MASTERMIX: return masterMix;
        case LogTypes::COMMENT: return commentLog;
        case LogTypes::CLIENT: return usageLog;
        case LogTypes::QUEST: return questLog;
        case LogTypes::ADMIN: return adminLog;
        case LogTypes::UPNP: return upnpLog;
        case LogTypes::MISC: return miscLog;
        case LogTypes::CHAT: return chatLog;
        case LogTypes::PING: return pingLog;
        case LogTypes::ALL: break;
    }
    return miscLog; //Use Misc log as fallthrough.
}

void WriteThread::openLogFile(const LogTypes& type)
{
    QString log{ "logs/%1/%2.txt" };
            log = log.arg( logDate )
                     .arg( logType.at( static_cast<int>( type ) ) );

    QFileInfo logInfo( log );
    if ( !logInfo.dir().exists() )
        logInfo.dir().mkpath( "." );

    switch ( type )
    {
        case LogTypes::PUNISHMENT:
        {
            punishmentLog.setFileName( log );
            punishmentLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LogTypes::COMMENT:
        {
            commentLog.setFileName( log );
            commentLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LogTypes::MASTERMIX:
        {
            masterMix.setFileName( log );
            masterMix.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LogTypes::CLIENT:
        {
            usageLog.setFileName( log );
            usageLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LogTypes::QUEST:
        {
            questLog.setFileName( log );
            questLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LogTypes::ADMIN:
        {
            adminLog.setFileName( log );
            adminLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LogTypes::UPNP:
        {
            upnpLog.setFileName( log );
            upnpLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LogTypes::MISC:
        {
            miscLog.setFileName( log );
            miscLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LogTypes::CHAT:
        {
            chatLog.setFileName( log );
            chatLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LogTypes::PING:
        {
            pingLog.setFileName( log );
            pingLog.open( QFile::WriteOnly | QFile::Append );
        }
        break;
        case LogTypes::ALL: break;
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
    this->closeLogFile( usageLog );
    this->closeLogFile( questLog );
    this->closeLogFile( adminLog );
    this->closeLogFile( upnpLog );
    this->closeLogFile( miscLog );
    this->closeLogFile( chatLog );
    this->closeLogFile( pingLog );
}

//Slots
void WriteThread::insertLogSlot(const LogTypes& type, const QString& text, const QString& timeStamp, const bool& newLine)
{
    this->logToFile( type, text, timeStamp, newLine );
}
