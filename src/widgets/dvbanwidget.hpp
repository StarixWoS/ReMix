#ifndef DVBANWIDGET_HPP
#define DVBANWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QWidget>

namespace Ui {
    class DVBanWidget;
}

class DVBanWidget : public QWidget
{
    Q_OBJECT

    QSortFilterProxyModel* dvProxy{ nullptr };
    QStandardItemModel* dvModel{ nullptr };

    public:
        explicit DVBanWidget(QWidget *parent = 0);
        ~DVBanWidget();

        void addDVBan(QString ip, QString& reason);
        void removeDVBan(QString& ip);

        static bool getIsDVBanned(QString ipAddr);

    private:
        void loadBannedDVs();
        void addDVBanImpl(QString& ip, QString& reason);
        void removeDVBanImpl(QModelIndex& index);

    public slots:
        void dvBanTableChangedRowSlot(const QModelIndex &index,
                                      const QModelIndex&);

    private slots:
        void on_dvBanTable_clicked(const QModelIndex &index);
        void on_addDeviceBan_clicked();
        void on_removeDVBan_clicked();

    private:
        Ui::DVBanWidget *ui;
};

#endif // DVBANWIDGET_HPP
