
#ifndef READMIN_HPP
#define READMIN_HPP

#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QCryptographicHash>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDialog>
#include <QMenu>
#include <QFile>

#include "adminhelper.hpp"

class ServerInfo;
class BanDialog;
class RandDev;
class Player;

namespace Ui {
    class Admin;
}

class Admin : public QDialog
{
    Q_OBJECT

    Ui::Admin *ui;

    QSortFilterProxyModel* tableProxy{ nullptr };
    QStandardItemModel* tableModel{ nullptr };

    ServerInfo* server{ nullptr };
    RandDev* randDev{ nullptr };
    BanDialog* banDialog{ nullptr };

    QMenu* contextMenu{ nullptr };
    QModelIndex menuIndex;

    public:
        enum Ranks{ GMASTER = 0, COADMIN = 1, ADMIN = 2, OWNER = 3 };

        explicit Admin(QWidget *parent = nullptr, ServerInfo* svr = nullptr);
        ~Admin();

        void loadServerAdmins();

        BanDialog* getBanDialog() const;
        void showBanDialog();

        bool makeAdmin(QString& sernum, QString& pwd);
        bool parseCommand(QString& packet, Player* plr);

    private:
        void initContextMenu();
        void setAdminRank(int rank, QModelIndex index);
        bool makeAdminImpl(QString& sernum, QString& pwd);

    private slots:
        void on_makeAdmin_clicked();
        void on_adminTable_customContextMenuRequested(const QPoint& pos);
        void on_actionRevokeAdmin_triggered();
        void on_actionChangeRank_triggered();
};

#endif // READMIN_HPP
