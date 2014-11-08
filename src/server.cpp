#include "server.hpp"

Server::Server(QObject*)
{
    //Setup Objects.
    masterCheckIn.setInterval( 300000 );    //Every 5 Minutes.

    masterSocket = new QUdpSocket( this );
    masterSocket->bind();

    //Connect Objects.
    QObject::connect( this, &QTcpServer::newConnection,
                      this, &Server::newConnectionSlot );

    QObject::connect( &masterCheckIn, &QTimer::timeout,
                      this, &Server::masterCheckInTimeoutSlot );
}

void Server::setupServerInfo(QString& port)
{
    QString ipAddr = QHostAddress( QHostAddress::LocalHost ).toString();
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
    for ( int i = 0; i < ipList.size(); ++i )
    {
        if ( ipList.at(i) != QHostAddress::LocalHost
          && ipList.at(i).toIPv4Address())
        {
            ipAddr = ipList.at(i).toString();   //Use first non-local IP address.
            break;
        }
    }
    this->listen( QHostAddress( ipAddr ), port.toInt() );
}

void Server::newConnectionSlot()
{
    QObject* sender = QObject::sender();
    if ( sender != nullptr )
        qDebug() << "newConnectionSlot" << sender;

    QTcpSocket* peer = this->nextPendingConnection();
    tcpSockets.insert( peer->localAddress(), peer );

    QObject::connect( peer, &QTcpSocket::readyRead,
                      this, &Server::readyReadSlot );
}

void Server::readyReadSlot()
{
   QObject* sender = QObject::sender();
   if ( sender != nullptr )
       qDebug() << "readyReadSlot" << sender;
}

void Server::masterCheckInTimeoutSlot()
{
    ;   //Send checkin data to the Master Mix.
}
