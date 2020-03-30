
#ifndef SERVER_HPP
#define SERVER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QTcpServer>
#include <QTimer>

class Server : public QTcpServer
{
    Q_OBJECT

    static QHash<QHostAddress, QByteArray> bioHash;
    QStandardItemModel* plrViewModel{ nullptr };
    QHash<QString, QStandardItem*> plrTableItems;

    QWidget* mother{ nullptr };

    Comments* serverComments{ nullptr };
    PacketHandler* pktHandle{ nullptr };
    ChatView* chatView{ nullptr };
    ServerInfo* server{ nullptr };

    public:
        Server(QWidget* parent = nullptr, ServerInfo* svr = nullptr, QStandardItemModel* plrView = nullptr);
        ~Server() override;

        void setupServerInfo();
        void updatePlayerTable(Player* plr, const QHostAddress& peerAddr, const quint16& port);
        QStandardItem* updatePlayerTableImpl(const QString& peerIP, const QByteArray& data, Player* plr, const bool& insert);

        Comments* getServerComments() const;
        ChatView* getChatView() const;

        void userReadyRead(QTcpSocket* socket);
        void userDisconnected(QTcpSocket* socket);

        static QHash<QHostAddress, QByteArray> getBioHash();
        static void insertBioHash(const QHostAddress& addr, const QByteArray& value);
        static QByteArray getBioHashValue(const QHostAddress& addr);
        static QHostAddress getBioHashKey(const QByteArray& bio);

    private slots:
        void newConnectionSlot();
};

#endif // SERVER_HPP
