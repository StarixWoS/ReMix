#ifndef SNBANWIDGET_HPP
#define SNBANWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QWidget>

namespace Ui {
    class SNBanWidget;
}

class SNBanWidget : public QWidget
{
    Q_OBJECT

    QSortFilterProxyModel* snProxy{ nullptr };
    QStandardItemModel* snModel{ nullptr };

    public:
        explicit SNBanWidget(QWidget *parent = 0);
        ~SNBanWidget();

        bool getIsSernumBanned(QString sernum);

        void addSerNumBan(QString& sernum, QString& reason);

        void removeSerNumBan(QString& sernum);

    private:
        void loadBannedSernums();

        void addSerNumBanImpl(QString& sernum, QString& reason);

        void removeSerNumBanImpl(QModelIndex& index);

    public slots:
        void snBanTableChangedRowSlot(const QModelIndex& index,
                                      const QModelIndex&);

    private slots:
        void on_snBanTable_clicked(const QModelIndex& index);
        void on_forgiveButton_clicked();
        void on_addSernumBan_clicked();

    private:
        Ui::SNBanWidget *ui;
};

#endif // SNBANWIDGET_HPP
