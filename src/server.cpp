
#include "server.hpp"

#include "serverinfo.hpp"

Server::Server(QObject*)
{
    //Setup Objects.
    masterCheckIn.setInterval( 300000 );    //Every 5 Minutes.

    masterSocket = new QUdpSocket( this );

    //Connect Objects.
    QObject::connect( this, &QTcpServer::newConnection,
                      this, &Server::newConnectionSlot );

    QObject::connect( masterSocket, &QUdpSocket::readyRead,
                      this, &Server::readyReadUDPSlot );

    QObject::connect( &masterCheckIn, &QTimer::timeout,
                      this, &Server::masterCheckInTimeoutSlot );
}

void Server::setupServerInfo(ServerInfo* svrInfo)
{
    serverInfo = svrInfo;

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

    //Bind to our UDP socket using our non-local IP and desired Port.
    masterSocket->bind( QHostAddress( ipAddr ), svrInfo->serverPort );

    //Begin listening for TCP connections using our non-local IP and desired Port.
    this->listen( QHostAddress( ipAddr ), svrInfo->serverPort );
}

void Server::newConnectionSlot()
{
    QObject* sender = QObject::sender();
    if ( sender != nullptr )
        qDebug() << "\nnewConnectionSlot" << sender;

    QTcpSocket* peer = this->nextPendingConnection();
    tcpSockets.insert( peer->localAddress(), peer );

    QObject::connect( peer, &QTcpSocket::readyRead,
                      this, &Server::readyReadTCPSlot );
}

void Server::readyReadTCPSlot()
{
    qDebug() << "\nreadyReadTCPSlot";
    QTcpSocket* sender = static_cast<QTcpSocket*>( QObject::sender() );
    if ( sender != nullptr )
        qDebug() << sender->readAll();
}

void Server::readyReadUDPSlot()
{
    QUdpSocket* sender = static_cast<QUdpSocket*>( QObject::sender() );
    if ( sender != nullptr )
    {
        QHostAddress senderAddr;
        quint16 senderPort = 0;
        udpData.resize( sender->pendingDatagramSize() );
        sender->readDatagram( udpData.data(), udpData.size(), &senderAddr, &senderPort );

        QString response;
        QString data( udpData );
        if ( !data.isEmpty() )
        {
            switch ( data.at( 0 ).toLatin1() )
            {
                case 'G':
                    serverInfo->world = data.mid( data.indexOf( "GWorld=", Qt::CaseInsensitive ) + 8 );
                break;
                case 'M':
                    qDebug() << senderAddr << "Parse data from the MasterMix relating to our external IP.";
                    this->parseMasterServerResponse( udpData );
                break;
                case 'P':
                    //Player information is stored for later access.
                    qDebug() << "Updating PlayerInfo for address: " << senderAddr;
                    udpDatas.insert( senderAddr, udpData );

                    if ( !serverInfo->world.isEmpty() )
                    {
                        response = QString( "#name=%1 [world=%2] //Rules: %3 //ID:%4 //TM:%5 //US:%6" )
                                       .arg( serverInfo->serverName )
                                       .arg( serverInfo->world )
                                       .arg( serverInfo->serverRules )
                                       .arg( QString::number( serverInfo->serverID, 16 ).toUpper(), 8, QChar( '0' ) )
                                       .arg( QString::number( QDateTime().toTime_t(), 16 ).toUpper(), 8, QChar( '0' ) )
                                       .arg( "1.1.26" );
                    }
                    else
                    {
                        response = QString( "#name=%1 //Rules: %2 //ID:%3 //TM:%4 //US:%5" )
                                        .arg( serverInfo->serverName )
                                        .arg( serverInfo->serverRules )
                                        .arg( QString::number( serverInfo->serverID, 16 ).toUpper(), 8, QChar( '0' ) )
                                        .arg( QString::number( QDateTime().toTime_t(), 16 ).toUpper(), 8, QChar( '0' ) )
                                        .arg( "1.1.26" );
                    }
                    qDebug() << response;
                    sender->writeDatagram( response.toLatin1(), response.size() + 1, senderAddr, senderPort );
                break;
                case 'Q':
                    qDebug() << senderAddr << "Send our online User information to the requestor.";
                break;
                case 'R':
                    qDebug() << senderAddr << "Command \"R\" with unknown use";
                break;
                default:
                    qDebug() << senderAddr << "Unknown command!";
                break;
            }
        }
    }
}

void Server::setupPublicServer(bool value)
{
    if ( value != isPublic )
    {
        if ( !isPublic )
        {
            masterCheckIn.start();
            this->masterCheckInTimeoutSlot();
        }
        else
        {
            masterCheckIn.stop();

            //TODO: Mode Disconnecting from the MasterMix into it's own function/slot.
            char ex = 'X';
            masterSocket->writeDatagram( &ex, 2, QHostAddress( "63.197.64.78" ), 23999 );
        }
        isPublic = value;
    }
}

void Server::masterCheckInTimeoutSlot()
{
    QString response = QString( "!version=%1,nump=%2,gameid=%3,game=%4,host=%5,id=%6,port=%7,info=%8,name=%9" )
                            .arg( serverInfo->versionID_i )
                            .arg( serverInfo->playerCount )
                            .arg( serverInfo->gameId )
                            .arg( serverInfo->gameName )
                            .arg( serverInfo->hostInfo.localHostName() )
                            .arg( serverInfo->serverID )
                            .arg( serverInfo->serverPort )
                            .arg( "" )  //Unknown. What constitutes as info?
                            .arg( serverInfo->serverName );

    masterSocket->writeDatagram( response.toLatin1(), response.length() + 1, QHostAddress( "63.197.64.78" ), 23999 );
}

void Server::parseMasterServerResponse(QByteArray& mData)
{
    int opcode = 0;
    int pubIP = 0;
    int pubPort = 0;

    if ( !mData.isEmpty() )
    {
        QDataStream mDataStream( mData );
                    mDataStream >> opcode;
                    mDataStream >> pubIP;
                    mDataStream >> pubPort;
    }
    qDebug() << QHostAddress( pubIP ).toString() << qFromBigEndian( pubPort );
}
