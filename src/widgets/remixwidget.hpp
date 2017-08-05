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

    MOTDWidget* motdWidget{ nullptr };
    RulesWidget* rules{ nullptr };

    PlrListWidget* plrWidget{ nullptr };

    Server* tcpServer{ nullptr };
    RandDev* randDev{ nullptr };
    Settings* settings{ nullptr };

    ServerInfo* server{ nullptr };
    User* user{ nullptr };

    //Setup Objects.
    QMenu* contextMenu{ nullptr };
    QPalette defaultPalette;
    bool nightMode{ false };

    QString serverID{ "" };

    enum Themes{ LIGHT = 0, DARK = 1 };

    public:
        explicit ReMixWidget(QWidget* parent = nullptr,
                             ServerInfo* svrInfo = nullptr);
        ~ReMixWidget();

        void sendServerMessage(QString msg);
        qint32 getPlayerCount();
        QString getServerName() const;

        Settings* getSettings() const;
        Server* getTcpServer() const;

        QString& getServerID();
        quint16 getPrivatePort() const;

    private:
        void initUIUpdate();

    private slots:
        void on_openSettings_clicked();
        void on_openUserComments_clicked();
        void on_openUserInfo_clicked();
        void on_isPublicServer_toggled(bool);

        void on_networkStatus_linkActivated(const QString &link);
        void on_networkStatus_customContextMenuRequested(const QPoint &pos);

        void on_openChatView_clicked();

    signals:
        void reValidateServerIP();

    private:
        Ui::ReMixWidget *ui;
};

#endif // REMIXWIDGET_HPP
