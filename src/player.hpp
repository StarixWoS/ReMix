
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSharedPointer>
#include <QElapsedTimer>
#include <QTcpSocket>
#include <QObject>
#include <QTimer>
#include <QIcon>

class Player : public QTcpSocket
{
    Q_OBJECT

    QList<QMetaObject::Connection> slotConnections;
    QSharedPointer<Server> server;
    User* userUIObject{ nullptr };

    bool isQuarantineOverride{ false };
    bool newAdminPwdRequested{ false };
    bool newAdminPwdReceived{ false };
    bool adminPwdRequested{ false };
    bool adminPwdReceived{ false };
    bool svrPwdRequested{ false };
    bool svrPwdReceived{ false };
    bool isDisconnected{ false };
    bool isQuarantined{ false };
    bool isPartyLocked{ false };
    bool isCampLocked{ false };
    bool isCampOptOut{ false };
    bool isIncarnated{ false };
    bool isGhosting{ false };
    bool isVisible{ true };
    bool isAFK{ false };
    bool isPK{ false };

    QString plrName{ "Unincarnated" };
    QString sernumHex_s{ "00000000" };
    QString sernum_s{ "SOUL 0" };
    QString bioData{ "" };

    QByteArray campPacket{ "" };
    QByteArray outBuff;

    quint64 muteDuration{ 0 };
    quint64 bytesOut{ 0 };
    quint64 bytesIn{ 0 };

    qint32 targetSerNum{ 0 };
    qint32 targetHost{ 0 };
    qint32 sceneHost{ 0 };

    bool hasSerNum{ false };
    qint32 sernum_i{ 0 };
    qint32 plrLevel{ 0 };

    qint32 plrCheatCount{ 0 };
    qint32 plrModCount{ 0 };

    qint64 plrConnectedTime{ 0 };
    qint64 campCreatedTime{ 0 };
    qint64 connTime{ 0 };

    qint32 pktHeaderExemptCount{ 0 };
    qint32 pktHeaderSlot{ 0 };
    qint32 cmdAttempts{ 0 };

    bool isAdmin{ false };
    GMRanks adminRank{ GMRanks::User };

    int packetFloodCount{ 0 };
    int packetsOut{ 0 };
    int packetsIn{ 0 };
    int slotPos{ -1 };

    QElapsedTimer floodTimer;
    QElapsedTimer idleTime;

    QTimer vanishStateTimer;
    QTimer serNumKillTimer;
    QTimer killTimer;
    QTimer afkTimer;

    PktTarget targetType{ PktTarget::ALL };
    QSharedPointer<Player> thisPlayer;

    public:
        explicit Player(qintptr socketDescriptor, QSharedPointer<Server> svr);
        ~Player() override;
        static void customDeconstruct(Player* plr);

        static QSharedPointer<Player> createPlayer(qintptr socketDescriptor, QSharedPointer<Server> svr);
        void setThisPlayer(QSharedPointer<Player> plr);
        QSharedPointer<Player> getThisPlayer();
        void clearThisPlayer();

        qint64 getConnTime() const;
        void startConnTimer();

        QSharedPointer<Server> getServer();
        void clearServer();

        bool getIsVisible() const;
        void setIsVisible(const bool& value);

        bool getHasSerNum() const;
        void setHasSerNum( const bool& value);

        qint32 getSernum_i() const;
        void setSernum_i(const qint32& value);

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

        QByteArray& getCampPacket();
        void setCampPacket(const QByteArray& value);

        void forceSendCampPacket();
        QString getBioData() const;
        void setBioData(const QByteArray& value);

        bool getHasBioData() const;

        QByteArray getOutBuff() const;
        void setOutBuff(const QByteArray& value);

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
        void setIsAdmin(const bool& value);

        GMRanks getAdminRank() const;
        void setAdminRank(const GMRanks& value);

        qint32 getCmdAttempts() const;
        void setCmdAttempts(const qint32& value);

        bool getIsDisconnected() const;
        void setDisconnected(const bool& value, const DCTypes& dcType = DCTypes::IPDC);

        bool getQuarantineOverride() const;
        void setQuarantineOverride(const bool& value);

        bool getIsQuarantined() const;
        void setQuarantined(const bool& value);

        bool getIsCampLocked() const;
        void setIsCampLocked(bool value);

        bool getIsCampOptOut() const;
        void setIsCampOptOut(bool value);

        quint64 getMuteDuration();
        void setMuteDuration(const quint64& value);
        bool getIsMuted();

        void updateIconState();

        bool getIsAFK();
        void setIsAFK(const bool& value);

        bool getIsPK();
        void setIsPK(const bool& value);

        void validateSerNum(const qint32& id);
        bool getIsGoldenSerNum();

        qint64 getPlrConnectedTime() const;
        void setPlrConnectedTime(const qint64& value);

        qint64 getCampCreatedTime() const;
        void setCampCreatedTime(const qint64& value);

        qint32 getPlrLevel() const;
        void setPlrLevel(const qint32& value);

        qint32 getPlrCheatCount() const;
        void setPlrCheatCount(const qint32& value);

        qint32 getPlrModCount() const;
        void setPlrModCount(const qint32& value);

        qint32 getIsPartyLocked() const;
        void setIsPartyLocked(const qint32& value);

        qint32 getPktHeaderExemptCount() const;
        void setPktHeaderExemptCount(const qint32& value);

        QString getIPAddress() const;
        QString getIPPortAddress() const;

        bool getIsIncarnated() const;
        void setIsIncarnated(bool newIsIncarnated);

        bool getIsGhosting() const;
        void setIsGhosting(bool newIsGhosting);

    public slots:
        void vanishStateTimerTimeOutSlot();
        void sendPacketToPlayerSlot(QSharedPointer<Player> plr, const qint32& targetType, const qint32& trgSerNum,
                                    const qint32& trgScene, const QByteArray& packet);
        void sendMasterMsgToPlayerSlot(const QSharedPointer<Player> plr, const bool& all, const QByteArray& packet);
        void refreshAFKTimersSlot(const qint64& maxAFK);
        void connectionTimeUpdateSlot();
        void setAdminRankSlot(const QString& hexSerNum, const GMRanks& rank);

    private slots:
        void mutedSerNumDurationSlot(const QString& sernum, const quint64& duration, const QString& reason);
        void readyReadSlot();

        void serNumKillTimerTimeOutSlot();
        void killTimerTimeOutSlot();
        void afkTimerTimeOutSlot();

    signals:
        void insertLogSignal(const QString& source, const QString& message, const LKeys& type, const bool& logToFile, const bool& newLine) const;
        void parsePacketSignal(const QByteArray& packet, QSharedPointer<Player> plr);
        void hexSerNumSetSignal(QSharedPointer<Player> plr);
        void updatePlrViewSignal(QSharedPointer<Player> plr, const qint32& column, const QVariant& data, const qint32& role, const bool& isColor = false);
        void ipDCIncreaseSignal(const DCTypes& type);
        void setVisibleStateSignal(const bool& state);
};

#endif // PLAYER_HPP
