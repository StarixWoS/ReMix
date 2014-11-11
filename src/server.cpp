
#include "server.hpp"

#include "preferences.hpp"
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
                      this, &Server::masterCheckInTimeOutSlot );
}

bool Server::getIsSetUp() const
{
    return isSetUp;
}

void Server::setIsSetUp(bool value)
{
    isSetUp = value;
}

void Server::setupServerInfo(ServerInfo* svrInfo)
{
    if ( !isSetUp )
        this->setIsSetUp( true );

    serverInfo = svrInfo;

    QString ipAddr = QHostAddress( QHostAddress::LocalHost ).toString();
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();

    QString tmp;
    for ( int i = 0; i < ipList.size(); ++i )
    {
        tmp = ipList.at( i ).toString();
        if ( ipList.at( i ) != QHostAddress::LocalHost
          && ipList.at( i ).toIPv4Address()
          && !Preferences::isInvalidIPAddress( tmp ) )
        {
            ipAddr = ipList.at(i).toString();   //Use first non-local IP address.
            break;
        }
    }
    serverInfo->privateIP = ipAddr;

    //Bind to our UDP socket using our non-local IP and desired Port.
    masterSocket->bind( QHostAddress( ipAddr ), svrInfo->privatePort );

    //Begin listening for TCP connections using our non-local IP and desired Port.
    this->listen( QHostAddress( ipAddr ), svrInfo->privatePort );

    if ( isPublic && this->getIsSetUp() )
        this->masterCheckInTimeOutSlot();
}

void Server::newConnectionSlot()
{
    QTcpSocket* peer = this->nextPendingConnection();

    QHostAddress peerAddr = peer->localAddress();
    if ( !tcpSockets.contains( peerAddr ) )
        tcpSockets.insert( peer->localAddress(), peer );
    else
    {
        ;   //TODO: Check if Dupe Connections are Allowed or Bannable.
    }

    //Connect the Pending TCP Connection to a ReadyRead lambda.
    QObject::connect( peer, &QTcpSocket::readyRead, [peer, this]()
    {
        if ( !tcpBuffer.trimmed().isEmpty()
          && peer != nullptr )
        {
            tcpBuffer.append( peer->readAll() );
            if ( tcpBuffer.contains( "\r" )
              || tcpBuffer.contains( "\n" ) )
            {
                int bytes = tcpBuffer.indexOf( "\r\n" );
                if ( bytes <= 0 )
                    bytes = tcpBuffer.indexOf( "\n" );
                if ( bytes <= 0 )
                    bytes = tcpBuffer.indexOf( "\r" );

                if ( bytes > 0 )
                {
                    QString packet = tcpBuffer.left( bytes + 1 ).trimmed();
                    tcpBuffer = tcpBuffer.mid( bytes + 1 ).data();

                    this->parsePacket( packet );
                    emit peer->readyRead();
                }
            }
        }
    });

    //Connect the Pending TCP Connection to a Disconnected lambda.
    QObject::connect( peer, &QTcpSocket::disconnected, [peer, this]()
    {
        if ( peer != nullptr )
        {
            tcpSockets.remove( peer->localAddress() );
            peer->deleteLater();
        }
    });
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
                    serverInfo->gameInfo = data.mid( data.indexOf( "GWorld=", Qt::CaseInsensitive ) + 8 );
                break;
                case 'M':
                    this->parseMasterServerResponse( udpData );
                break;
                case 'P':
                    //Player information is stored for later access.
                    qDebug() << "Updating PlayerInfo for address: " << senderAddr;
                    udpDatas.insert( senderAddr, udpData );

                    if ( !serverInfo->gameInfo.isEmpty() )
                    {
                        response = QString( "#name=%1 [world=%2] //Rules: %3 //ID:%4 //TM:%5 //US:%6" )
                                       .arg( serverInfo->name )
                                       .arg( serverInfo->gameInfo )
                                       .arg( serverInfo->serverRules )
                                       .arg( QString::number( serverInfo->serverID, 16 ).toUpper(), 8, QChar( '0' ) )
                                       .arg( QString::number( QDateTime::currentDateTime().toTime_t(), 16 ).toUpper(), 8, QChar( '0' ) )
                                       .arg( "999.999.999" );
                    }
                    else
                    {
                        response = QString( "#name=%1 //Rules: %2 //ID:%3 //TM:%4 //US:%5" )
                                        .arg( serverInfo->name )
                                        .arg( serverInfo->serverRules )
                                        .arg( QString::number( serverInfo->serverID, 16 ).toUpper(), 8, QChar( '0' ) )
                                        .arg( QString::number( QDateTime::currentDateTime().toTime_t(), 16 ).toUpper(), 8, QChar( '0' ) )
                                        .arg( "999.999.999" );
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
            this->masterCheckInTimeOutSlot();
        }
        else
        {
            masterCheckIn.stop();
            this->disconnectFromMaster();
        }
        isPublic = value;
    }
}

