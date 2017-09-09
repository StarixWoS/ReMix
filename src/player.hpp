
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QElapsedTimer>
#include <QObject>
#include <QTimer>

class Player : public QObject
{
    Q_OBJECT

    QStandardItem* tableRow{ nullptr };
    ServerInfo* serverInfo{ nullptr };
    SendMsg* messageDialog{ nullptr };
    QTcpSocket* socket{ nullptr };

    QString publicIP{ "" };
    quint32 publicPort{ 0 };

    QString alias{ "" };
    QString playTime{ "" };
    QString dVar{ "" };
    QString wVar{ "" };

    QString bioData;
    QByteArray outBuff;

    bool hasSernum{ false };

    qint32 sernum_i{ 0 };
    QString sernum_s{ "" };
    QString sernumHex_s{ "" };

    qint32 sceneHost{ 0 };

    qint32 targetHost{ 0 };
    qint32 targetSerNum{ 0 };
    int targetType{ 0 };

    bool svrPwdRequested{ false };
    bool svrPwdReceived{ false };
    int slotPos{ -1 };

    bool adminPwdRequested{ false };
    bool adminPwdReceived{ false };
    qint32 adminRank{ -1 };

    qint32 cmdAttempts{ 0 };  //Max limit is 3 attempts before auto-banning.

    bool newAdminPwdRequested{ false };
    bool newAdminPwdReceived{ false };

    QElapsedTimer floodTimer;
    int packetFloodCount{ 0 };

    int packetsIn{ 0 };
    qint64 bytesIn{ 0 };
    qint64 avgBaudIn{ 0 };

    int packetsOut{ 0 };
    qint64 bytesOut{ 0 };
    qint64 avgBaudOut{ 0 };

    QTimer connTimer;
    qint64 connTime{ 0 };
    QElapsedTimer idleTime;

    QTimer killTimer;
    bool pendingDisconnect{ false };

    bool networkMuted{ false };

    public:
        explicit Player();
        ~Player();

        void sendMessage(QString msg = "", bool toAll = false);

        enum Target{ ALL = 0, PLAYER = 1, SCENE = 2 };

        qint64 getConnTime() const;
        void startConnTimer();

        QStandardItem* getTableRow() const;
        void setTableRow(QStandardItem* value);

        QTcpSocket* getSocket() const;
        void setSocket(QTcpSocket* value);

        qint32 getSernum_i() const;
        void setSernum_i(qint32 value);

        QString getSernum_s() const;
        void setSernum_s(const QString& value);

        QString getSernumHex_s() const;
        void setSernumHex_s(const QString& value);

        qint32 getTargetScene() const;
        void setTargetScene(qint32 value);

        qint32 getSceneHost() const;
        void setSceneHost(qint32 value);

        qint32 getTargetSerNum() const;
        void setTargetSerNum(qint32 value);

        int getTargetType() const;
        void setTargetType(int value);

        QString getPlayTime() const;
        void setPlayTime(const QString& value);

        QString getAlias() const;
        void setAlias(const QString& value);

        QString getBioData() const;
        void setBioData(const QByteArray& value);

        QByteArray getOutBuff() const;
        void setOutBuff(const QByteArray& value);

        bool getSvrPwdRequested() const;
        void setSvrPwdRequested(bool value);

        int getSlotPos() const;
        void setSlotPos(int value);

        QString getPublicIP() const;
        void setPublicIP(const QString& value);

        quint32 getPublicPort() const;
        void setPublicPort(const quint32& value);

        bool getSvrPwdReceived() const;
        void setSvrPwdReceived(bool value);

        qint64 getFloodTime() const;
        void restartFloodTimer();

        int getPacketFloodCount() const;
        void setPacketFloodCount(int value);

        int getPacketsIn() const;
        void setPacketsIn(int value, int incr);

        qint64 getBytesIn() const;
        void setBytesIn(const qint64& value);

        int getPacketsOut() const;
        void setPacketsOut(int value);

        qint64 getBytesOut() const;
        void setBytesOut(const qint64& value);

        qint64 getAvgBaud(bool out) const;
        void setAvgBaud(const qint64& bytes, bool out);

        void resetAdminAuth();

        bool getAdminPwdRequested() const;
        void setAdminPwdRequested(bool value);

        bool getAdminPwdReceived() const;
        void setAdminPwdReceived(bool value);

        bool getIsAdmin();
        qint32 getAdminRank();

        qint32 getCmdAttempts() const;
        void setCmdAttempts(const qint32& value);

        bool getNewAdminPwdRequested() const;
        void setNewAdminPwdRequested(bool value);

        bool getNewAdminPwdReceived() const;
        void setNewAdminPwdReceived(bool value);

        //Note: A User will be disconnected on their next update.
        //Usually every 1,000 MS.
        bool getDisconnected() const;
        void setDisconnected(bool value);

        bool getNetworkMuted() const;
        void setNetworkMuted(bool value, QString& msg);

        void validateSerNum(ServerInfo* server, qint32 id);

        QString getDVar() const;
        void setDVar(const QString& value);

        QString getWVar() const;
        void setWVar(const QString& value);

        ServerInfo* getServerInfo() const;
        void setServerInfo(ServerInfo* value);

    signals:
            void newAdminPwdRequestedSignal(Player* plr);
            void newRemoteAdminRegisterSignal(Player* plr);
};

#endif // PLAYER_HPP
