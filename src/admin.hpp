
#ifndef READMIN_HPP
#define READMIN_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QModelIndex>
#include <QDialog>
#include <QObject>

namespace Ui {
    class Admin;
}

class Admin : public QDialog
{
    Q_OBJECT

    Ui::Admin *ui;

    QSortFilterProxyModel* tableProxy{ nullptr };
    QStandardItemModel* tableModel{ nullptr };

    BanDialog* banDialog{ nullptr };
    ServerInfo* server{ nullptr };
    RandDev* randDev{ nullptr };

    QMenu* contextMenu{ nullptr };
    QModelIndex menuIndex;

    static const QStringList commands;

    public:
        explicit Admin(QWidget *parent = nullptr, ServerInfo* svr = nullptr);
        ~Admin();

        void loadServerAdmins();

        BanDialog* getBanDialog() const;
        void showBanDialog();

        bool makeAdmin(QString& sernum, QString& pwd);

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
