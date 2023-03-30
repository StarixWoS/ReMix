
//Class includes.
#include "logger.hpp"
#include "ui_logger.h"

//ReMix includes.
#include "thread/writethread.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "theme.hpp"
#include "remix.hpp"

//Qt Includes.
#include <QGraphicsPixmapItem>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QGraphicsItem>
#include <QPixmapCache>
#include <QScrollBar>
#include <QVariant>
#include <QPixmap>
#include <QString>
#include <QObject>
#include <QtCore>

QMap<QStandardItemModel*, LKeys> Logger::logViews;

Logger* Logger::logInstance{ nullptr };

const QString Logger::website{ "https://bitbucket.org/ahitb/remix" };
const QMap<LKeys, QString> Logger::logType =
{
    { LKeys::AllLogs,       "AllLogs"       },
    { LKeys::AdminLog,      "AdminUsageLog" },
    { LKeys::CommentLog,    "CommentLog"    },
    { LKeys::ClientLog,     "ClientLog"     },
    { LKeys::MasterMixLog,  "MasterMixLog"  },
    { LKeys::UPNPLog,       "UPNPLog"       },
    { LKeys::PunishmentLog, "PunishmentLog" },
    { LKeys::MiscLog,       "MiscLog"       },
    { LKeys::ChatLog,       "ChatLog"       }, //Unused, placeholder.
    { LKeys::SSVLog,        "QuestLog"      },
    { LKeys::PingLog,       "PingLog"       },
};

Logger::Logger(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Logger)
{
    ui->setupUi(this);

    logThread = new QThread();
    writeThread = WriteThread::getNewWriteThread( nullptr );
    writeThread->moveToThread( logThread );

    //Connect Objects to Slots.
    QObject::connect( Theme::getInstance(), &Theme::themeChangedSignal, this, &Logger::themeChangedSlot );
    QObject::connect( &autoClearTimer, &QTimer::timeout, this, &Logger::autoClearTimeOutSlot );
    QObject::connect( this, &Logger::insertLogSignal, writeThread, &WriteThread::insertLogSlot );
    QObject::connect( this, &Logger::resizeColumnsSignal, this, &Logger::resizeColumnsSlot );

    this->createLogViews();
    ui->logView->setModel( logViews.key( LKeys::AllLogs, nullptr ) );

    //Load the application Icon into the top left of the dialog.
    iconViewerItem = new QGraphicsPixmapItem( QPixmap::fromImage( QImage( ":/icon/ReMix.png" ) ) );
    iconViewerScene = new QGraphicsScene();
    iconViewerScene->addItem( iconViewerItem );

    ui->iconViewer->setScene( iconViewerScene );
    ui->iconViewer->show();

    //Set the version number into the versionLabel.
    QString versionText{ ui->versionLabel->text() };
            versionText = versionText.arg( QString( REMIX_VERSION ) );
    ui->versionLabel->setText( versionText );

    //Restore the AutoLog setting.
    ui->autoScroll->setChecked( Settings::getSetting( SKeys::Logger, SSubKeys::LoggerAutoScroll ).toBool() );

    //Restore the AutoClear Logs setting.
    ui->autoClear->setChecked( Settings::getSetting( SKeys::Logger, SSubKeys::LoggerAutoClear ).toBool() );
    autoClearTimer.setInterval( *TimeInterval::Day * *TimeMultiply::Milliseconds );

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        this->restoreGeometry( Settings::getSetting( SKeys::Positions, "Logger" ).toByteArray() );

    logThread->start();
}

Logger::~Logger()
{
    //Disable Logging Signals.
    this->disconnect();

    if ( logThread != nullptr )
    {
        logThread->exit();
        logThread->wait();  //Properly await thread exit.

        writeThread->deleteLater();
    }

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        Settings::setSetting( this->saveGeometry(), SKeys::Positions, "Logger" );

    iconViewerScene->removeItem( iconViewerItem );
    iconViewerScene->deleteLater();

    //Delete logModels
    for ( const auto& type : logType.keys() )
    {
        if ( type != LKeys::ChatLog )
        {
            QStandardItemModel* model{ logViews.key( type, nullptr ) };
            if ( model != nullptr )
            {
                logViews.remove( model );
                model->deleteLater();
            }
            model = nullptr;
        }
    }

    this->deleteLater();

    delete iconViewerItem;
    delete ui;
}

Logger* Logger::getInstance()
{
    if ( logInstance == nullptr )
        logInstance = new Logger( ReMix::getInstance() );

    return logInstance;
}

void Logger::setInstance(Logger* logger)
{
    logInstance = logger;
}

void Logger::scrollToBottom(const bool& forceScroll)
{
    if ( ui->autoScroll->isChecked() )
    {
        //Detect when the user is scrolling upwards. And prevent scrolling.
        if ( ( ui->logView->verticalScrollBar()->sliderPosition() == ui->logView->verticalScrollBar()->maximum() )
          || forceScroll )
        {
            ui->logView->scrollToBottom();
        }
    }
}

