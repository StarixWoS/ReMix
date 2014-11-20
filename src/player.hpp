#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QElapsedTimer>
#include <QHostAddress>
#include <QTcpSocket>
#include <QObject>

class Player
{
    QTcpSocket* socket{ nullptr };
    QString publicIP{ "" };

    QString alias{ "" };
    QString playTime{ "" };

    QByteArray outBuff;

    quint32 sernum{ 0 };
    quint32 sceneHost{ 0 };

    quint32 targetHost{ 0 };
    quint32 targetSerNum{ 0 };
    int targetType{ 0 };

    bool pwdRequested{ false };
    bool enteredPwd{ false };
    int slotPos{ -1 };

    //HB ID/Slot: Set by :MIX7 and is used to disconnect a certain type of hacker.
    quint32 hbID{ 0 };
    int hbSlot{ 0 };

    QElapsedTimer connectionTime;
    QElapsedTimer lastPacketTime;

    public:
        explicit Player();

        enum Target{ ALL = 0, PLAYER = 1, SCENE = 2 };

        qint64 getLastPacketTime() const;
        void startLastPacketTime();

        qint64 getConnectionTime() const;
        void startConnectionTime();

        QTcpSocket* getSocket() const;
        void setSocket(QTcpSocket* value);

        quint32 getSernum() const;
        void setSernum(quint32 value);

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

        quint32 getHBID() const;
        void setHBID(quint32 value);

        int getHBSlot() const;
        void setHBSlot(int value);
};

#endif // PLAYER_HPP
