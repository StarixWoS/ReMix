#ifndef REMIXWIDGET_HPP
#define REMIXWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QModelIndex>
#include <QSplitter>
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

    QSharedPointer<Server> server;

    //Setup Objects.
    QMenu* contextMenu{ nullptr };

    bool censorUIIPInfo{ false };
    public:
        explicit ReMixWidget(QSharedPointer<Server> svrInfo, QWidget* parent = nullptr);
        ~ReMixWidget() override;

        QSharedPointer<Server> getServer() const;
        void renameServer(const QString& newName);

        void sendServerMessage(const QString& msg) const;
        quint32 getPlayerCount() const;
        QString getServerName() const;

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
        void plrDisconnectedSlot(QSharedPointer<Player> plr, const bool& timedOut = false);
        void updatePlayerTable(QSharedPointer<Player> plr);

    public slots:
        void censorUIIPInfoSlot(const bool& state);

    signals:
        void reValidateServerIPSignal(const QString& interfaceIP = "");
        void crossServerCommentSignal(const QSharedPointer<Server> server, const QString& comment);
        void plrViewInsertRowSignal(QSharedPointer<Player> plr, const QString& ipPortStr, const QByteArray& data);
        void plrViewRemoveRowSignal(QSharedPointer<Player> plr);

    private:
        Ui::ReMixWidget* ui;
};

#endif // REMIXWIDGET_HPP
