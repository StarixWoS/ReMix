
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

    QTcpSocket* socket{ nullptr };
    QString publicIP{ "" };
    quint32 publicPort{ 0 };

    QString alias{ "" };
    QString playTime{ "" };

    QByteArray bioData;
    QByteArray outBuff;

    bool hasSernum{ false };
    qint32 sernum{ 0 };
    QString sernum_s{ "" };

    qint32 sceneHost{ 0 };

    qint32 targetHost{ 0 };
    qint32 targetSerNum{ 0 };
    int targetType{ 0 };

    bool pwdRequested{ false };
    bool enteredPwd{ false };
    int slotPos{ -1 };

    bool reqAuthPwd{ false };
    bool gotAuthPwd{ false };
    qint32 adminRank{ -1 };

    qint32 cmdAttempts{ 0 };  //Max limit is 3 attempts before auto-banning.

    bool reqNewAuthPwd{ false };
    bool gotNewAuthPwd{ false };

    QElapsedTimer floodTimer;
    int packetFloodCount{ 0 };

    int packetsIn{ 0 };
    quint64 bytesIn{ 0 };
    quint32 avgBaudIn{ 0 };

    int packetsOut{ 0 };
    quint64 bytesOut{ 0 };
    quint32 avgBaudOut{ 0 };

    QTimer connTimer;
    quint64 connTime{ 0 };
    QElapsedTimer idleTime;

    QTimer killTimer;
    bool pendingHardDisconnect{ false };
    bool pendingSoftDisconnect{ false };

    bool networkMuted{ false };

    #ifdef DECRYPT_PACKET_PLUGIN
        QString gameInfo{ "" };
        bool hasWorldInfo{ false };
    #endif

    Admin* adminDialog{ nullptr };

    public:
        explicit Player(Admin* aDlg = nullptr);
        ~Player();

        enum Target{ ALL = 0, PLAYER = 1, SCENE = 2 };

        qint64 getConnTime() const;
        void startConnTimer();

        QStandardItem* getTableRow() const;
        void setTableRow(QStandardItem* value);

        QTcpSocket* getSocket() const;
        void setSocket(QTcpSocket* value);

        qint32 getSernum() const;
        void setSernum(qint32 value);

        QString getSernum_s() const;
        void setSernum_s(const QString& value);

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

        QByteArray getBioData() const;
        void setBioData(const QByteArray& value);

        QByteArray getOutBuff() const;
        void setOutBuff(const QByteArray& value);

        bool getPwdRequested() const;
        void setPwdRequested(bool value);

        int getSlotPos() const;
        void setSlotPos(int value);

        QString getPublicIP() const;
        void setPublicIP(const QString& value);

        quint32 getPublicPort() const;
        void setPublicPort(const quint32& value);

        bool getEnteredPwd() const;
        void setEnteredPwd(bool value);

        quint64 getFloodTime() const;
        void restartFloodTimer();

        int getPacketFloodCount() const;
        void setPacketFloodCount(int value);

        int getPacketsIn() const;
        void setPacketsIn(int value, int incr);

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

        void resetAdminAuth();

        bool getReqAuthPwd() const;
        void setReqAuthPwd(bool value);

        bool getGotAuthPwd() const;
        void setGotAuthPwd(bool value);

        qint32 getAdminRank() const;
        void setAdminRank(const qint32& value);

        qint32 getCmdAttempts() const;
        void setCmdAttempts(const qint32& value);

        bool getReqNewAuthPwd() const;
        void setReqNewAuthPwd(bool value);

        bool getGotNewAuthPwd() const;
        void setGotNewAuthPwd(bool value);

        //Note: A User will be disconnected on their next update.
        //Usually every 1,000 MS.
        bool getHardDisconnect() const;
        void setHardDisconnect(bool value);

        bool getSoftDisconnect() const;
        void setSoftDisconnect(bool value);

        bool getNetworkMuted() const;
        void setNetworkMuted(bool value);

        #ifdef DECRYPT_PACKET_PLUGIN
            QString getGameInfo() const;
            void setGameInfo(const QString& value);

            bool getHasGameInfo() const;
            void setHasGameInfo(bool value);
        #endif

    signals:
        void sendRemoteAdminPwdReqSignal(Player* plr, QString& serNum);
};

#endif // PLAYER_HPP
