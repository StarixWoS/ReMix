
#ifndef SERVER_HPP
#define SERVER_HPP

#include <QNetworkInterface>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QString>
#include <QDebug>
#include <QTimer>

class Server : public QTcpServer
{
    Q_OBJECT

    QHash<QHostAddress, QTcpSocket*> tcpSockets;
    QHash<QHostAddress, QByteArray> udpDatas;

    QUdpSocket* masterSocket{ nullptr };
    QTimer masterCheckIn;

    public:
        explicit Server(QObject *parent = 0);

        void setupServerInfo(QString& port);

    signals:

    private slots:
        void masterCheckInTimeoutSlot();
        void newConnectionSlot();
        void readyReadSlot();
};

#endif // SERVER_HPP
