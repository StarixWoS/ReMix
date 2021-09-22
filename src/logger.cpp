
//Class includes.
#include "logger.hpp"
#include "ui_logger.h"

//ReMix includes.
#include "thread/writethread.hpp"
#include "settings.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QGraphicsPixmapItem>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QGraphicsItem>
#include <QScrollBar>
#include <QVariant>
#include <QPixmap>
#include <QString>
#include <QObject>
#include <QtCore>

QStandardItemModel* Logger::tblModel{ nullptr };

QMap<QModelIndex, LogTypes> Logger::logMap;
Logger* Logger::logInstance{ nullptr };

const QString Logger::website{ "https://bitbucket.org/ahitb/remix" };
const QStringList Logger::logType =
{
    "AllLogs", //Unused, placeholder.
    "AdminUsageLog",
    "CommentLog",
    "ClientLog",
    "MasterMixLog",
    "UPNPLog",
    "PunishmentLog",
    "MiscLog",
    "ChatLog",
    "QuestLog",
    "PingLog",
};

Logger::Logger(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Logger)
{
    ui->setupUi(this);

    logThread = new QThread();
    writeThread = WriteThread::getNewWriteThread( logType, nullptr );
    writeThread->moveToThread( logThread );

    //Connect Objects to Slots.
    QObject::connect( this, &Logger::insertLogSignal, writeThread, &WriteThread::insertLogSlot );
    QObject::connect( this, &Logger::resizeColumnsSignal, this, &Logger::resizeColumnsSlot );

    tblModel = new QStandardItemModel( 0, 4, nullptr );
    tblModel->setHeaderData( static_cast<int>( LogCols::Message ), Qt::Horizontal, "Message" );
    tblModel->setHeaderData( static_cast<int>( LogCols::Source ), Qt::Horizontal, "Source" );
    tblModel->setHeaderData( static_cast<int>( LogCols::Date ), Qt::Horizontal, "Date" );
    tblModel->setHeaderData( static_cast<int>( LogCols::Type ), Qt::Horizontal, "Type" );
    ui->logView->setModel( tblModel );

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
    autoClearTimer.setInterval( 86400 * 1000 );
    QObject::connect( &autoClearTimer, &QTimer::timeout, this, [=, this]()
    {
        this->clearLogs();
    });

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        this->restoreGeometry( Settings::getSetting( SKeys::Positions, this->metaObject()->className() ).toByteArray() );

    logThread->start();
}

Logger::~Logger()
{
    //Disable Logging Signals.
    this->disconnect();

    if ( logThread != nullptr )
    {
        logThread->exit();
        writeThread->deleteLater();
    }

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        Settings::setSetting( this->saveGeometry(), SKeys::Positions, this->metaObject()->className() );

    iconViewerScene->removeItem( iconViewerItem );
    iconViewerScene->deleteLater();

    tblModel->deleteLater();
    this->deleteLater();

    delete iconViewerItem;
    delete ui;
}

Logger* Logger::getInstance()
{
    if ( logInstance == nullptr )
        logInstance = new Logger();

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
        QListView* obj{ ui->logView };

        //Detect when the user is scrolling upwards. And prevent scrolling.
        if ( obj->verticalScrollBar()->sliderPosition() == obj->verticalScrollBar()->maximum()
          || forceScroll )
        {
            obj->scrollToBottom();
        }
    }
}

void Logger::insertLog(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine)
{
    QAbstractItemModel* tblModel{ ui->logView->model() };
    QString time{ Helper::getTimeAsString() };
    QString format{ "%1 - %2 - %3 - %4" }; //Format string. - Easier to modify.

    if ( tblModel != nullptr
      && type != LogTypes::CHAT ) //Hide the Chat from the Log View.
    {
        qint32 row{ tblModel->rowCount() };
        tblModel->insertRows( row, 1 );

        auto idx{ tblModel->index( row, 0 ) };
        format = format.arg( time )
                       .arg( logType.at( static_cast<int>( type ) ) )
                       .arg( source )
                       .arg( message.simplified() );
        tblModel->setData( idx, format, Qt::DisplayRole );

        //Insert the newly created row into the LoggerMap.
        //LogType as the value, and the row as the key.
        //This is to maintain purely unique combinations without overwriting data due to duplicate keys.
        logMap.insert( idx, type );
    }

    if ( logToFile && Settings::getSetting( SKeys::Logger, SSubKeys::LogFiles ).toBool() )
        emit this->insertLogSignal( type, message, time, newLine );

    this->filterLogs();
}

void Logger::filterLogs()
{
    //qint32 rowCount{ tblModel->rowCount() };
    qint32 index{ ui->filterComboBox->currentIndex() };
    if ( static_cast<LogTypes>( index ) >= LogTypes::CHAT ) //This log type is not valid for filtering.
        ++index;                                            //Adjust the index to the next valid log type.

    ui->logView->setUpdatesEnabled( false );

    bool filteringLogs{ index != static_cast<int>( LogTypes::ALL ) };
    for ( const QModelIndex& idx : logMap.keys() )
    {
        if ( idx.isValid() )
        {
            if ( filteringLogs )
            {
                LogTypes type{ logMap.value( idx ) };
                if ( type == static_cast<LogTypes>( index ) )
                    ui->logView->setRowHidden( idx.row(), false );
                else
                    ui->logView->setRowHidden( idx.row(), true );
            }
            else //Unfilter logs.
                ui->logView->setRowHidden( idx.row(), false );
        }
    }

    ui->logView->setUpdatesEnabled( true );

    this->scrollToBottom( true );
}

void Logger::clearLogs()
{
    QString logMsg{ "The Log View has been cleared!" };

    ui->logView->setUpdatesEnabled( false );

    tblModel->setRowCount( 0 );
    logMap.clear();

    ui->logView->setUpdatesEnabled( true );

    this->insertLog( "Logger", logMsg, LogTypes::MISC, true, true );
}

void Logger::startAutoClearingLogs(const bool& start)
{
    if ( start )
        autoClearTimer.start();
    else
        autoClearTimer.stop();
}

//Slots
void Logger::insertLogSlot(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine)
{
    this->insertLog( source, message, type, logToFile, newLine );
}

void Logger::on_websiteLabel_linkActivated(const QString&)
{
    QString title{ "Open Link?" };
    QString prompt{ "Do you wish to open the website [ %1 ] in a browser window?" };
    prompt = prompt.arg( website );

    if ( Helper::confirmAction( this, title, prompt ) )
    {
        QUrl websiteLink( website );
        QDesktopServices::openUrl( websiteLink );

        QString message{ "Opening a local Web Browser to the website [ %1 ] per user request." };
        message = message.arg( website );

        this->insertLog( "Logger", message, LogTypes::MISC, true, true );
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
    QString title{ "Clear Logs?" };
    QString prompt{ "Do you wish to erase the current Log View Data?" };

    if ( Helper::confirmAction( this, title, prompt ) )
        this->clearLogs();
}

void Logger::on_autoClear_toggled(bool checked)
{
    Settings::setSetting( checked, SKeys::Logger, SSubKeys::LoggerAutoClear );
    this->startAutoClearingLogs( checked );
}

void Logger::on_logView_customContextMenuRequested(const QPoint& pos)
{
    qDebug() << pos;
}
