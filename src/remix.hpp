
#ifndef REMIX_HPP
#define REMIX_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QMainWindow>

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

    Player* menuTarget{ nullptr };

    QSystemTrayIcon* trayIcon{ nullptr };

    enum CMDLArgs{ GAME = 0, MASTER = 1, PUBLIC = 2, LISTEN = 3, NAME = 4,
                   FUDGE = 5 };
    static const QStringList cmdlArgs;

    bool hasSysTray{ false };
    private:

    #if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
        void initSysTray();
    #endif
        void initUIUpdate();

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
        void on_actionMuteNetwork_triggered();
        void on_actionUnMuteNetwork_triggered();
        void on_actionDisconnectUser_triggered();
        void on_actionBANISHIPAddress_triggered();
        void on_actionBANISHSerNum_triggered();
        void on_actionRevokeAdmin_triggered();

        //Handle Minimize events.
        #if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
            void changeEvent(QEvent* event);
        #endif

        void closeEvent(QCloseEvent* event);
        bool rejectCloseEvent();

    private:
        Ui::ReMix *ui;
};

#endif // REMIX_HPP
