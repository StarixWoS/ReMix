#ifndef BANNEDSERNUM_HPP
#define BANNEDSERNUM_HPP

#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QDialog>

namespace Ui {
    class BannedSernum;
}

class BannedSernum : public QDialog
{
    Q_OBJECT

    QSortFilterProxyModel* tableProxy{ nullptr };
    QStandardItemModel* tableModel{ nullptr };

    public:
        explicit BannedSernum(QWidget *parent = 0);
        ~BannedSernum();

    private:
        Ui::BannedSernum *ui;
};

#endif // BANNEDSERNUM_HPP
