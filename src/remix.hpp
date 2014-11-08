
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
#include <QDebug>
#include <QTimer>

class BannedSernum;
class UserMessage;
class Messages;
class BannedIP;
class Server;

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

    public:
        explicit ReMix(QWidget *parent = 0);
        ~ReMix();

    private slots:
        void on_openSysMessages_clicked();
        void on_openBanIP_clicked();
        void on_openBannedSernums_clicked();
        void on_openUserComments_clicked();
        void on_enableNetworking_clicked();

        void on_isPublicServer_clicked();

    private:
        Ui::ReMix *ui;
};

#endif // REMIX_HPP
