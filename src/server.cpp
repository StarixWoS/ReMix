
//Class includes.
#include "server.hpp"

//Qt Includes.
#include <QTcpServer>
#include <QWidget>
#include <QtCore>

Server::Server(QWidget* parent)
    : QTcpServer(parent)
{
    //Setup Objects.
    mother = parent;

    //Ensure all possible User slots are fillable.
    this->setMaxPendingConnections( MAX_PLAYERS );
}

Server::~Server()
{
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    emit this->plrConnectedSignal( socketDescriptor );
}
