
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

    enum Keys{ RANK = 0, HASH = 1, SALT = 2 };
    static const QString adminKeys[ 3 ];
    static const QStringList ranks;

    public:
        explicit Admin(QWidget *parent = nullptr,
                       ServerInfo* svr = nullptr);
        ~Admin();

        void loadServerAdmins();

        BanDialog* getBanDialog() const;
        void showBanDialog();

        bool makeAdmin(QString& sernum, QString& pwd);

        void setAdminData(const QString& key, const QString& subKey,
                          QVariant& value);
        QVariant getAdminData(const QString& key, const QString& subKey);

        void setReqAdminAuth(QVariant& value);
        bool getReqAdminAuth();

        bool getIsRemoteAdmin(QString& serNum);
        bool cmpRemoteAdminPwd(QString& serNum, QVariant& value);

        qint32 getRemoteAdminRank(QString& sernum);
        void setRemoteAdminRank(QString& sernum, qint32 rank);

        qint32 changeRemoteAdminRank(QWidget* parent, QString& sernum);
        bool deleteRemoteAdmin(QWidget* parent, QString& sernum);
        bool createRemoteAdmin(QWidget* parent, QString& sernum);

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
