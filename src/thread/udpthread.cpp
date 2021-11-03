
//Class includes.
#include "thread/udpthread.hpp"

//ReMix Includes
#include "packethandler.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "user.hpp"

//Qt Includes.
#include <QDateTime>
#include <QtCore>
#include <QDebug>

UdpThread::UdpThread(QObject *parent)
    : QThread(parent)
{
    QObject::connect( this, &UdpThread::insertLogSignal, Logger::getInstance(), &Logger::insertLogSlot );
    QObject::connect( this, &UdpThread::logBIOSignal, User::getInstance(), &User::logBIOSlot );
}

UdpThread::~UdpThread()
{
    if ( socket != nullptr )
    {
        socket->close();
        socket->disconnect();
        socket->deleteLater();
    }

    this->disconnect();
    this->deleteLater();
}

UdpThread* UdpThread::getNewUdpThread(QObject* parent)
{
    return new UdpThread( parent );
}

void UdpThread::run()
{
    socket = new QUdpSocket();
    if ( socket != nullptr )
    {
        socket->moveToThread( this->thread() );
        QObject::connect( socket, &QUdpSocket::readyRead, this, &UdpThread::readyReadUDPSlot, Qt::DirectConnection );
    }

    this->exec();
}

void UdpThread::parseUdpPacket(const QByteArray& udp, const QHostAddress& ipAddr, const quint16& port)
{
    QString data{ udp };
    QString sernum{ "" };

    if ( !data.isEmpty() )
    {
        emit this->setBytesInSignal( static_cast<quint64>( data.size() ) );

        QChar opCode{ data.at( 0 ).toLatin1() };
        switch ( opCode.toLatin1() )
        {
            case 'G':   //Set the Server's gameInfoString.
            {
                emit this->recvPlayerGameInfoSignal( data, ipAddr.toString() );
            }
            break;
            case 'P':   //Store the Player information into a struct.
            {
                qint32 index{ Helper::getStrIndex( data, "sernum=" ) };
                if ( index >= 0 )
                {
                    sernum = data.mid( index + 7 );
                    index = Helper::getStrIndex( sernum, "," );
                    if ( index >= 0 )
                    {
                        sernum = sernum.left( index );
                        if ( !sernum.isEmpty() )
                            sernum = Helper::serNumToHexStr( sernum );
                    }
                }

                bool reqSernum{ Settings::getSetting( SKeys::Setting, SSubKeys::ReqSerNum ).toBool() };
                if (( reqSernum && Helper::serNumtoInt( sernum, true ) )
                  || !reqSernum )
                {
                    QString sGameInfo{ " [world=%1]" };
                    if ( !worldInfo.isEmpty() )
                        sGameInfo = sGameInfo.arg( worldInfo );
                    else
                        sGameInfo.clear();

                    QString response{ "#name=%1%2 //Rules: %3 //ID:%4 //TM:%5 //US:%6 //ReMix[ %7 ]" };
                            response = response.arg( serverName )
                                               .arg( sGameInfo )
                                               .arg( Settings::getRuleSet( serverName ) )
                                               .arg( serverID )
                                               .arg( Helper::intToStr( QDateTime::currentDateTimeUtc().toSecsSinceEpoch(),
                                                                       static_cast<int>( IntBase::HEX ), 8 ) )
                                               .arg( this->getUsageString() )
                                               .arg( QString( REMIX_VERSION ) );

                    this->sendUdpDataSlot( ipAddr, port, response );

                    QString msg{ "Recieved ping from User [ %1:%2 ] with SoulID [ %3 ] and BIO data; %4" };
                            msg = msg.arg( ipAddr.toString() )
                                     .arg( port )
                                     .arg( Helper::serNumToIntStr( sernum, true ) )
                                     .arg( data );

                    Settings::insertBioHash( ipAddr, udp.mid( 1 ) );

                    emit this->logBIOSignal( sernum, ipAddr, data );
                    emit this->insertLogSignal( serverName, msg, LogTypes::PING, true, true );
                }
                emit this->increaseServerPingsSignal();
            }
            break;
            case 'M':   //Master Response - Parse information.
            {
                if ( !data.isEmpty() )
                {
                    quint32 pubIP{ 0 };
                    int pubPort{ 0 };
                    int opcode{ 0 };

                    QDataStream mDataStream( udp );
                                mDataStream >> opcode;
                                mDataStream >> pubIP;
                                mDataStream >> pubPort;

                    emit this->recvMasterInfoResponseSignal( ipAddr.toString(), port, QHostAddress( pubIP ).toString(),
                                                             static_cast<quint16>( qFromBigEndian( pubPort ) ) );
                }
            }
            break;
            case 'Q':   //Send Online User Information.
            {
                emit this->sendUserListSignal( ipAddr, port, UserListResponse::Q_Response );
            }
            break;
            case 'R':   //Send Online User Information.
            {
                emit this->sendUserListSignal( ipAddr, port, UserListResponse::R_Response );
            }
            break;
        }
    }
}

QString UdpThread::getUsageString()
{
    return QString( "%1.%2.%3" )
            .arg( usageMins )
            .arg( usageHours )
            .arg( usageDays );
}

//Slot
void UdpThread::sendUdpDataSlot(const QHostAddress& addr, const quint16& port, const QString& data)
{
    if ( socket != nullptr )
    {
        quint64 size{ static_cast<quint64>( data.length() ) };
        socket->writeDatagram( data.toLatin1(), data.size() + 1, addr, port );
        emit this->dataOutSizeSignal( size );
    }
}

void UdpThread::readyReadUDPSlot()
{
    if ( socket == nullptr )
        return;

    QByteArray data;
    QHostAddress senderAddr{ };
    quint16 senderPort{ 0 };

    data.resize( static_cast<int>( socket->pendingDatagramSize() ) );
    socket->readDatagram( data.data(), data.size(), &senderAddr, &senderPort );

    if ( !data.isEmpty() )
        this->parseUdpPacket( data, senderAddr, senderPort );
}

void UdpThread::bindSocketSlot(const QHostAddress& addr, const quint16& port)
{
    if ( socket != nullptr )
    {
        if ( !addr.isNull()
          && port != 0 )
        {
            socket->close();
            while ( true )
            {
                if ( !socket->bind( addr, port ) )
                    continue;
                else
                    break;
            }
        }
    }
}

void UdpThread::closeUdpSocketSlot()
{
    if ( socket != nullptr )
    {
        socket->flush();
        socket->close();
        socket->deleteLater();
    }
    this->disconnect();
    this->exit();
}

void UdpThread::serverUsageChangedSlot(const qint32& minute, const qint32& day, const qint32& hour)
{
    usageDays = day;
    usageHours = hour;
    usageMins = minute;
}

void UdpThread::serverWorldChangedSlot(const QString& newWorld)
{
    worldInfo = newWorld;
}

void UdpThread::serverNameChangedSlot(const QString& newName)
{
    serverName = newName;
}

void UdpThread::serverIDChangedSlot(const QString& id)
{
    serverID = id;
}
