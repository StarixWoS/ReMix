#ifndef BANWIDGET_HPP
#define BANWIDGET_HPP

#include "prototypes.hpp"
#include <QWidget>

namespace Ui {
    class BanWidget;
}

class BanWidget : public QWidget
{
    Q_OBJECT

    QSortFilterProxyModel* tblProxy{ nullptr };
    QStandardItemModel* tblModel{ nullptr };

    public:
        explicit BanWidget(QWidget *parent = nullptr);
        ~BanWidget();

        enum Rows{ ip = 0, sn = 1, dv = 2, wv = 3, reason = 4, date = 5 };
        enum Types{ COUNT = 0, IP = 1, SN = 2, DV = 3, WV = 4, Reason = 5,
                    Date = 6 };

        static const QString banKeys[ BAN_KEY_COUNT ];

        static quint32 getIsBanned(QString value);

        void addBan(Player* plr, QString& reason);
        void removeBan(QString& value, qint32 type);

    private:
        void loadBans();
        void addBanImpl(QString& ip, QString& sn, QString& dv, QString& wv,
                        QString& reason);
        void removeBanImpl(QModelIndex& index, QString& value);

    public slots:
        void banTableChangedRowSlot(const QModelIndex& index,
                                    const QModelIndex&);
    private slots:
        void on_addBan_clicked();
        void on_removeBan_clicked();

    private:
        Ui::BanWidget *ui;
};

#endif // BANWIDGET_HPP
