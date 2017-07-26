
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
    User* user{ nullptr };
    UPNP* upnp{ nullptr };

    QString serverID{ "" };
    public:
        Server(QWidget* parent = nullptr, ServerInfo* svr = nullptr,
               User* adminDlg = nullptr, QStandardItemModel* plrView = nullptr,
               QString svrID = "0");
        ~Server();

        void setupServerInfo();
        void setupUPNPForward();
        void removeUPNPForward();
        void setupPublicServer(bool value);

        void updatePlayerTable(Player* plr, QHostAddress peerAddr,
                               quint16 port);
        QStandardItem* updatePlayerTableImpl(QString& peerIP, QByteArray& data,
                                             Player* plr, bool insert);

        Comments* getServerComments() const;
        ChatView* getChatView() const;

        void userReadyRead(QTcpSocket* socket);
        void userDisconnected(QTcpSocket* socket);

    public slots:
        void newRemotePwdRequestedSlot(Player* plr);
        void newRemoteAdminRegisterSlot(Player* plr);

    private slots:
        void newConnectionSlot();
        void readyReadUDPSlot();
};

#endif // SERVER_HPP
