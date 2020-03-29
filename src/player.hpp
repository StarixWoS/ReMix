
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QElapsedTimer>
#include <QObject>
#include <QTimer>
#include <QIcon>

class Player : public QObject
{
    Q_OBJECT

    QStandardItem* tableRow{ nullptr };
    ServerInfo* serverInfo{ nullptr };
    User* userUIObject{ nullptr };
    SendMsg* messageDialog{ nullptr };
    QTcpSocket* socket{ nullptr };
    QByteArray outBuff;

    QString publicIP{ "" };
    quint32 publicPort{ 0 };

    QString plrName{ "Unincarnated" };
    QString alias{ "" };
    QString playTime{ "" };
    QString dVar{ "" };
    QString wVar{ "" };

    QByteArray campPacket{ "" };
    bool sentCampPacket{ false };

    QString bioData{ "" };
    bool hasBioData{ false };

    bool hasSernum{ false };
    quint32 sernum_i{ 0 };
    QString sernum_s{ "" };
    QString sernumHex_s{ "" };

    quint32 sceneHost{ 0 };

    quint32 targetHost{ 0 };
    quint32 targetSerNum{ 0 };
    PktTarget targetType{ PktTarget::ALL };

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
    quint64 bytesIn{ 0 };
    quint64 avgBaudIn{ 0 };

    int packetsOut{ 0 };
    quint64 bytesOut{ 0 };
    quint64 avgBaudOut{ 0 };

    QTimer connTimer;
    quint64 connTime{ 0 };
    QElapsedTimer idleTime;

    QTimer killTimer;

    bool isDisconnected{ false };
    bool isVisible{ true };
    bool isMuted{ false };

    quint64 muteDuration{ 0 };

    QTimer afkTimer;
    QIcon afkIcon;
    bool isAFK{ false };

    public:
        explicit Player();
        ~Player() override;

        void sendMessage(const QString& msg = "", const bool& toAll = false);

        quint64 getConnTime() const;
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

        PktTarget getTargetType() const;
        void setTargetType(const PktTarget& value);

        QString getPlayTime() const;
        void setPlayTime(const QString& value);

        QString getPlrName() const;
        void setPlrName(const QString& value);

        QString getAlias() const;
        void setAlias(const QString& value);

        QString getBioData() const;
        void setBioData(const QByteArray& value);

        bool getHasBioData() const;
        void setHasBioData(bool value);

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

        quint64 getBytesIn() const;
        void setBytesIn(const quint64 &value);

        int getPacketsOut() const;
        void setPacketsOut(const int& value);

        quint64 getBytesOut() const;
        void setBytesOut(const quint64 &value);

        quint64 getAvgBaud(const bool& out) const;
        void setAvgBaud(const quint64 &bytes, const bool& out);

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
        //Usually every 250 MS or as defined by MAX_DISCONNECT_TTL.
        bool getIsDisconnected() const;
        void setDisconnected(const bool& value,
                             const DCTypes& dcType = DCTypes::IPDC);

        quint64 getMuteDuration();
        void setMuteDuration(const quint64& value);
        bool getIsMuted();
        void setIsMuted(const quint64& duration);

        void chatPacketFound();

        QIcon getAfkIcon() const;
        void setAfkIcon(const QString& value);

        bool getIsAFK() const;
        void setIsAFK(bool value);

        void validateSerNum(ServerInfo* server, const quint32& id);

        QString getDVar() const;
        void setDVar(const QString& value);

        QString getWVar() const;
        void setWVar(const QString& value);

        ServerInfo* getServerInfo() const;
        void setServerInfo(ServerInfo* value);

        bool getIsVisible() const;
        void setIsVisible(const bool& value);

        bool getHasSernum() const;
        void setHasSernum(bool value);

        QByteArray getCampPacket() const;
        void setCampPacket(const QByteArray& value);

        bool getSentCampPacket() const;
        void setSentCampPacket(bool value);

        void forceSendCampPacket();

    private:
        void setModelData(QStandardItem* model, const qint32& row, const qint32& column, const QVariant& data,
                          const qint32& role, const bool& isColor = false);

    public slots:
        void sendPacketToPlayerSlot(Player* plr, QTcpSocket* srcSocket, qint32 targetType, quint32 trgSerNum,
                                    quint32 trgScene, const QByteArray& packet);
};

#endif // PLAYER_HPP
