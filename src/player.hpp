#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QElapsedTimer>
#include <QHostAddress>
#include <QTcpSocket>

class Player
{
    QTcpSocket* socket{ nullptr };
    QString publicIP{ "" };

    QString alias{ "" };
    QString playTime{ "" };

    QByteArray outBuff;

    unsigned int sernum{ 0 };
    unsigned int privateDest{ 0 };
    unsigned int sendDest{ 0 };

    bool pwdRequested{ false };
    bool enteredPwd{ false };
    int slotPos{ -1 };

    public:
        explicit Player();
        ~Player();

        QElapsedTimer connectionTime;

        QTcpSocket* getSocket() const;
        void setSocket(QTcpSocket* value);

        unsigned int getSernum() const;
        void setSernum(unsigned int value);

        unsigned int getPrivateDest() const;
        void setPrivateDest(unsigned int value);

        unsigned int getSendDest() const;
        void setSendDest(unsigned int value);

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
};

#endif // PLAYER_HPP
