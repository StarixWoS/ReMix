
#include "includes.hpp"
#include "remix.hpp"
#include "ui_remix.h"

Settings* ReMix::settings;
ReMix* ReMix::instance;
User* ReMix::user;

ReMix::ReMix(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReMix)
{
    ui->setupUi(this);

    this->setInstance( this );
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

    //Setup Objects.
    settings = new Settings( this );
    user = new User( this );

    serverUI = ReMixTabWidget::getTabInstance( this );
    ui->frame->layout()->addWidget( serverUI );

    //Initialize our Tray Icon if available.
    #if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
        this->initSysTray();
    #endif
}

ReMix::~ReMix()
{
    if ( Settings::getSaveWindowPositions() )
    {
        Settings::setWindowPositions( this->saveGeometry(),
                                      this->metaObject()->className() );
    }

    if ( trayObject != nullptr )
        trayObject->deleteLater();

    if ( trayMenu != nullptr )
        trayMenu->deleteLater();

    user->close();
    user->deleteLater();

    serverUI->close();
    serverUI->deleteLater();

    settings->close();
    settings->deleteLater();

    instance->close();
    instance->deleteLater();

    Settings::prefs->deleteLater();
    delete ui;
}

ReMix* ReMix::getInstance()
{
    return instance;
}

void ReMix::setInstance(ReMix* value)
{
    instance = value;
}

void ReMix::updateTitleBars(QString serverName, quint16 port)
{
    if ( settings != nullptr )
    {
        settings->updateTabBar( serverName );
    }
    QString title{ "ReMix: %1 [ %2 ]" };
            title = title.arg( serverName )
                         .arg( port );
    ReMix::getInstance()->setWindowTitle( title );
}

Settings* ReMix::getSettings()
{
    if ( settings == nullptr )
        settings = new Settings( );

    return settings;
}

User* ReMix::getUser()
{
    if ( user == nullptr )
        user = new User( );

    return user;
}

#if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
void ReMix::initSysTray()
{
    //While possible to create a system tray icon, some versions of linux
    //disallow applications to create their own.
    //Also disable the feature on OSX. --Unable to test.

    if ( QSystemTrayIcon::isSystemTrayAvailable()
      && !hasSysTray )
    {
        trayIcon = QIcon( QIcon( ":/icon/ReMix.png" ) );
        trayObject = new QSystemTrayIcon( trayIcon, this );
        trayObject->show();

        QAction* showAction = new QAction( "Show", this );
        QObject::connect( showAction, &QAction::triggered,
                          this, &QMainWindow::show );

        QAction* hideAction = new QAction( "Hide", this );
        QObject::connect( hideAction, &QAction::triggered,
                          this, &QMainWindow::hide );

        QAction* minimizeAction = new QAction( "Minimize", this );
        QObject::connect( minimizeAction, &QAction::triggered,
                          this, &QMainWindow::hide );

        QAction* maximizeAction = new QAction( "Maximize", this );
        QObject::connect( maximizeAction, &QAction::triggered,
                          this, &QMainWindow::showMaximized );

        QAction* restoreAction = new QAction( "Restore", this );
        QObject::connect( restoreAction, &QAction::triggered,
                          this, &QMainWindow::showNormal );

        QAction* quitAction = new QAction( "Quit", this );
        QObject::connect( quitAction, &QAction::triggered, quitAction,
        [=]()
        {
            //Allow Rejection of a Global CloseEvent.
            if ( !this->rejectCloseEvent() )
                qApp->quit();
        });

        trayMenu = new QMenu( this );
        trayMenu->addAction( showAction );
        trayMenu->addAction( hideAction );
        trayMenu->addSeparator();
        trayMenu->addAction( minimizeAction );
        trayMenu->addAction( maximizeAction );
        trayMenu->addAction( restoreAction );
        trayMenu->addAction( quitAction );

        QObject::connect( trayObject, &QSystemTrayIcon::activated, trayObject,
        [=]( QSystemTrayIcon::ActivationReason reason )
        {
            if ( reason == QSystemTrayIcon::Trigger )
            {
                if ( this->isHidden() )
                {
                    this->show();
                    this->setWindowState( this->windowState()
                                        & ~Qt::WindowMinimized );
                    this->activateWindow();
                }
                else
                {
                    this->hide();
                    this->setWindowState( Qt::WindowMinimized );
                }
            }
            else if ( reason == QSystemTrayIcon::Context )
            {
                if ( trayMenu != nullptr )
                    trayMenu->popup( QCursor::pos() );
            }
        });
        hasSysTray = true;
    }
}
#endif

