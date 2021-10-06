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

    QHash<qintptr, QStandardItem*> plrTableItems;
    MOTDWidget* motdWidget{ nullptr };
    RulesWidget* rules{ nullptr };

    QThread* masterMixThread{ nullptr };
    PlrListWidget* plrWidget{ nullptr };
    Server* server{ nullptr };

    Comments* serverComments{ nullptr };
    PacketHandler* pktHandle{ nullptr };
    ChatView* chatView{ nullptr };

    //Setup Objects.
    QMenu* contextMenu{ nullptr };

    public:
        explicit ReMixWidget(QWidget* parent = nullptr, Server* svrInfo = nullptr);
        ~ReMixWidget() override;

        Server* getServer() const;
        void renameServer(const QString& newName);

        void sendServerMessage(const QString& msg) const;
        quint32 getPlayerCount() const;
        QString getServerName() const;

        quint16 getPrivatePort() const;
        qintptr getPeerFromQItem(QStandardItem* item) const;

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
        void plrDisconnectedSlot(Player* plr, const bool& timedOut = false);

        void updatePlayerTable(Player* plr);

    public slots:
        void fwdUpdatePlrViewSlot(Player* plr, const qint32& column, const QVariant& data, const qint32& role, const bool& isColor = false);
        void insertedRowItemSlot(QStandardItem* item, const qintptr& peer, const QByteArray& data);

    signals:
        void reValidateServerIPSignal();
        void crossServerCommentSignal(Server* server, const QString& comment);
        void fwdUpdatePlrViewSignal(QStandardItem* object, const qint32& column, const QVariant& data, const qint32& role, const bool& isColor = false);
        void plrViewInsertRowSignal(const qintptr& peer, const QByteArray& data);
        void plrViewRemoveRowSignal(QStandardItem* object);

    private:
        Ui::ReMixWidget* ui;
};

#endif // REMIXWIDGET_HPP
