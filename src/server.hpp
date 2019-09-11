
#ifndef SERVER_HPP
#define SERVER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QTcpServer>
#include <QTimer>

class Server : public QTcpServer
{
    Q_OBJECT

    QStandardItemModel* plrViewModel{ nullptr };
    QHash<QString, QStandardItem*> plrTableItems;
    QHash<QHostAddress, QByteArray> bioHash;

    QWidget* mother{ nullptr };
    QUdpSocket* masterSocket{ nullptr };

    Comments* serverComments{ nullptr };
    PacketHandler* pktHandle{ nullptr };
    ChatView* chatView{ nullptr };
    ServerInfo* server{ nullptr };

    public:
        Server(QWidget* parent = nullptr, ServerInfo* svr = nullptr,
               QStandardItemModel* plrView = nullptr);
        ~Server() override;

        void setupServerInfo();
        void updatePlayerTable(Player* plr, const QHostAddress& peerAddr,
                               const quint16& port);
        QStandardItem* updatePlayerTableImpl(const QString& peerIP,
                                             const QByteArray& data,
                                             Player* plr, const bool& insert);

        Comments* getServerComments() const;
        ChatView* getChatView() const;

        void userReadyRead(QTcpSocket* socket);
        void userDisconnected(QTcpSocket* socket);

    private slots:
        void newConnectionSlot();
        void readyReadUDPSlot();
};

#endif // SERVER_HPP
