
//Class includes.
#include "remix.hpp"
#include "ui_remix.h"

//ReMix Widget includes.
#include "widgets/remixtabwidget.hpp"

//ReMix includes.
#include "campexemption.hpp"
#include "settings.hpp"
#include "logger.hpp"
#include "helper.hpp"
#include "server.hpp"
#include "theme.hpp"
#include "user.hpp"

//Qt Includes.
#include <QNetworkAccessManager>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QSettings>
#include <QMenu>

ReMix* ReMix::instance{ nullptr };

ReMix::ReMix(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::ReMix)
{
    instance = this;

    //Register the LogTypes type for use within signals and slots.
    qRegisterMetaType<LogTypes>("LogTypes");
    //Register the QHostAddress type for use within signals and slots.
    qRegisterMetaType<QHostAddress>("QHostAddress");
    //Register the UserListResponse type for use within signals and slots.
    qRegisterMetaType<UserListResponse>("UserListResponse");
    //Register the type for use within signals and slots.
    qRegisterMetaType<QVector<int>>("QVector<int>");

    ui->setupUi(this);

    Logger::setInstance( new Logger( this ) );
    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        this->restoreGeometry( Settings::getSetting( SKeys::Positions, this->metaObject()->className() ).toByteArray() );

    //Setup Objects.
    Settings::setInstance( new Settings( this ) );
    User::setInstance( new User( this ) );

    serverUI = ReMixTabWidget::getTabInstance( this );
    ui->frame->layout()->addWidget( serverUI );

    //Update the window title to reflect the current version.
    QString title{ "ReMix[ %1 ]:" };
            title = title.arg( QString( REMIX_VERSION ) );
    this->setWindowTitle( title );

    //Initialize our Tray Icon if available.
    #if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
        this->initSysTray();
    #endif
}

ReMix::~ReMix()
{
    if ( Settings::getSetting( SKeys::Setting, SSubKeys::SaveWindowPositions ).toBool() )
        Settings::setSetting( this->saveGeometry(), SKeys::Positions, this->metaObject()->className() );

    if ( trayObject != nullptr )
        trayObject->deleteLater();

    if ( trayMenu != nullptr )
        trayMenu->deleteLater();

    CampExemption::getInstance()->deleteLater();
    Settings::getInstance()->deleteLater();
    Logger::getInstance()->deleteLater();
    Theme::getInstance()->deleteLater();
    User::getInstance()->deleteLater();

    serverUI->close();
    serverUI->deleteLater();

    instance->close();
    instance->deleteLater();

    Settings::prefs->deleteLater();
    Settings::bioHash.clear();
    delete ui;
}

ReMix* ReMix::getInstance()
{
    if ( instance == nullptr )
        instance = new ReMix();

    return instance;
}

void ReMix::updateTitleBars(Server* server)
{
    Settings* settings{ Settings::getInstance() };
    if ( settings != nullptr )
        settings->updateTabBar( server );

    QString title{ "ReMix[ %1 ]: %2 [ %3 ]" };
            title = title.arg( QString( REMIX_VERSION ) )
                         .arg( server->getServerName() )
                         .arg( server->getPrivatePort() );

    ReMix::getInstance()->setWindowTitle( title );
}

void ReMix::quitSlot()
{
    qApp->quit();
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

        QAction* minimizeAction{ new QAction( "Minimize", this ) };
        QAction* restoreAction{ new QAction( "Restore", this ) };
        QAction* quitAction{ new QAction( "Quit", this ) };

        QObject::connect( restoreAction, &QAction::triggered, this, &QMainWindow::showNormal );
        QObject::connect( minimizeAction, &QAction::triggered, this, &QMainWindow::hide );

        QObject::connect( quitAction, &QAction::triggered, quitAction,
        [=, this]()
        {
            //Allow Rejection of a Global CloseEvent.
            if ( !this->rejectCloseEvent() )
                qApp->quit();
        } );

        trayMenu = new QMenu( this );
        trayMenu->addSeparator();
        trayMenu->addAction( minimizeAction );
        trayMenu->addAction( restoreAction );
        trayMenu->addAction( quitAction );

        QObject::connect( trayObject, &QSystemTrayIcon::activated, trayObject,
        [=, this]( QSystemTrayIcon::ActivationReason reason )
        {
            if ( reason == QSystemTrayIcon::Trigger )
            {
                if ( this->isHidden() )
                {
                    this->show();
                    this->setWindowState( this->windowState() & ~Qt::WindowMinimized );
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

#if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
void ReMix::changeEvent(QEvent* event)
{
    if ( Settings::getSetting( SKeys::Setting, SSubKeys::MinimizeToTray ).toBool()
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

    QString title{ "Close [ %1 ] Server Instances:" };
            title = title.arg( serverUI->getServerCount() );

    QString prompt{ "You are about to shut down your ReMix game server!\r\nThis will affect [ %1 ] User(s) connected to it.\r\n\r\nAre you certain?" };
            prompt = prompt.arg( serverUI->getPlayerCount() );

    serverUI->sendMultiServerMessage( "The admin is taking this server down..." );

    if ( !Helper::confirmAction( this, title, prompt ) )
    {
        exiting = false;
        serverUI->sendMultiServerMessage( "The admin changed his or her mind! (yay!)..." );
        return true;
    }
    return false;
}
