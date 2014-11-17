#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QElapsedTimer>
#include <QTcpSocket>
#include <QObject>

class Player : public QObject
{
    Q_OBJECT

    QTcpSocket* socket{ nullptr };
    QString alias{ "" };
    QString playTime{ "" };

    QByteArray outBuff;

    unsigned int sernum{ 0 };
    unsigned int privateDest{ 0 };
    unsigned int sendDest{ 0 };

    public:
        explicit Player(QObject *parent = 0);
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

    signals:

    public slots:

};

#endif // PLAYER_HPP
