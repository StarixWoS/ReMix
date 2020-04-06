#include "logger.hpp"
#include "ui_logger.h"

//ReMix includes.
#include "views/loggersortproxymodel.hpp"
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

LoggerSortProxyModel* Logger::tblProxy{ nullptr };
QStandardItemModel* Logger::tblModel{ nullptr };
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
};

Logger::Logger(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Logger)
{
    ui->setupUi(this);

    QThread* thread{ new QThread() };
    writeThread = WriteThread::getNewWriteThread( logType, nullptr );
    writeThread->moveToThread( thread );

    //Connect the Logger Class to the WriteThread Class.
    QObject::connect( this, &Logger::insertLogSignal, writeThread, &WriteThread::insertLogSlot, Qt::QueuedConnection );

    tblModel = new QStandardItemModel( 0, 4, nullptr );
    tblModel->setHeaderData( static_cast<int>( LogCols::Date ), Qt::Horizontal, "Date" );
    tblModel->setHeaderData( static_cast<int>( LogCols::Source ), Qt::Horizontal, "Source" );
    tblModel->setHeaderData( static_cast<int>( LogCols::Type ), Qt::Horizontal, "Type" );
    tblModel->setHeaderData( static_cast<int>( LogCols::Message ), Qt::Horizontal, "Message" );

    QObject::connect( this, &Logger::resizeColumnsSignal, this, &Logger::resizeColumnsSlot, Qt::QueuedConnection );
    ui->logView->setModel( tblModel );

    //Proxy model to support sorting without actually
    //altering the underlying model
    tblProxy = new LoggerSortProxyModel();
    tblProxy->setDynamicSortFilter( true );
    tblProxy->setSourceModel( tblModel );
    tblProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->logView->setModel( tblProxy );

    //ui->logView->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );
    //ui->logView->verticalHeader()->setDefaultSectionSize( 15 );
    ui->logView->verticalHeader()->setVisible( false );
    ui->logView->horizontalHeader()->setVisible( true );

    ui->logView->horizontalHeader()->setStretchLastSection( true );

    //Load the application Icon into the top left of the dialog.
    iconViewerScene = new QGraphicsScene();
    iconViewerItem = new QGraphicsPixmapItem( QPixmap::fromImage( QImage( ":/icon/ReMix.png" ) ) );
    iconViewerScene->addItem( iconViewerItem );

    ui->iconViewer->setScene( iconViewerScene );
    ui->iconViewer->show();

    //Set the version number into the versionLabel.
    QString versionText{ ui->versionLabel->text() };
    versionText = versionText.arg( QString( REMIX_VERSION ) );
    ui->versionLabel->setText( versionText );

    //Restore the AutoLog setting.
    ui->autoScroll->setChecked( Settings::getSetting( SettingKeys::Logger, SettingSubKeys::LoggerAutoScroll ).toBool() );

    if ( Settings::getSetting( SettingKeys::Setting, SettingSubKeys::SaveWindowPositions ).toBool() )
        this->restoreGeometry( Settings::getSetting( SettingKeys::Positions, this->metaObject()->className() ).toByteArray() );

    thread->start();
}

Logger::~Logger()
{
    //Disable Logging Signals.
    this->disconnect();

    QThread* thread = writeThread->thread();
    if ( thread != nullptr )
        thread->exit();

    if ( Settings::getSetting( SettingKeys::Setting, SettingSubKeys::SaveWindowPositions ).toBool() )
        Settings::setSetting( this->saveGeometry(), SettingKeys::Positions, this->metaObject()->className() );

    iconViewerScene->removeItem( iconViewerItem );
    iconViewerScene->deleteLater();

    tblProxy->deleteLater();
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

        //Detect when the user is scrolling upwards.
        //And prevent scrolling.
        if ( obj->verticalScrollBar()->sliderPosition() == obj->verticalScrollBar()->maximum() )
            obj->scrollToBottom();
    }
}

void Logger::insertLog(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine)
{
    QAbstractItemModel* tblModel = ui->logView->model();
    QString time = Helper::getTimeAsString();

    if ( tblModel != nullptr
      && ( type != LogTypes::CHAT ) ) //Prevent Chat from appearing
                                      //within the Logger UI.
    {
        qint32 row = tblModel->rowCount();
        tblModel->insertRow( row );
        ui->logView->setRowHeight( row, 20 );

        this->updateRowData( row, static_cast<int>( LogCols::Date ), time );
        //emit this->resizeColumnsSlot( LogCols::Date );

        this->updateRowData( row, static_cast<int>( LogCols::Source ), source );
        //emit this->resizeColumnsSlot( LogCols::Source );

        this->updateRowData( row, static_cast<int>( LogCols::Type ), logType.at( static_cast<int>( type ) ) );
        //emit this->resizeColumnsSlot( LogCols::Type );

        this->updateRowData( row, static_cast<int>( LogCols::Message ), message.simplified() );
        //emit this->resizeColumnsSlot( LogCols::Message );

        ui->logView->resizeColumnsToContents();
        this->scrollToBottom();
    }

    if ( logToFile && Settings::getSetting( SettingKeys::Logger, SettingSubKeys::LogFiles ).toBool() )
        emit this->insertLogSignal( type, message, time, newLine );
}

void Logger::updateRowData(const qint32& row, const qint32& col, const QVariant& data)
{
    QModelIndex index = tblModel->index( row, col );
    if ( index.isValid() )
    {
        QString msg{ "" };
        if ( col == static_cast<int>( LogCols::Date ) )
        {
            msg = data.toString();

            tblModel->setData( index, msg, Qt::DisplayRole );
            //if ( col == static_cast<int>( LogCols::Date ) )
            //    emit this->resizeColumnsSlot( LogCols::Date );
        }
        else
            tblModel->setData( index, data, Qt::DisplayRole );
    }
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

        this->insertLog( "Logger", message, LogTypes::MISC, false, false );
    }
}

void Logger::on_autoScroll_clicked()
{
    Settings::setSetting( ui->autoScroll->isChecked(), SettingKeys::Logger, SettingSubKeys::LoggerAutoScroll );
}

void Logger::resizeColumnsSlot(const LogCols& column)
{
    if ( ui == nullptr )
        return;

    ui->logView->resizeColumnToContents( static_cast<int>( column ) );
}
