
//Class includes.
#include "thread/udpthread.hpp"

//ReMix Includes
#include "packethandler.hpp"
#include "serverinfo.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "player.hpp"
#include "server.hpp"
#include "user.hpp"

//Qt Includes.
#include <QDateTime>
#include <QtCore>
#include <QDebug>

UdpThread::UdpThread(QObject *parent)
    : QThread(parent)
{
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

    qint32 index{ 0 };
    if ( !data.isEmpty() )
    {
        QChar opCode{ data.at( 0 ).toLatin1() };
        switch ( opCode.toLatin1() )
        {
            case 'P':   //Store the Player information into a struct.
            {
                index = Helper::getStrIndex( data, "sernum=" );
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
                    QString sGameInfo{ worldInfo };
                    if ( !sGameInfo.isEmpty() )
                        sGameInfo = " [world=" % worldInfo % "]";

                    QString response{ "#name=%1%2 //Rules: %3 //ID:%4 //TM:%5 //US:%6 //ReMix[ %7 ]" };
                            response = response.arg( serverName )
                                               .arg( sGameInfo )
                                               .arg( Settings::getRuleSet( serverName ) )
                                               .arg( serverID )
                                               .arg( Helper::intToStr( QDateTime::currentDateTimeUtc().toTime_t(), static_cast<int>( IntBase::HEX ), 8 ) )
                                               .arg( this->getUsageString() )
                                               .arg( QString( REMIX_VERSION ) );

                    this->sendUdpDataSlot( ipAddr, port, response );
                    emit this->sendServerInfoSignal( ipAddr, port );
                }
                emit this->increaseServerPingsSignal();
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
        socket->writeDatagram( data.toLatin1(), data.size() + 1, addr, port );
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
    {
        emit this->udpDataSignal( data, senderAddr, senderPort );
        if ( data.at( 0 ) == 'P'
          || data.at( 0 ) == 'Q'
          || data.at( 0 ) == 'R' )
        {
            //Time-critical packet types will be handled directly by the UdpThread class.
            this->parseUdpPacket( data, senderAddr, senderPort );
        }
    }
}

void UdpThread::bindSocketSlot(const QHostAddress& addr, const quint16& port)
{
    if ( socket != nullptr )
    {
        if ( !addr.isNull()
          && port != 0 )
        {
            socket->close();
            socket->bind( addr, port );
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
