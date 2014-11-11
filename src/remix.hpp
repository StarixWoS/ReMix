
#ifndef REMIX_HPP
#define REMIX_HPP

#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QNetworkInterface>
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QDateTime>
#include <QHostInfo>
#include <QFileInfo>
#include <QDebug>
#include <QTimer>

//Standard C++/11 Headers
#include <chrono>

//ServerInfo structure.
#include "serverinfo.hpp"

class BannedSernum;
class UserMessage;
class Messages;
class BannedIP;
class Server;

namespace Helper
{
    namespace RandDev{}
    int genRandNum(int min = 0, int max = 32767);
}

namespace Ui {
class ReMix;
}

class ReMix : public QMainWindow
{
    Q_OBJECT

    QSortFilterProxyModel* plrViewProxy{ nullptr };
    QStandardItemModel* plrViewModel{ nullptr };

    QSortFilterProxyModel* svrViewProxy{ nullptr };
    QStandardItemModel* svrViewModel{ nullptr };

    BannedSernum* sernumBan{ nullptr };
    Messages* sysMessages{ nullptr };
    UserMessage* usrMsg{ nullptr };
    Server* tcpServer{ nullptr };
    BannedIP* banIP{ nullptr };

    ServerInfo* serverInfo{ nullptr };

    public:
        explicit ReMix(QWidget *parent = 0);
        ~ReMix();

        int genServerID();
        void parseCMDLArgs();
        void getSynRealData();

    private slots:
        void on_openSysMessages_clicked();
        void on_openBanIP_clicked();
        void on_openBannedSernums_clicked();
        void on_openUserComments_clicked();
        void on_enableNetworking_clicked();
        void on_isPublicServer_stateChanged(int arg1);

        void on_serverPort_textChanged(const QString &arg1);
        void on_serverName_textChanged(const QString &arg1);

    private:
        Ui::ReMix *ui;
};

#endif // REMIX_HPP
