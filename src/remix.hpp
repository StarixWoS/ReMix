
#ifndef REMIX_HPP
#define REMIX_HPP

#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QNetworkInterface>
#include <QSystemTrayIcon>
#include <QStringBuilder>
#include <QElapsedTimer>
#include <QInputDialog>
#include <QMessageBox>
#include <QMainWindow>
#include <QCloseEvent>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QDateTime>
#include <QHostInfo>
#include <QFileInfo>
#include <QDebug>
#include <QTimer>
#include <QEvent>

//ServerInfo structure.
#include "serverinfo.hpp"

class Messages;
class Settings;
class Server;
class Admin;

class RandDev;

namespace Ui {
class ReMix;
}

class ReMix : public QMainWindow
{
    Q_OBJECT

    QSortFilterProxyModel* plrProxy{ nullptr };
    QStandardItemModel* plrModel{ nullptr };

    Messages* sysMessages{ nullptr };
    Server* tcpServer{ nullptr };
    Admin* admin{ nullptr };
    RandDev* randDev{ nullptr };
    Settings* settings{ nullptr };

    ServerInfo* server{ nullptr };

    QMenu* contextMenu{ nullptr };
    QMenu* trayMenu{ nullptr };
    QModelIndex menuIndex;

    QSystemTrayIcon* trayIcon{ nullptr };

    enum CMDLArgs{ GAME = 0, MASTER = 1, PUBLIC = 2, LISTEN = 3, NAME = 4, FUDGE = 5 };
    const QStringList cmdlArgs =
    {
        QStringList() << "game" << "master" << "public" << "listen" << "name" << "fudge"
    };

    public:
        explicit ReMix(QWidget *parent = 0);
        ~ReMix();

        int genServerID();
        void parseCMDLArgs();
        void getSynRealData();
        void initContextMenu();

    private slots:
        void on_openSysMessages_clicked();
        void on_openSettings_clicked();
        void on_openBanDialog_clicked();
        void on_openUserComments_clicked();
        void on_enableNetworking_clicked();
        void on_openRemoteAdmins_clicked();
        void on_isPublicServer_stateChanged(int arg1);

        void on_serverPort_textChanged(const QString &arg1);
        void on_serverName_textChanged(const QString &arg1);

        void on_playerView_customContextMenuRequested(const QPoint &pos);
        void on_actionSendMessage_triggered();
        void on_actionMakeAdmin_triggered();
        void on_actionDisconnectUser_triggered();
        void on_actionBANISHIPAddress_triggered();
        void on_actionBANISHSerNum_triggered();
        void on_actionRevokeAdmin_triggered();

        //Handle Minimize events.
        void changeEvent(QEvent* event);
        void closeEvent(QCloseEvent* event);
        bool rejectCloseEvent();

    private:
        Ui::ReMix *ui;
};

#endif // REMIX_HPP