void Logger::insertLog(const QString& source, const QString& message, const LKeys& type, const bool& logToFile, const bool& newLine)
{
    QStandardItemModel* typeModel{ logViews.key( type, nullptr ) };
    QStandardItemModel* defaultModel{ logViews.key( LKeys::AllLogs, nullptr ) };

    QString time{ Helper::getTimeAsString() };
    QString format{ "%1 - %2 - %3 - %4" }; //Format string. - Easier to modify.

    if ( defaultModel != nullptr
      && typeModel != nullptr
      && type != LKeys::ChatLog ) //Hide the Chat from the Log View.
    {
        format = format.arg( time )
                       .arg( logType.value( type ) )
                       .arg( source )
                       .arg( message.simplified() );

        //Log to the LKeys::LogType model.
        qint32 row{ typeModel->rowCount() };
        typeModel->insertRows( row, 1 );
        const auto typeModelIdx{ typeModel->index( row, 0 ) };

        //Continuously log to the default model.
        row = defaultModel->rowCount();
        defaultModel->insertRows( row, 1 );
        const auto tblModelIdx{ defaultModel->index( row, 0 ) };

        typeModel->setData( typeModelIdx, format );
        defaultModel->setData( tblModelIdx, format );
    }

    if ( logToFile && Settings::getSetting( SKeys::Logger, SSubKeys::LogFiles ).toBool() )
        emit this->insertLogSignal( type, message, time, newLine );
}

void Logger::createLogViews()
{
    for ( const auto& type : logType.keys() )
    {
        if ( type != LKeys::ChatLog )
            logViews.insert( new QStandardItemModel( 0, 1, nullptr ), type );
    }
}

void Logger::filterLogs()
{
    qint32 index{ ui->filterComboBox->currentIndex() };
    if ( static_cast<LKeys>( index ) >= LKeys::ChatLog ) //This log type is not valid for filtering.
        ++index;                                         //Adjust the index to the next valid log type.

    QStandardItemModel* model{ logViews.key( static_cast<LKeys>( index ), nullptr ) };
    if ( model != nullptr )
        ui->logView->setModel( model );

    this->scrollToBottom( true );
}

void Logger::clearLogs()
{
    static const QString logMsg{ "The Log View has been cleared!" };

    //Prevent updates to the view while clearing log models.
    ui->logView->setUpdatesEnabled( false );

    for ( const auto& type : logType.keys() )
    {
        if ( type != LKeys::ChatLog )
        {
            QStandardItemModel* model{ logViews.key( type, nullptr ) };
            if ( model != nullptr )
                model->setRowCount( 0 );
        }
    }

    ui->logView->setUpdatesEnabled( true );

    this->insertLog( "Logger", logMsg, LKeys::MiscLog, true, true );
}

void Logger::startAutoClearingLogs(const bool& start)
{
    if ( start )
        autoClearTimer.start();
    else
        autoClearTimer.stop();
}

//Slots
void Logger::insertLogSlot(const QString& source, const QString& message, const LKeys& type, const bool& logToFile, const bool& newLine)
{
    this->insertLog( source, message, type, logToFile, newLine );
}

void Logger::on_websiteLabel_linkActivated(const QString&)
{
    static const QString title{ "Open Link:" };
    QString prompt{ "Do you wish to open the website [ %1 ] in a browser window?" };
            prompt = prompt.arg( website );

    if ( Helper::confirmAction( this, title, prompt ) )
    {
        const QUrl websiteLink( website );
        QDesktopServices::openUrl( websiteLink );

        QString message{ "Opening a local Web Browser to the website [ %1 ] per user request." };
                message = message.arg( website );

        this->insertLog( "Logger", message, LKeys::MiscLog, true, true );
    }
}

void Logger::on_autoScroll_clicked()
{
    Settings::setSetting( ui->autoScroll->isChecked(), SKeys::Logger, SSubKeys::LoggerAutoScroll );
}

void Logger::resizeColumnsSlot(const LogCols&)
{
    if ( ui == nullptr )
        return;
}

void Logger::on_filterComboBox_currentIndexChanged(int)
{
    this->filterLogs();
}

void Logger::on_clearLogsButton_clicked()
{
    static const QString title{ "Clear Logs?" };
    static const QString prompt{ "Do you wish to erase the current Log View Data?" };

    if ( Helper::confirmAction( this, title, prompt ) )
        this->clearLogs();
}

void Logger::on_autoClear_toggled(bool checked)
{
    Settings::setSetting( checked, SKeys::Logger, SSubKeys::LoggerAutoClear );
    this->startAutoClearingLogs( checked );
}

void Logger::themeChangedSlot()
{
    const auto pal{ Theme::getCurrentPal() };
    ui->logView->setPalette( pal );
    ui->autoScroll->setPalette( pal );
    ui->autoClear->setPalette( pal );
}

void Logger::autoClearTimeOutSlot()
{
    this->clearLogs();
}
