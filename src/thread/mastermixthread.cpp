
//Class includes.
#include "mastermixthread.hpp"

//ReMix includes.
#include "widgets/settingswidget.hpp"
#include "settings.hpp"
#include "randdev.hpp"
#include "logger.hpp"
#include "helper.hpp"
#include "server.hpp"
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
QSettings* MasterMixThread::masterMixPref{ new QSettings( "synReal.ini", QSettings::IniFormat ) };
QTcpSocket* MasterMixThread::tcpSocket{ nullptr };
bool MasterMixThread::download;
QMutex MasterMixThread::mutex;

MasterMixThread::MasterMixThread()
{
    tcpSocket = new QTcpSocket( this );
    this->connectSlots();

    updateInfoTimer.setInterval( static_cast<qint32>( Globals::MASTER_MIX_UPDATE_INTERVAL ) ); //Default of 6 hours in Milliseconds..
    QObject::connect( &updateInfoTimer, &QTimer::timeout, this, [=, this]()
    {
        QString msg{ "Automatically refreshing the Master Mix Information." };
        emit this->insertLogSignal( "MasterMixThread", msg, LogTypes::MASTERMIX, true, true );

        masterMixPref->sync();
        this->updateMasterMixInfo( true );
    }, Qt::UniqueConnection );

    QObject::connect( this, &MasterMixThread::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot, Qt::UniqueConnection );
    QObject::connect( SettingsWidget::getInstance(), &SettingsWidget::masterMixInfoChangedSignal,
                      this, &MasterMixThread::masterMixInfoChangedSlot, Qt::UniqueConnection );

    updateInfoTimer.start();
}

MasterMixThread::~MasterMixThread()
{
    updateInfoTimer.stop();
    masterMixPref->sync();
    masterMixPref->deleteLater();

    tcpSocket->disconnect();
    tcpSocket->deleteLater();

    this->disconnect();
    this->deleteLater();
}

void MasterMixThread::connectSlots()
{
    QObject::connect( tcpSocket, &QTcpSocket::connected, tcpSocket,
    [=, this]()
    {
        tcpSocket->write( QString( "GET %1\r\n" ).arg( this->getModdedHost() ).toLatin1() );
    }, Qt::UniqueConnection );

    QObject::connect( tcpSocket, &QTcpSocket::readyRead, tcpSocket,
    [=, this]()
    {
        QFile synreal( "synReal.ini" );
        if ( synreal.open( QIODevice::WriteOnly | QIODevice::Append ) )
        {
            synreal.resize( 0 ); //Erase the file by overwriting previous data.
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
                msg = msg.arg( this->getModdedHost() )
                         .arg( bytes )
                         .arg( bytesUnit );

        masterMixPref->sync();

        emit this->insertLogSignal( "MasterMixThread", msg, LogTypes::MASTERMIX, true, true );
        emit this->masterMixInfoSignal(); //Inform Listening Objects of a completed download.
    }, Qt::UniqueConnection );
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
    QString host{ this->getModdedHost() };
    QFileInfo synRealFile( "synReal.ini" );

    QString message{ "Fetching Master Info from [ %1 ]." };
            message = message.arg( host );

    emit this->insertLogSignal( "MasterMixThread", message, LogTypes::MASTERMIX, true, true );

    QUrl url{ host };

    if ( tcpSocket == nullptr )
    {
        tcpSocket = new QTcpSocket( this );
        this->connectSlots();
    }

    if ( !download || forceDownload )
    {
        download = true;
        if ( !tcpSocket->isOpen() || forceDownload )
            tcpSocket->connectToHost( url.host(), 80 );
    }
}

QString MasterMixThread::getDefaultHost() const
{
    return defaultHost;
}

QString MasterMixThread::getModdedHost()
{
    QString host{ Settings::getSetting( SKeys::Setting, SSubKeys::OverrideMasterHost ).toString() };
    if ( host.isEmpty() )
        host = this->getDefaultHost();

    return host;
}

void MasterMixThread::obtainMasterData(Server* server)
{
    QMutexLocker<QMutex> locker( &mutex ); //Ensure thread safety.
    masterMixPref->sync();

    QString overrideIP{ Settings::getSetting( SKeys::Setting, SSubKeys::OverrideMasterIP ).toString() };
    if ( !overrideIP.isEmpty() )
    {
        qint32 index{ static_cast<int>( overrideIP.indexOf( ":" ) ) };
        if ( index > 0 )
        {
            server->setMasterIP( overrideIP.left( index ), static_cast<quint16>( overrideIP.mid( index + 1 ).toInt() ) );
            QString msg{ "Loaded Master Server Override [ %1:%2 ]." };
                    msg = msg.arg( server->getMasterIP() )
                             .arg( server->getMasterPort() );
            Logger::getInstance()->insertLog( server->getServerName(), msg, LogTypes::MASTERMIX, true, true );
        }
    }
    else
    {
        QString str{ masterMixPref->value( server->getGameName() % "/master" ).toString() };

        int index{ static_cast<int>( str.indexOf( ":" ) ) };
        if ( index > 0 )
        {
            QString ip{ str.left( index ) };
            quint16 port{ static_cast<quint16>( str.mid( index + 1 ).toInt() ) };

            QString msg{ "Got Master Server [ %1:%2 ] for Game [ %3 ]." };
            msg = msg.arg( ip )
                  .arg( port )
                  .arg( server->getGameName() );
            emit this->insertLogSignal( server->getServerName(), msg, LogTypes::MASTERMIX, true, true );

            server->setMasterIP( ip, port );
        }
    }
}

void MasterMixThread::masterMixInfoChangedSlot()
{
    this->updateMasterMixInfo( true );
}

void MasterMixThread::run()
{
    masterMixPref->moveToThread( this );
    masterMixPref->sync();

    this->exec();
}

void MasterMixThread::getMasterMixInfo(Server* server)
{
    QObject::connect( this, &MasterMixThread::masterMixInfoSignal, this, [=, this]()
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
