
//Class includes.
#include "mastermixthread.hpp"

//ReMix includes.
#include "widgets/settingswidget.hpp"
#include "settings.hpp"
#include "logger.hpp"
#include "helper.hpp"

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

//Initialize our accepted Games List.
const QMap<Games, QString> MasterMixThread::gameNames =
{
    { Games::WoS, "WoS" },
    { Games::ToY, "TOY" },
    { Games::W97, "W97" },
};

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
        emit this->insertLogSignal( "MasterMixThread", msg, LKeys::MasterMixLog, true, true );

        masterMixPref->sync();
        this->updateMasterMixInfo( true );
    }, Qt::UniqueConnection );

    QObject::connect( this, &MasterMixThread::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot, Qt::UniqueConnection );
    QObject::connect( this, &MasterMixThread::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot, Qt::UniqueConnection );
    QObject::connect( SettingsWidget::getInstance(), &SettingsWidget::masterMixInfoChangedSignal,
                      this, &MasterMixThread::masterMixInfoChangedSlot, Qt::UniqueConnection );

    updateInfoTimer.start();
}

MasterMixThread::~MasterMixThread()
{
    connectedGames.clear();

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
        static QFile synreal( "synReal.ini" );
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

        emit this->insertLogSignal( "MasterMixThread", msg, LKeys::MasterMixLog, true, true );
        emit this->obtainedMasterMixInfoSignal(); //Inform Listening Objects of a completed download.
    }, Qt::UniqueConnection );

    QObject::connect( tcpSocket, &QTcpSocket::errorOccurred, tcpSocket,
    [](QAbstractSocket::SocketError socketError)
    {
        qDebug() << socketError;
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
    const QString host{ this->getModdedHost() };
    QString message{ "Fetching Master Info from [ %1 ]." };
            message = message.arg( host );

    emit this->insertLogSignal( "MasterMixThread", message, LKeys::MasterMixLog, true, true );

    const QUrl url{ host };
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

void MasterMixThread::obtainMasterData(const Games& game)
{
    QMutexLocker<QMutex> locker( &mutex ); //Ensure thread safety.
    masterMixPref->sync();

    const QString overrideIP{ Settings::getSetting( SKeys::Setting, SSubKeys::OverrideMasterIP ).toString() };
    if ( !overrideIP.isEmpty() )
    {
        qint32 index{ static_cast<int>( overrideIP.indexOf( ":" ) ) };
        if ( index > 0 )
        {
            const QString ip{ overrideIP.left( index ) };
            const quint16 port{ static_cast<quint16>( overrideIP.mid( index + 1 ).toInt() ) };

            QString msg{ "Loaded Master Server Override [ %1:%2 ]." };
                    msg = msg.arg( ip )
                             .arg( port );

            emit this->masterMixInfoSignal( game, ip, port );
            emit this->insertLogSignal( "MasterMixThread", msg, LKeys::MasterMixLog, true, true );
        }
    }
    else
        this->parseMasterInfo( game );
}

void MasterMixThread::getMasterMixInfoSlot(const Games& game)
{
    if ( !connectedGames.contains( game ) )
    {
        QObject::connect( this, &MasterMixThread::obtainedMasterMixInfoSignal, this,
        [=, this]()
        {
            this->obtainMasterData( game );
        }, Qt::ConnectionType::UniqueConnection );
        connectedGames.append( game );
    }

    if ( !download )
        this->updateMasterMixInfo( false );
    else
        this->obtainMasterData( game );
}

void MasterMixThread::masterMixInfoChangedSlot()
{
    this->updateMasterMixInfo( true );
}

void MasterMixThread::parseMasterInfo(const Games& game)
{
    const QString gameStr{ gameNames.value( game, "WoS" ) };
    const QString info{ masterMixPref->value( gameStr % "/master" ).toString() };

    int index{ static_cast<int>( info.indexOf( ":" ) ) };
    if ( index > 0 )
    {
        const QString ip{ info.left( index ) };
        const quint16 port{ static_cast<quint16>( info.mid( index + 1 ).toInt() ) };

        QString msg{ "Obtained Master Server [ %1:%2 ] for Game [ %3 ]." };
                msg = msg.arg( ip )
                         .arg( port )
                         .arg( gameStr );

        emit this->masterMixInfoSignal( game, ip, port );
        emit this->insertLogSignal( "MasterMixThread", msg, LKeys::MasterMixLog, true, true );
    }
}

void MasterMixThread::run()
{
    masterMixPref->moveToThread( this );
    masterMixPref->sync();

    this->exec();
}

MasterMixThread* MasterMixThread::getInstance()
{
    static MasterMixThread* instance;
    if ( instance == nullptr )
        instance = new MasterMixThread();

    return instance;
}
