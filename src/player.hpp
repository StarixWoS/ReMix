#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QStandardItemModel>
#include <QElapsedTimer>
#include <QStandardItem>
#include <QHostAddress>
#include <QTcpSocket>
#include <QObject>
#include <QTimer>

const int MAX_CMD_ATTEMPTS = 3;
const int PACKET_FLOOD_LIMIT = 256; //Users are able to send 256 packets within PACKET_FLOOD_TIME.
const int PACKET_FLOOD_TIME = 2000; //Anything above PACKET_FLOOD_LIMIT within 2000MS will disconnect/ban the User.
class Player : public QObject
{
    Q_OBJECT

    QStandardItem* tableRow{ nullptr };

    QTcpSocket* socket{ nullptr };
    QString publicIP{ "" };

    QString alias{ "" };
    QString playTime{ "" };

    QByteArray outBuff;

    quint32 sernum{ 0 };
    QString sernum_s{ "" };

    quint32 sceneHost{ 0 };

    quint32 targetHost{ 0 };
    quint32 targetSerNum{ 0 };
    int targetType{ 0 };

    bool pwdRequested{ false };
    bool enteredPwd{ false };
    int slotPos{ -1 };

    bool adminPwdRequested{ false };
    bool adminPwdEntered{ false };
    quint32 adminCmdAttempts{ MAX_CMD_ATTEMPTS };  //Max limit is 3 attempts before auto-banning.

    int packetFloodCount{ 0 };
    int packetsIn{ 0 };
    quint64 bytesIn{ 0 };
    quint32 avgBaudIn{ 0 };

    int packetsOut{ 0 };
    quint64 bytesOut{ 0 };
    quint32 avgBaudOut{ 0 };

    QTimer connTimer;
    quint64 connTime{ 0 };
    QElapsedTimer lastPacketTime;

    public:
        explicit Player();
        ~Player();

        enum Target{ ALL = 0, PLAYER = 1, SCENE = 2 };

        qint64 getConnTime() const;
        void startConnTimer();

        QStandardItem* getTableRow() const;
        void setTableRow(QStandardItem* value);

        QTcpSocket* getSocket() const;
        void setSocket(QTcpSocket* value);

        quint32 getSernum() const;
        void setSernum(quint32 value);

        QString getSernum_s() const;
        void setSernum_s(const QString& value);

        quint32 getTargetScene() const;
        void setTargetScene(quint32 value);

        quint32 getSceneHost() const;
        void setSceneHost(quint32 value);

        quint32 getTargetSerNum() const;
        void setTargetSerNum(quint32 value);

        int getTargetType() const;
        void setTargetType(int value);

        QString getPlayTime() const;
        void setPlayTime(const QString& value);

        QString getAlias() const;
        void setAlias(const QString& value);

        QByteArray getOutBuff() const;
        void setOutBuff(const QByteArray& value);

        bool getPwdRequested() const;
        void setPwdRequested(bool value);

        int getSlotPos() const;
        void setSlotPos(int value);

        QString getPublicIP() const;
        void setPublicIP(const QString& value);

        bool getEnteredPwd() const;
        void setEnteredPwd(bool value);

        int getPacketsIn() const;
        void setPacketsIn(int value);

        quint64 getBytesIn() const;
        void setBytesIn(const quint64& value);

        quint64 getAvgBaudIn() const;
        void setAvgBaudIn(const quint64& bIn);

        int getPacketsOut() const;
        void setPacketsOut(int value);

        quint64 getBytesOut() const;
        void setBytesOut(const quint64& value);

        quint64 getAvgBaudOut() const;
        void setAvgBaudOut(const quint64& bOut);

        bool getAdminPwdRequested() const;
        void setAdminPwdRequested(bool value);

        bool getAdminPwdEntered() const;
        void setAdminPwdEntered(bool value);

        int getAdminRank() const;
        void setAdminRank(int value);
};

#endif // PLAYER_HPP
