
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
    ServerInfo* server{ nullptr };
    User* user{ nullptr };

    public:
        Server(QWidget* parent = nullptr, ServerInfo* svr = nullptr,
               User* adminDlg = nullptr,
               QStandardItemModel* plrView = nullptr );
        ~Server();

        void setupServerInfo();
        void setupPublicServer(bool value);

        void updatePlayerTable(Player* plr, QHostAddress peerAddr,
                               quint16 port);
        QStandardItem* updatePlayerTableImpl(QString& peerIP, QByteArray& data,
                                             Player* plr, bool insert);

        void showServerComments();

    public slots:
        void sendRemoteAdminPwdReqSlot(Player* plr);
        void sendRemoteAdminRegisterSlot(Player* plr);

    private slots:
        void newConnectionSlot();
        void userReadyReadSlot();
        void userDisconnectedSlot();

        void readyReadUDPSlot();
};

#endif // SERVER_HPP
