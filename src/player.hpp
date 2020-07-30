
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
    ServerInfo* serverInfo{ nullptr };
    User* userUIObject{ nullptr };

    bool newAdminPwdRequested{ false };
    bool newAdminPwdReceived{ false };
    bool adminPwdRequested{ false };
    bool adminPwdReceived{ false };
    bool svrPwdRequested{ false };
    bool sentCampPacket{ false };
    bool svrPwdReceived{ false };
    bool isDisconnected{ false };
    bool isVisible{ true };
    bool isAFK{ false };

    QString plrName{ "Unincarnated" };
    QString sernumHex_s{ "" };
    QString sernum_s{ "" };
    QString playTime{ "" };
    QString bioData{ "" };
    QString alias{ "" };

    QByteArray campPacket{ "" };
    QByteArray outBuff;

    quint64 muteDuration{ 0 };
    quint64 avgBaudOut{ 0 };
    quint64 avgBaudIn{ 0 };
    quint64 connTime{ 0 };
    quint64 bytesOut{ 0 };
    quint64 bytesIn{ 0 };

    quint32 targetSerNum{ 0 };
    quint32 targetHost{ 0 };
    quint32 sceneHost{ 0 };
    quint32 sernum_i{ 0 };

    qint64 maxIdleTime{ 0 };

    qint32 pktHeaderSlot{ 0 };
    qint32 cmdAttempts{ 0 };
    qint32 adminRank{ -1 };

    int packetFloodCount{ 0 };
    int packetsOut{ 0 };
    int packetsIn{ 0 };
    int slotPos{ -1 };

    QElapsedTimer floodTimer;
    QElapsedTimer idleTime;

    QTimer connTimer;
    QTimer killTimer;
    QTimer afkTimer;

    PktTarget targetType{ PktTarget::ALL };

    QIcon afkIcon;

    public:
        explicit Player(qintptr socketDescriptor);
        ~Player() override;

        quint64 getConnTime() const;
        void startConnTimer();

        QStandardItem* getTableRow() const;
        void setTableRow(QStandardItem* value);
        void setTableRowData(QStandardItem* model, const qint32& row, const qint32& column, const QVariant& data,
                             const qint32& role, const bool& isColor = false);

        ServerInfo* getServerInfo() const;
        void setServerInfo(ServerInfo* value);

        bool getIsVisible() const;
        void setIsVisible(const bool& value);

        bool getHasSernum() const;
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

        QByteArray getCampPacket() const;
        void setCampPacket(const QByteArray& value);

        bool getSentCampPacket() const;
        void setSentCampPacket(bool value);

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

        quint64 getAvgBaud(const bool& out) const;
        void setAvgBaud(const quint64 &bytes, const bool& out);

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

        quint64 getMuteDuration();
        void setMuteDuration(const quint64& value);
        bool getIsMuted();

        QIcon getAfkIcon() const;
        void setAfkIcon(const QString& value);

        bool getIsAFK() const;
        void setIsAFK(bool value);

        void validateSerNum(ServerInfo* server, const quint32& id);

        qint64 getMaxIdleTime() const;
        void setMaxIdleTime(const qint64& value);

    public slots:
        void sendPacketToPlayerSlot(Player* plr, qint32 targetType, quint32 trgSerNum, quint32 trgScene, const QByteArray& packet);
        void sendMasterMsgToPlayerSlot(Player* plr, const bool& all, const QByteArray& packet);
        void setMaxIdleTimeSlot(const qint64& maxAFK);

    private slots:
        void readyReadSlot();

    signals:
        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine) const;
        void parsePacketSignal(const QByteArray& packet, Player* plr);
};

#endif // PLAYER_HPP
