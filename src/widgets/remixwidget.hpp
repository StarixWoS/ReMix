#ifndef REMIXWIDGET_HPP
#define REMIXWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QModelIndex>
#include <QWidget>

namespace Ui {
    class ReMixWidget;
}

class ReMixWidget : public QWidget
{
    Q_OBJECT

    QHash<QString, QStandardItem*> plrTableItems;
    QStandardItemModel* plrViewModel{ nullptr };

    MOTDWidget* motdWidget{ nullptr };
    RulesWidget* rules{ nullptr };

    PlrListWidget* plrWidget{ nullptr };
    ServerInfo* server{ nullptr };
    Server* tcpServer{ nullptr };

    Comments* serverComments{ nullptr };
    PacketHandler* pktHandle{ nullptr };
    ChatView* chatView{ nullptr };

    //Setup Objects.
    QMenu* contextMenu{ nullptr };

    public:
        explicit ReMixWidget(QWidget* parent = nullptr, ServerInfo* svrInfo = nullptr);
        ~ReMixWidget() override;

        ServerInfo* getServerInfo() const;
        void renameServer(const QString& newName);

        void sendServerMessage(const QString& msg);
        quint32 getPlayerCount() const;
        QString getServerName() const;

        Server* getTcpServer() const;
        quint16 getPrivatePort() const;

    private:
        void initUIUpdate();

    private slots:
        void on_openSettings_clicked();
        void on_openUserComments_clicked();
        void on_openUserInfo_clicked();
        void on_isPublicServer_toggled(bool value);
        void on_useUPNP_toggled(bool value);
        void on_networkStatus_linkActivated(const QString& link);
        void on_networkStatus_customContextMenuRequested(const QPoint&);
        void on_openChatView_clicked();

        void on_logButton_clicked();

        void initializeServerSlot();

        void plrConnectedSlot(qintptr socketDescriptor);
        void plrDisconnectedSlot(Player* plr);

        void updatePlayerTable(Player* plr);
        QStandardItem* updatePlayerTableImpl(const QString& peerIP, const QByteArray& data, Player* plr, const bool& insert);

    signals:
        void reValidateServerIPSignal();

    private:
        Ui::ReMixWidget* ui;
};

#endif // REMIXWIDGET_HPP
