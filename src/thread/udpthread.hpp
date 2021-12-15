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
    Games gameID{ Games::Invalid };

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

        void sendUserListSignal(const QHostAddress& addr, const quint16& port, const UserListResponse& type);
        void dataOutSizeSignal(const quint64& size);
        void dataInSizeSignal(const quint64& size);
        void increaseServerPingsSignal();
        void insertLogSignal(const QString& source, const QString& message, const LKeys& type, const bool& logToFile, const bool& newLine) const;

        void setBytesInSignal(const quint64& bytes);
        void recvMasterInfoResponseSignal(const QString& masterIP, const quint16& masterPort, const QString& userIP, const quint16& userPort);
        void recvPlayerGameInfoSignal(const QString& info, const QString& ip);
        void logBIOSignal(const QString& serNum, const QHostAddress& ip, const QString& bio);

    public slots:
        void sendUdpDataSlot(const QHostAddress& addr, const quint16& port, const QString& data);
        void readyReadUDPSlot();
        void bindSocketSlot(const QHostAddress& addr, const quint16& port);
        void closeUdpSocketSlot();

        void serverUsageChangedSlot(const qint32& minute, const qint32& day, const qint32& hour);
        void serverWorldChangedSlot(const QString& newWorld);
        void serverNameChangedSlot(const QString& newName);
        void serverIDChangedSlot(const QString& id);
        void serverGameChangedSlot(const Games& game);
};

#endif // UDPTHREAD_HPP
