
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

    ServerInfo* server{ nullptr };
    RandDev* randDev{ nullptr };
    BanDialog* banDialog{ nullptr };

    QMenu* contextMenu{ nullptr };
    QModelIndex menuIndex;

    static const QStringList commands;

    public:
        enum Ranks{ GMASTER = 0, COADMIN = 1, ADMIN = 2, OWNER = 3 };
        enum CMDS{ BAN = 0, KICK = 1, MUTE = 2, MSG = 3 };

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
