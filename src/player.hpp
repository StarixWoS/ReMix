
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QElapsedTimer>
#include <QTcpSocket>
#include <QObject>
#include <QTimer>
#include <QIcon>

class Player : public QTcpSocket
{
    Q_OBJECT

    QStandardItem* tableRow{ nullptr };
    Server* server{ nullptr };
    User* userUIObject{ nullptr };

    bool newAdminPwdRequested{ false };
    bool newAdminPwdReceived{ false };
    bool adminPwdRequested{ false };
    bool adminPwdReceived{ false };
    bool svrPwdRequested{ false };
    bool svrPwdReceived{ false };
    bool isDisconnected{ false };
    bool isCampLocked{ false };
    bool isCampOptOut{ false };
    bool isVisible{ true };
    bool isAFK{ false };

    QString plrName{ "Unincarnated" };
    QString sernumHex_s{ "" };
    QString sernum_s{ "" };
    QString bioData{ "" };

    QByteArray campPacket{ "" };
    QByteArray outBuff;

    quint64 muteDuration{ 0 };
    quint64 bytesOut{ 0 };
    quint64 bytesIn{ 0 };

    qint32 targetSerNum{ 0 };
    qint32 targetHost{ 0 };
    qint32 sceneHost{ 0 };
    qint32 sernum_i{ 0 };
    qint32 plrLevel{ 0 };

    qint64 plrConnectedTime{ 0 };
    qint64 campCreatedTime{ 0 };
    qint64 maxIdleTime{ 0 };
    qint64 connTime{ 0 };

    qint32 pktHeaderExemptCount{ 0 };
    qint32 pktHeaderSlot{ 0 };
    qint32 cmdAttempts{ 0 };
    qint32 adminRank{ -1 };

    int packetFloodCount{ 0 };
    int packetsOut{ 0 };
    int packetsIn{ 0 };
    int slotPos{ -1 };

    QElapsedTimer floodTimer;
    QElapsedTimer idleTime;

    QTimer killTimer;
    QTimer afkTimer;

    PktTarget targetType{ PktTarget::ALL };

    QIcon afkIcon;

    public:
        explicit Player(qintptr socketDescriptor);
        ~Player() override;

        qint64 getConnTime() const;
        void startConnTimer();

        Server* getServer() const;
        void setServer(Server* value);

        bool getIsVisible() const;
        void setIsVisible(const bool& value);

        bool getHasSernum() const;
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

        PktTarget getTargetType() const;
        void setTargetType(const PktTarget& value);

        QString getPlrName() const;
        void setPlrName(const QString& value);

        QByteArray getCampPacket() const;
        void setCampPacket(const QByteArray& value);

        void forceSendCampPacket();
        QString getBioData() const;
        void setBioData(const QByteArray& value);

        bool getHasBioData() const;

        QByteArray getOutBuff() const;
        void setOutBuff(const QByteArray& value);

        int getSlotPos() const;
        void setSlotPos(const int& value);

        qint32 getPktHeaderSlot() const;
        void setPktHeaderSlot(const qint32& value);

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

        bool getSvrPwdRequested() const;
        void setSvrPwdRequested(bool value);

        bool getSvrPwdReceived() const;
        void setSvrPwdReceived(const bool& value);

        void resetAdminAuth();

        bool getAdminPwdRequested() const;
        void setAdminPwdRequested(const bool& value);

        bool getAdminPwdReceived() const;
        void setAdminPwdReceived(const bool& value);

        bool getNewAdminPwdRequested() const;
        void setNewAdminPwdRequested(const bool& value);

        bool getNewAdminPwdReceived() const;
        void setNewAdminPwdReceived(const bool& value);

        bool getIsAdmin() const;
        qint32 getAdminRank() const;

        qint32 getCmdAttempts() const;
        void setCmdAttempts(const qint32& value);

        bool getIsDisconnected() const;
        void setDisconnected(const bool& value, const DCTypes& dcType = DCTypes::IPDC);

        bool getIsCampLocked() const;
        void setIsCampLocked(bool value);

        bool getIsCampOptOut() const;
        void setIsCampOptOut(bool value);

        quint64 getMuteDuration();
        void setMuteDuration(const quint64& value);
        bool getIsMuted();

        QIcon getAfkIcon() const;
        void setAfkIcon(const QString& value);

        void setIsAFK(bool value);

        void validateSerNum(Server* server, const qint32& id);

        qint64 getPlrConnectedTime() const;
        void setPlrConnectedTime(const qint64& value);

        qint64 getCampCreatedTime() const;
        void setCampCreatedTime(const qint64& value);

        qint64 getMaxIdleTime() const;
        void setMaxIdleTime(const qint64& value);

        qint32 getPlrLevel() const;
        void setPlrLevel(const qint32& value);

        qint32 getPktHeaderExemptCount() const;
        void setPktHeaderExemptCount(const qint32& value);

        QString getIPAddress() const;

    public slots:
        void sendPacketToPlayerSlot(Player* plr, qint32 targetType, qint32 trgSerNum, qint32 trgScene, const QByteArray& packet);
        void sendMasterMsgToPlayerSlot(Player* plr, const bool& all, const QByteArray& packet);
        void setMaxIdleTimeSlot(const qint64& maxAFK);
        void connectionTimeUpdateSlot();

    private slots:
        void readyReadSlot();

    signals:
        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine) const;
        void parsePacketSignal(const QByteArray& packet, Player* plr);
        void hexSerNumSetSignal(Player* plr);
        void updatePlrViewSignal(Player* plr, const qint32& column, const QVariant& data, const qint32& role, const bool& isColor = false);
};

#endif // PLAYER_HPP
