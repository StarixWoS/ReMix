
//Class includes.
#include "mastermixthread.hpp"

//ReMix includes.
#include "widgets/settingswidget.hpp"
#include "serverinfo.hpp"
#include "settings.hpp"
#include "randdev.hpp"
#include "logger.hpp"
#include "helper.hpp"
#include "user.hpp"

//Qt Includes.
#include <QNetworkInterface>
#include <QInputDialog>
#include <QHostAddress>
#include <QMessageBox>
#include <QTcpSocket>
#include <QSettings>
#include <QtCore>

//Initialize our QSettings Object globally to make things more responsive.
//Should be Thread safe.
QSettings* MasterMixThread::masterMixPref{ new QSettings( "synreal.ini", QSettings::IniFormat ) };
QTcpSocket* MasterMixThread::tcpSocket{ nullptr };
bool MasterMixThread::download;
QMutex MasterMixThread::mutex;

MasterMixThread::MasterMixThread()
{
    tcpSocket = new QTcpSocket( this );
    updateInfoTimer.setInterval( 21600 * 1000 ); //Default of 24 hours.

    QObject::connect( &updateInfoTimer, &QTimer::timeout, this, [=]()
    {
        QString msg{ "Automatically refreshing the Master Mix Information." };
        emit this->insertLogSignal( "MasterMixThread", msg, LogTypes::MASTERMIX, true, true );

        this->updateMasterMixInfo( true );
    } );

    QObject::connect( this, &MasterMixThread::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );
    QObject::connect( SettingsWidget::getInstance(), &SettingsWidget::masterMixInfoChangedSignal, this, &MasterMixThread::masterMixInfoChangedSlot );

    updateInfoTimer.start();
}

MasterMixThread::~MasterMixThread()
{
    updateInfoTimer.stop();
    masterMixPref->deleteLater();

    tcpSocket->disconnect();
    tcpSocket->deleteLater();

    this->disconnect();
    this->deleteLater();
}

void MasterMixThread::startUpdateInfoTimer(const bool& start)
{
    if ( start )
        updateInfoTimer.start();
    else
        updateInfoTimer.stop();
}

void MasterMixThread::updateMasterMixInfo(const bool& forceDownload)
{
    QString defaultHost{ "http://synthetic-reality.com/synreal.ini" };
    QFileInfo synRealFile( "synReal.ini" );

    QString host{ Settings::getSetting( SKeys::Setting, SSubKeys::OverrideMasterHost ).toString() };
    if ( host.isEmpty() )
        host = defaultHost;

    QString message{ "Fetching Master Info from [ %1 ]." };
            message = message.arg( host );

    emit this->insertLogSignal( "MasterMixThread", message, LogTypes::MASTERMIX, true, true );

    QUrl url{ host };

    if ( tcpSocket == nullptr )
        new QTcpSocket( this );

    if ( !download || forceDownload )
    {
        download = true;
        tcpSocket->connectToHost( url.host(), 80 );
    }

    QObject::connect( tcpSocket, &QTcpSocket::connected, tcpSocket,
    [=]()
    {
        tcpSocket->write( QString( "GET %1\r\n" )
                           .arg( host ).toLatin1() );
    }, Qt::UniqueConnection );

    QObject::connect( tcpSocket, &QTcpSocket::readyRead, tcpSocket,
    [=]()
    {
        download = true;
        QFile synreal( "synReal.ini" );
        if ( synreal.open( QIODevice::WriteOnly | QIODevice::Append ) )
        {
            synreal.seek( 0 ); //Erase the file by overwriting previous data.
            tcpSocket->waitForReadyRead();

            synreal.write( tcpSocket->readAll() );
        }

        synreal.flush();
        synreal.close();

        this->startUpdateInfoTimer( true );

        QString bytesUnit{ "" };
        QString bytes{ "" };
        Helper::sanitizeToFriendlyUnits( static_cast<quint64>( synreal.size() ), bytes, bytesUnit );

        QString msg{ "Obtained Master Info from [ %1 ] with a file size of [ %2 %3 ]." };
                msg = msg.arg( host )
                         .arg( bytes )
                         .arg( bytesUnit );

        emit this->insertLogSignal( "MasterMixThread", msg, LogTypes::MASTERMIX, true, true );
        emit this->masterMixInfoSignal(); //Inform Listening Objects of a completed download.
    }, Qt::UniqueConnection );
}

void MasterMixThread::obtainMasterData(ServerInfo* server)
{
    QString str{ masterMixPref->value( server->getGameName() % "/master" ).toString() };

    int index{ str.indexOf( ":" ) };
    if ( index > 0 )
    {
        QString ip{ str.left( index ) };
        quint16 port{ static_cast<quint16>( str.midRef( index + 1 ).toInt() ) };

        QString msg{ "Got Master Server [ %1:%2 ] for Game [ %3 ]." };
                msg = msg.arg( ip )
                         .arg( port )
                         .arg( server->getGameName() );
        emit this->insertLogSignal( server->getServerName(), msg, LogTypes::MASTERMIX, true, true );

        server->setMasterIP( ip, port );
    }
}

void MasterMixThread::masterMixInfoChangedSlot()
{
    this->updateMasterMixInfo( true );
}

void MasterMixThread::run()
{
    masterMixPref->moveToThread( this );

    this->exec();
}

void MasterMixThread::getMasterMixInfo(ServerInfo* server)
{
    QMutexLocker locker( &mutex ); //Ensure thread safety.
    QObject::connect( this, &MasterMixThread::masterMixInfoSignal, this, [=]()
    {
        this->obtainMasterData( server );
    }, Qt::ConnectionType::UniqueConnection );

    if ( !download )
        this->updateMasterMixInfo( false );
    else
        this->obtainMasterData( server );
}

MasterMixThread* MasterMixThread::getInstance()
{
    static MasterMixThread* instance;
    if ( instance == nullptr )
        instance = new MasterMixThread();

    return instance;
}
