
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
    RandDev* randDev{ nullptr };

    QMenu* contextMenu{ nullptr };
    QModelIndex menuIndex;

    enum Keys{ RANK = 0, HASH = 1, SALT = 2 };
    static const QString adminKeys[ 3 ];
    static const QStringList ranks;

    public:
        explicit Admin(QWidget *parent = nullptr);
        ~Admin();

        void loadServerAdmins();

        BanDialog* getBanDialog() const;
        void showBanDialog();

        bool makeAdmin(QString& sernum, QString& pwd);

        static void setAdminData(const QString& key, const QString& subKey,
                                 QVariant& value);
        static QVariant getAdminData(const QString& key, const QString& subKey);

        static bool getIsRemoteAdmin(QString& serNum);
        static bool cmpRemoteAdminPwd(QString& serNum, QString& value);

        static qint32 getRemoteAdminRank(QString& sernum);
        static void setRemoteAdminRank(QString& sernum, qint32 rank);

        static qint32 changeRemoteAdminRank(QWidget* parent, QString& sernum);
        static bool deleteRemoteAdmin(QWidget* parent, QString& sernum);
        static bool createRemoteAdmin(QWidget* parent, QString& sernum);

//        static bool remoteChangeRank(QString& sernum, qint32 rank);
//        static bool remoteRemoveAdmin(QString& sernum);

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
