#ifndef UDPTHREAD_HPP
#define UDPTHREAD_HPP

#include "prototypes.hpp"

//Required QT Includes.
#include <QUdpSocket>
#include <QThread>

class UdpThread : public QThread
{
    Q_OBJECT

    QUdpSocket* socket{ nullptr };

    qint32 usageDays{ 0 };
    qint32 usageHours{ 0 };
    qint32 usageMins{ 0 };

    QString serverName{ "" };
    QString serverID{ "" };
    QString worldInfo{ "" };

    public:
        UdpThread(QObject *parent = nullptr);
        ~UdpThread() override;

        static UdpThread* getNewUdpThread(QObject* parent = nullptr);
        void run() override;

        void parseUdpPacket(const QByteArray& udp, const QHostAddress& ipAddr, const quint16& port);

        QString getUsageString();

    signals:
        void error(QUdpSocket::SocketError socketError);
        void udpDataSignal(const QByteArray& udp, const QHostAddress& ipAddr, const quint16& port);

        void sendUserListSignal(const QHostAddress& addr, const quint16& port, const UserListResponse& type);
        void dataOutSizeSignal(const quint64& size);
        void dataInSizeSignal(const quint64& size);
        void increaseServerPingsSignal();
        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine) const;

    public slots:
        void sendUdpDataSlot(const QHostAddress& addr, const quint16& port, const QString& data);
        void readyReadUDPSlot();
        void bindSocketSlot(const QHostAddress& addr, const quint16& port);
        void closeUdpSocketSlot();

        void serverUsageChangedSlot(const qint32& minute, const qint32& day, const qint32& hour);
        void serverWorldChangedSlot(const QString& newWorld);
        void serverNameChangedSlot(const QString& newName);
        void serverIDChangedSlot(const QString& id);
};

#endif // UDPTHREAD_HPP
