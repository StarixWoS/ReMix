#ifndef REMIXWIDGET_HPP
#define REMIXWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QModelIndex>
#include <QWidget>
#include <QSplitter>
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
    Server* server{ nullptr };

    //Setup Objects.
    QMenu* contextMenu{ nullptr };

    bool censorUIIPInfo{ false };
    public:
        explicit ReMixWidget(QWidget* parent = nullptr, Server* svrInfo = nullptr);
        ~ReMixWidget() override;

        Server* getServer() const;
        void renameServer(const QString& newName);

        void sendServerMessage(const QString& msg) const;
        quint32 getPlayerCount() const;
        QString getServerName() const;

        qintptr getPeerFromQItem(QStandardItem* item) const;

        bool getCensorUIIPInfo() const;

    private:
        void initUIUpdate();
        QString getInterface(QWidget* parent);

    private slots:
        void on_openSettings_clicked();
        void on_openPlayerView_clicked();
        void on_openUserInfo_clicked();
        void on_isPublicServer_toggled(bool value);
        void on_useUPNP_toggled(bool value);
        void on_networkStatus_linkActivated(const QString&);
        void on_networkStatus_customContextMenuRequested(const QPoint&);
        void on_openChatView_clicked();
        void on_useUPNP_clicked();

        void on_logButton_clicked();
        void initializeServerSlot();

        void plrConnectedSlot(qintptr socketDescriptor);
        void plrDisconnectedSlot(Player* plr, const bool& timedOut = false);
        void updatePlayerTable(Player* plr);

    public slots:
        void fwdUpdatePlrViewSlot(Player* plr, const qint32& column, const QVariant& data, const qint32& role, const bool& isColor = false);
        void insertedRowItemSlot(QStandardItem* item, const qintptr& peer, const QByteArray& data);
        void censorUIIPInfoSlot(const bool& state);

    signals:
        void reValidateServerIPSignal(const QString& interfaceIP = "");
        void crossServerCommentSignal(Server* server, const QString& comment);
        void fwdUpdatePlrViewSignal(QStandardItem* object, const qint32& column, const QVariant& data, const qint32& role, const bool& isColor = false);
        void plrViewInsertRowSignal(const qintptr& peer, const QByteArray& data);
        void plrViewRemoveRowSignal(QStandardItem* object);

    private:
        Ui::ReMixWidget* ui;
};

#endif // REMIXWIDGET_HPP
