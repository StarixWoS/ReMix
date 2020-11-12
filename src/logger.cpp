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
Logger* Logger::logInstance;

const QString Logger::website{ "https://bitbucket.org/ahitb/remix" };
const QStringList Logger::logType =
{
    "AdminUsage",
    "Comments",
    "UsageLog",
    "UPNPLog",
    "PunishmentLog",
    "Misc",
    "ChatLog",
    "QuestLog",
    "PktForge",
    "PingLog",
};

Logger::Logger(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Logger)
{
    ui->setupUi(this);

    QThread* thread{ new QThread() };
    writeThread = WriteThread::getNewWriteThread( logType, nullptr );
    writeThread->moveToThread( thread );

    //Connect Objects to Slots.
    QObject::connect( this, &Logger::insertLogSignal, writeThread, &WriteThread::insertLogSlot );
    QObject::connect( this, &Logger::resizeColumnsSignal, this, &Logger::resizeColumnsSlot );

    tblModel = new QStandardItemModel( 0, 4, nullptr );
    tblModel->setHeaderData( static_cast<int>( LogCols::Message ), Qt::Horizontal, "Message" );
    tblModel->setHeaderData( static_cast<int>( LogCols::Source ), Qt::Horizontal, "Source" );
    tblModel->setHeaderData( static_cast<int>( LogCols::Date ), Qt::Horizontal, "Date" );
    tblModel->setHeaderData( static_cast<int>( LogCols::Type ), Qt::Horizontal, "Type" );

    ui->logView->setModel( tblModel );
    ui->logView->setColumnWidth( static_cast<int>( LogCols::Source ), 150 );
    ui->logView->setColumnWidth( static_cast<int>( LogCols::Date ), 150 );
    ui->logView->setColumnWidth( static_cast<int>( LogCols::Type ), 100 );

    ui->logView->horizontalHeader()->setStretchLastSection( true );
    ui->logView->horizontalHeader()->setVisible( true );
    ui->logView->verticalHeader()->setVisible( false );

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
    QObject::connect( &autoClearTimer, &QTimer::timeout, this, [=]()
    {
        this->clearLogs();
    });

    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        this->restoreGeometry( Settings::getSetting( SKeys::Positions, this->metaObject()->className() ).toByteArray() );

    thread->start();
}

Logger::~Logger()
{
    //Disable Logging Signals.
    this->disconnect();

    QThread* thread{ writeThread->thread() };
    if ( thread != nullptr )
        thread->exit();

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

void Logger::scrollToBottom()
{
    if ( ui->autoScroll->isChecked() )
    {
        QTableView* obj{ ui->logView };

        //Detect when the user is scrolling upwards. And prevent scrolling.
        if ( obj->verticalScrollBar()->sliderPosition() == obj->verticalScrollBar()->maximum() )
            obj->scrollToBottom();
    }
}

void Logger::insertLog(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine)
{
    QAbstractItemModel* tblModel{ ui->logView->model() };
    QString time{ Helper::getTimeAsString() };

    if ( tblModel != nullptr ) //Prevent Chat from appearing within the Logger UI.
    {
        qint32 row{ tblModel->rowCount() };
        //tblModel->insertRow( row );
        tblModel->insertRows( row, 1 );
        ui->logView->setRowHeight( row, 20 );

        this->updateRowData( row, static_cast<int>( LogCols::Type ), logType.at( static_cast<int>( type ) ) );
        this->updateRowData( row, static_cast<int>( LogCols::Message ), message.simplified() );
        this->updateRowData( row, static_cast<int>( LogCols::Source ), source );
        this->updateRowData( row, static_cast<int>( LogCols::Date ), time );

        //Insert the newly created row into the LoggerMap.
        //LogType as the value, and the row as the key.
        //This is to maintain purely unique combinations without overwriting data due to duplicate keys.
        logMap.insert( tblModel->index( row, 0 ), type );
    }

    if ( logToFile && Settings::getSetting( SKeys::Logger, SSubKeys::LogFiles ).toBool() )
        emit this->insertLogSignal( type, message, time, newLine );

    this->filterLogs();
}

void Logger::updateRowData(const qint32& row, const qint32& col, const QVariant& data)
{
    QModelIndex index{ tblModel->index( row, col ) };
    if ( index.isValid() )
    {
        if ( col == static_cast<int>( LogCols::Date ) )
            tblModel->setData( index, data.toString(), Qt::DisplayRole );
        else
            tblModel->setData( index, data, Qt::DisplayRole );
    }
}

void Logger::filterLogs()
{
    //qint32 rowCount{ tblModel->rowCount() };
    qint32 index{ ui->filterComboBox->currentIndex() - 1 };

    ui->logView->setUpdatesEnabled( false );

    bool filteringLogs{ index >= 0 };
    for ( const QModelIndex& idx : logMap.keys() )
    {
        if ( idx.isValid() )
        {
            if ( filteringLogs )
            {
                LogTypes type{ logMap.value( idx ) };
                if ( type == static_cast<LogTypes>( index ) )
                    ui->logView->showRow( idx.row() );
                else
                    ui->logView->hideRow( idx.row() );
            }
            else //Unfilter logs.
                ui->logView->showRow( idx.row() );
        }
    }

    ui->logView->setUpdatesEnabled( true );

    this->scrollToBottom();
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

    //ui->logView->resizeColumnToContents( static_cast<int>( column ) );
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
