
#ifndef SERVER_HPP
#define SERVER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QTcpServer>

class Server : public QTcpServer
{
    Q_OBJECT

    QWidget* mother{ nullptr };

    public:
        Server(QWidget* parent = nullptr);
        ~Server() override;

        void incomingConnection(qintptr socketDescriptor) override;

    signals:
        void plrConnectedSignal(qintptr socketDescriptor);
};

#endif // SERVER_HPP