void Server::disconnectFromMaster()
{
    if ( this->getIsSetUp() )
    {
        char ex = 'X';
        masterSocket->writeDatagram( &ex, 2,
                                     QHostAddress( serverInfo->masterIP ), serverInfo->masterPort );
    }
}

void Server::masterCheckInTimeOutSlot()
{
    if ( this->getIsSetUp() )
    {
        QString response = QString( "!version=%1,nump=%2,gameid=%3,game=%4,host=%5,id=%6,port=%7,info=%8,name=%9" )
                           .arg( serverInfo->versionID_i )
                           .arg( serverInfo->playerCount )
                           .arg( serverInfo->gameId )
                           .arg( serverInfo->gameName )
                           .arg( serverInfo->hostInfo.localHostName() )
                           .arg( serverInfo->serverID )
                           .arg( serverInfo->privatePort )
                           .arg( "" )  //Unknown. What constitutes as info?
                           .arg( serverInfo->name );

        masterSocket->writeDatagram( response.toLatin1(), response.length() + 1,
                                     QHostAddress( serverInfo->masterIP ), serverInfo->masterPort );
    }
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
    serverInfo->publicIP = QHostAddress( pubIP ).toString();
    serverInfo->publicPort = qFromBigEndian( pubPort );
}

void Server::parsePacket(QString& packet)
{
    if ( packet.startsWith( ":SR", Qt::CaseInsensitive ) )
    {
        //Prevent Users from Impersonating the Server Admin.
        if ( packet.startsWith( ":SR@", Qt::CaseInsensitive ) )
            return;

        //Prevent Users from changing the Server's rules. ( However temporary the effect may be. )
        if ( packet.startsWith( ":SR$", Qt::CaseInsensitive ) )
            return;

        this->parseSRPacket(packet);
    }

    if ( packet.startsWith( ":MIX", Qt::CaseInsensitive ) )
        this->parseMIXPacket(packet);
}

void Server::parseMIXPacket(QString& packet)
{
    QChar opCode = packet.at( 4 );
    QString tmp = packet;

    switch ( opCode.toLatin1() )
    {
        case '1':
            //Send Packet to Scene hosted by DestSlot.
        break;
        case '2':
            //Unknown Function.
        break;
        case '3':
            //Unknown Function.
        break;
        case '4':
            //Send the next packet from SourceSlot to DestSlot as defined within the packet.
        break;
        case '5':
            //Print remoteUser comment.
        break;
        case '6':
            //Respond to a remoteAdmin command.
        break;
        case '7':
            //Slot SerNum.
        break;
        case '8':
            //Set/Read SSV Variable.
        break;
        case '9':
            //Set/Read SSV Variable.
        break;
        default:
            return;
        break;
    }
}

void Server::parseSRPacket(QString& packet)
{

}
