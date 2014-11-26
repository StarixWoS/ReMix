
#ifndef READMIN_HPP
#define READMIN_HPP

#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QCryptographicHash>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDialog>
#include <QDebug>
#include <QMenu>
#include <QFile>

#include "adminhelper.hpp"

class BanDialog;
class RandDev;

namespace Ui {
    class Dialog;
}

class Admin : public QDialog
{
    Q_OBJECT

    Ui::Dialog *ui;

    QSortFilterProxyModel* tableProxy{ nullptr };
    QStandardItemModel* tableModel{ nullptr };

    RandDev* randDev{ nullptr };
    BanDialog* banDialog{ nullptr };

    QMenu* contextMenu{ nullptr };
    QModelIndex menuIndex;

    public:
        enum Ranks{ GMASTER = 0, COADMIN = 1, ADMIN = 2, OWNER = 3 };

        explicit Admin(QWidget *parent = 0);
        ~Admin();

        void loadServerAdmins();
        void showBanDialog();

    private:
        void initContextMenu();
        void setAdminRank(int rank, QModelIndex index);

    private slots:
        void on_makeAdmin_clicked();
        void on_adminTable_customContextMenuRequested(const QPoint& pos);
        void on_actionRevokeAdmin_triggered();
        void on_actionChangeRank_triggered();
};

#endif // READMIN_HPP
