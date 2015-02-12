#ifndef DABANWIDGET_HPP
#define DABANWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QWidget>

namespace Ui {
    class DABanWidget;
}

class DABanWidget : public QWidget
{
    Q_OBJECT

    QSortFilterProxyModel* daProxy{ nullptr };
    QStandardItemModel* daModel{ nullptr };

    public:
        explicit DABanWidget(QWidget *parent = 0);
        ~DABanWidget();

        void addDABan(QString ip, QString& reason);
        void removeDABan(QString& ip);

        static bool getIsDABanned(QString ipAddr);

    private:
        void loadBannedDAs();
        void addDABanImpl(QString& ip, QString& reason);
        void removeDABanImpl(QModelIndex& index);

    public slots:
        void daBanTableChangedRowSlot(const QModelIndex &index,
                                      const QModelIndex&);

    private slots:
        void on_addDateBan_clicked();
        void on_removeDABan_clicked();
        void on_daBanTable_clicked(const QModelIndex &index);

    private:
        Ui::DABanWidget *ui;
};

#endif // DABANWIDGET_HPP
