#ifndef MASTERMIXTHREAD_HPP
#define MASTERMIXTHREAD_HPP

#include "prototypes.hpp"

//Required QT Includes.
#include <QMetaObject>
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

    static const QMap<Games, QString> gameNames;
    static QMap<Games, QMetaObject::Connection> connectedGames;
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
        void obtainMasterData(const Games& game);

    public slots:
        void getMasterMixInfoSlot(const Games& game);
        void masterMixInfoChangedSlot();
        void removeConnectedGameSlot(const Games& game);

    public:
        static UdpThread* getNewUdpThread(QObject* parent = nullptr);
        static QString getMasterInfo(const Games& game);
        void parseMasterInfo(const Games& game);

        void run() override;

        static MasterMixThread* getInstance();
        static void setInstance(MasterMixThread* value);
        void updateMasterMixInfo(const bool& forceDownload = false);

        QString getDefaultHost() const;
        QString getModdedHost();

    signals:
        void obtainedMasterMixInfoSignal();
<<<<<<< HEAD
        void masterMixInfoSignal(const Games& game, const QString& ip, const quint16& port, const bool& override );

        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine);
=======
        void masterMixInfoSignal(const Games& game, const QString& ip, const quint16& port);

        void insertLogSignal(const QString& source, const QString& message, const LKeys& type, const bool& logToFile, const bool& newLine);
>>>>>>> develop_unstable
};

#endif // MASTERMIXTHREAD_HPP
