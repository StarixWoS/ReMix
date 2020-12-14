#ifndef MASTERMIXTHREAD_HPP
#define MASTERMIXTHREAD_HPP

#include "prototypes.hpp"

//Required QT Includes.
#include <QUdpSocket>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QTimer>

class MasterMixThread : public QThread
{
    Q_OBJECT

    QString defaultHost{ "http://synthetic-reality.com/synreal.ini" };
    QTimer updateInfoTimer;

    static QSettings* masterMixPref;
    static QTcpSocket* tcpSocket;
    static bool download;
    static QMutex mutex;

    private:
        MasterMixThread();
        ~MasterMixThread() override;

        MasterMixThread(const MasterMixThread*) = delete;
        void operator=(const MasterMixThread* x) = delete;

        void connectSlots();
        void startUpdateInfoTimer(const bool& start);

    private slots:
        void obtainMasterData(ServerInfo* server);

    public slots:
        void masterMixInfoChangedSlot();

    public:
        static UdpThread* getNewUdpThread(QObject* parent = nullptr);
        void run() override;

        static MasterMixThread* getInstance();
        static void setInstance(MasterMixThread* value);
        void getMasterMixInfo(ServerInfo* server);
        void updateMasterMixInfo(const bool& forceDownload = false);

        QString getDefaultHost() const;
        QString getModdedHost();

    signals:
        void masterMixInfoSignal();
        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine);
};

#endif // MASTERMIXTHREAD_HPP
