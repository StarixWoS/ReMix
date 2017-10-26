
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

    quint32 sernum_i{ 0 };
    QString sernum_s{ "" };
    QString sernumHex_s{ "" };

    quint32 sceneHost{ 0 };

    quint32 targetHost{ 0 };
    quint32 targetSerNum{ 0 };
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

    bool isInvisible{ false };
    bool networkMuted{ false };

    public:
        explicit Player();
        ~Player();

        void sendMessage(const QString& msg = "", const bool& toAll = false);

        enum Target{ ALL = 0, PLAYER, SCENE = 2 };

        qint64 getConnTime() const;
        void startConnTimer();

        QStandardItem* getTableRow() const;
        void setTableRow(QStandardItem* value);

        QTcpSocket* getSocket() const;
        void setSocket(QTcpSocket* value);

        quint32 getSernum_i() const;
        void setSernum_i(quint32 value);

        QString getSernum_s() const;
        void setSernum_s(const QString& value);

        QString getSernumHex_s() const;
        void setSernumHex_s(const QString& value);

        quint32 getTargetScene() const;
        void setTargetScene(quint32 value);

        quint32 getSceneHost() const;
        void setSceneHost(quint32 value);

        quint32 getTargetSerNum() const;
        void setTargetSerNum(quint32 value);

        int getTargetType() const;
        void setTargetType(const int& value);

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
        void setSlotPos(const int& value);

        QString getPublicIP() const;
        void setPublicIP(const QString& value);

        quint32 getPublicPort() const;
        void setPublicPort(const quint32& value);

        bool getSvrPwdReceived() const;
        void setSvrPwdReceived(const bool& value);

        qint64 getFloodTime() const;
        void restartFloodTimer();

        int getPacketFloodCount() const;
        void setPacketFloodCount(const int& value);

        int getPacketsIn() const;
        void setPacketsIn(const int& value, const int& incr);

        qint64 getBytesIn() const;
        void setBytesIn(const qint64& value);

        int getPacketsOut() const;
        void setPacketsOut(const int& value);

        qint64 getBytesOut() const;
        void setBytesOut(const qint64& value);

        qint64 getAvgBaud(const bool& out) const;
        void setAvgBaud(const qint64& bytes, const bool& out);

        void resetAdminAuth();

        bool getAdminPwdRequested() const;
        void setAdminPwdRequested(const bool& value);

        bool getAdminPwdReceived() const;
        void setAdminPwdReceived(const bool& value);

        bool getIsAdmin() const;
        qint32 getAdminRank() const;

        qint32 getCmdAttempts() const;
        void setCmdAttempts(const qint32& value);

        bool getNewAdminPwdRequested() const;
        void setNewAdminPwdRequested(const bool& value);

        bool getNewAdminPwdReceived() const;
        void setNewAdminPwdReceived(const bool& value);

        //Note: A User will be disconnected on their next update.
        //Usually every 1,000 MS.
        bool getDisconnected() const;
        void setDisconnected(const bool& value,
                             const DCTypes& dcType = DCTypes::IPDC);

        bool getNetworkMuted() const;
        void setNetworkMuted(const bool& value, const QString& msg);

        void validateSerNum(ServerInfo* server, const quint32& id);

        QString getDVar() const;
        void setDVar(const QString& value);

        QString getWVar() const;
        void setWVar(const QString& value);

        ServerInfo* getServerInfo() const;
        void setServerInfo(ServerInfo* value);

        bool getIsInvisible() const;
        void setIsInvisible(const bool& value);

    private:
        void setModelData(QStandardItem* model, const qint32& row,
                          const qint32& column, const QVariant& data,
                          const qint32& role, const bool& isColor = false);

    signals:
            void newAdminPwdRequestedSignal(Player* plr);
            void newRemoteAdminRegisterSignal(Player* plr);
};

#endif // PLAYER_HPP
