#ifndef BANNEDIP_HPP
#define BANNEDIP_HPP

#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QDialog>

namespace Ui {
    class BannedIP;
}

class BannedIP : public QDialog
{
    Q_OBJECT

    QSortFilterProxyModel* tableProxy{ nullptr };
    QStandardItemModel* tableModel{ nullptr };

    public:
        explicit BannedIP(QWidget *parent = 0);
        ~BannedIP();

    private:
        Ui::BannedIP *ui;
};

#endif // BANNEDIP_HPP
