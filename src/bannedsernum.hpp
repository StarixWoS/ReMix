
#ifndef BANNEDSERNUM_HPP
#define BANNEDSERNUM_HPP

#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QDateTime>
#include <QSettings>
#include <QDialog>
#include <QDebug>
#include <QFile>

namespace Ui {
    class BannedSernum;
}

class BannedSernum : public QDialog
{
    Q_OBJECT

    QSortFilterProxyModel* tableProxy{ nullptr };
    QStandardItemModel* tableModel{ nullptr };

    void loadBannedSernums();
    public:
        explicit BannedSernum(QWidget *parent = 0);
        ~BannedSernum();

    private slots:
        void currentRowChangedSlot(const QModelIndex& index, const QModelIndex&);
        void on_forgiveButton_clicked();

        void on_sernumTable_clicked(const QModelIndex &index);

        void on_cancelButton_clicked();

    private:
        Ui::BannedSernum *ui;
};

#endif // BANNEDSERNUM_HPP