void ReMix::getSynRealData(ServerInfo* svr)
{
    if ( svr == nullptr )
        return;

    QFileInfo synRealFile( "synReal.ini" );

    bool downloadFile = true;
    if ( synRealFile.exists() )
    {
        qint64 curTime = static_cast<qint64>(
                             QDateTime::currentDateTime()
                                  .toMSecsSinceEpoch() / 1000 );
        qint64 modTime = static_cast<qint64>(
                             synRealFile.lastModified()
                                  .toMSecsSinceEpoch() / 1000 );

        //Check if the file is 48 hours old and set our bool.
        downloadFile = ( curTime - modTime >= 172800 );
    }

    //The file was older than 48 hours or did not exist. Request a fresh copy.
    if ( downloadFile )
    {
        QTcpSocket* socket = new QTcpSocket;
        QUrl url( svr->getMasterInfoHost() );

        socket->connectToHost( url.host(), 80 );
        QObject::connect( socket, &QTcpSocket::connected, socket,
        [=]()
        {
            socket->write( QString( "GET %1\r\n" )
                               .arg( svr->getMasterInfoHost() )
                                             .toLatin1() );
        });

        QObject::connect( socket, &QTcpSocket::readyRead, socket,
        [=]()
        {
            QFile synreal( "synReal.ini" );
            if ( synreal.open( QIODevice::WriteOnly ) )
            {
                socket->waitForReadyRead();
                synreal.write( socket->readAll() );
            }

            synreal.close();

            QSettings settings( "synReal.ini", QSettings::IniFormat );
            QString str = settings.value( svr->getGameName()
                                        % "/master" ).toString();
            int index = str.indexOf( ":" );
            if ( index > 0 )
            {
                svr->setMasterIP( str.left( index ) );
                svr->setMasterPort(
                            static_cast<quint16>(
                                str.mid( index + 1 ).toInt() ) );
            }
        });

        QObject::connect( socket, &QTcpSocket::disconnected,
                          socket, &QTcpSocket::deleteLater );
    }
    else
    {
        QSettings settings( "synReal.ini", QSettings::IniFormat );
        QString str = settings.value( svr->getGameName()
                                    % "/master" ).toString();
        if ( !str.isEmpty() )
        {
            int index = str.indexOf( ":" );
            if ( index > 0 )
            {
                svr->setMasterIP( str.left( index ) );
                svr->setMasterPort(
                            static_cast<quint16>(
                                str.mid( index + 1 ).toInt() ) );
            }
        }
    }
}

#if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
void ReMix::changeEvent(QEvent* event)
{
    if ( Settings::getMinimizeToTray()
      && hasSysTray )
    {
        if ( event->type() == QEvent::WindowStateChange )
        {
            if ( this->isMinimized() )
                this->hide();
        }
    }
    QMainWindow::changeEvent( event );
}
#endif

void ReMix::closeEvent(QCloseEvent* event)
{
    if ( event == nullptr )
        return;

    if ( event->type() == QEvent::Close
      && !exiting )
    {
        if ( !this->rejectCloseEvent() )
        {
            event->accept();
            qApp->quit();
        }
        else
            event->ignore();
    }
    else
        QMainWindow::closeEvent( event );
}

bool ReMix::rejectCloseEvent()
{
    exiting = true;
    if ( serverUI == nullptr )
        return false;

    //There aren't any servers to keep open. Close without question.
    if ( serverUI->getServerCount() == 0 )
        return false;

    QString title = QString( "Close [ %1 ] Server Instances:" )
                        .arg( serverUI->getServerCount() );

    QString prompt = QString( "You are about to shut down your ReMix game "
                              "server!\r\nThis will affect [ %1 ] User(s) "
                              "connected to it.\r\n\r\nAre you certain?" )
                         .arg( serverUI->getPlayerCount() );

    serverUI->sendMultiServerMessage( "The admin is taking this server "
                                       "down...", nullptr, true );

    if ( !Helper::confirmAction( this, title, prompt ) )
    {
        exiting = false;
        serverUI->sendMultiServerMessage( "The admin changed his or her "
                                           "mind! (yay!)...", nullptr, true );
        return true;
    }
    return false;
}
