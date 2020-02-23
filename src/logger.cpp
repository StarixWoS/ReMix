#include "logger.hpp"
#include "ui_logger.h"

//ReMix includes.
#include "views/loggersortproxymodel.hpp"
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
    "BanLog",
    "DCLog",
    "MuteLog",
    "Ignored",
    "Misc",
    "PacketForge",
    "ChatLog",
};

Logger::Logger(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Logger)
{
    ui->setupUi(this);

    tblModel = new QStandardItemModel( 0, 4, nullptr );
    tblModel->setHeaderData( static_cast<int>( LogCols::Date ),
                             Qt::Horizontal, "Date" );
    tblModel->setHeaderData( static_cast<int>( LogCols::Source ),
                             Qt::Horizontal, "Source" );
    tblModel->setHeaderData( static_cast<int>( LogCols::Type ),
                             Qt::Horizontal, "Type" );
    tblModel->setHeaderData( static_cast<int>( LogCols::Message ),
                             Qt::Horizontal, "Message" );

    ui->logView->setModel( tblModel );

    //Proxy model to support sorting without actually
    //altering the underlying model
    tblProxy = new LoggerSortProxyModel();
    tblProxy->setDynamicSortFilter( true );
    tblProxy->setSourceModel( tblModel );
    tblProxy->setSortCaseSensitivity( Qt::CaseInsensitive );
    ui->logView->setModel( tblProxy );

    ui->logView->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );
    ui->logView->verticalHeader()->setDefaultSectionSize( 15 );
    ui->logView->verticalHeader()->setVisible( false );

    ui->logView->horizontalHeader()->setStretchLastSection( true );

    //Load the application Icon into the top left of the dialog.
    iconViewerScene = new QGraphicsScene();
    iconViewerItem = new QGraphicsPixmapItem(
                         QPixmap::fromImage(
                             QImage( ":/icon/ReMix.png" ) ) );
    iconViewerScene->addItem( iconViewerItem );

    ui->iconViewer->setScene( iconViewerScene );
    ui->iconViewer->show();

    //Set the version number into the versionLabel.
    QString versionText{ ui->versionLabel->text() };
    versionText = versionText.arg( QString( REMIX_VERSION ) );
    ui->versionLabel->setText( versionText );

    //Restore the AutoLog setting.
    ui->autoScroll->setChecked( Settings::getLoggerAutoScroll() );

    if ( Settings::getSaveWindowPositions() )
    {
        QByteArray geometry{ Settings::getWindowPositions(
                             this->metaObject()->className() ) };
        if ( !geometry.isEmpty() )
        {
            this->restoreGeometry( Settings::getWindowPositions(
                                       this->metaObject()->className() ) );
        }
    }
}

Logger::~Logger()
{
    if ( Settings::getSaveWindowPositions() )
    {
        Settings::setWindowPositions( this->saveGeometry(),
                                      this->metaObject()->className() );
    }

    iconViewerScene->removeItem( iconViewerItem );
    iconViewerScene->deleteLater();

    tblProxy->deleteLater();
    tblModel->deleteLater();

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
        if ( obj->verticalScrollBar()->sliderPosition()
          == obj->verticalScrollBar()->maximum() )
        {
            obj->scrollToBottom();
        }
    }
}

void Logger::insertLog(const QString& source, const QString& message,
                       const LogTypes& type, const bool& logToFile,
                       const bool& newLine)
{
    QAbstractItemModel* tblModel = ui->logView->model();
    QString time = Helper::getTimeAsString();

    if ( tblModel != nullptr
      && ( type != LogTypes::Chat ) ) //Prevent Chat from appearing
                                      //within the Logger UI.
    {
        qint32 row = tblModel->rowCount();
        tblModel->insertRow( row );
        ui->logView->setRowHeight( row, 20 );

        this->updateRowData( row,
                             static_cast<int>( LogCols::Date ),
                             time );
        ui->logView->resizeColumnToContents(
                    static_cast<int>( LogCols::Date ) );

        this->updateRowData( row,
                             static_cast<int>( LogCols::Source ),
                             source );
        ui->logView->resizeColumnToContents(
                    static_cast<int>( LogCols::Source ) );

        this->updateRowData( row,
                             static_cast<int>( LogCols::Type ),
                             logType.at( static_cast<int>( type ) ) );
        ui->logView->resizeColumnToContents(
                    static_cast<int>( LogCols::Type ) );

        this->updateRowData( row,
                             static_cast<int>( LogCols::Message ),
                             message.simplified() );

        //ui->logView->resizeRowsToContents();

        this->scrollToBottom();
    }

    if ( logToFile && Settings::getLogFiles() )
    {
        this->logToFile( type, message, time, newLine );
    }
}

void Logger::updateRowData(const qint32& row, const qint32& col,
                           const QVariant& data)
{
    QModelIndex index = tblModel->index( row, col );
    if ( index.isValid() )
    {
        QString msg{ "" };
        if ( col == static_cast<int>( LogCols::Date ) )
        {
            msg = data.toString();

            tblModel->setData( index, msg, Qt::DisplayRole );
            if ( col == static_cast<int>( LogCols::Date ) )
                ui->logView->resizeColumnToContents( static_cast<int>(
                                                         LogCols::Date ) );
        }
        else
        {
            tblModel->setData( index, data, Qt::DisplayRole );
        }
    }
}

void Logger::logToFile(const LogTypes& type, const QString& text,
                       const QString& timeStamp,
                       const bool& newLine)
{
    if ( Settings::getLogFiles() )
    {
        QString logTxt = text;

        QFile log( "logs/"
                 % logType.at( static_cast<int>( type ) )
                 % QDate::currentDate().toString( "/[yyyy-MM-dd]/" )
                 % logType.at( static_cast<int>( type ) )
                 % ".txt" );

        QFileInfo logInfo( log );
        if ( !logInfo.dir().exists() )
            logInfo.dir().mkpath( "." );

        if ( log.open( QFile::WriteOnly | QFile::Append ) )
        {
            logTxt.prepend( "[ " % timeStamp % " ] " );

            if ( newLine )
                logTxt.prepend( "\r\n" );

            log.write( logTxt.toLatin1() );

            log.close();
        }
    }
}

void Logger::on_websiteLabel_linkActivated(const QString&)
{
    QString title{ "Open Link?" };
    QString prompt{ "Do you wish to open the website [ %1 ] "
                    "in a browser window?" };
    prompt = prompt.arg( website );

    if ( Helper::confirmAction( this, title, prompt ) )
    {
        QUrl websiteLink( website );
        QDesktopServices::openUrl( websiteLink );

        QString message{ "Opening a local Web Browser to the website [ %1 ] "
                         "per user request." };
        message = message.arg( website );

        this->insertLog( "Logger", message, LogTypes::MISC, false, false );
    }
}

void Logger::on_autoScroll_clicked()
{
    Settings::setLoggerAutoScroll( ui->autoScroll->isChecked() );
}
