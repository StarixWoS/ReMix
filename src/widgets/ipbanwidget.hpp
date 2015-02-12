
#ifndef IPBANWIDGET_HPP
#define IPBANWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QWidget>

namespace Ui {
    class IPBanWidget;
}

class IPBanWidget : public QWidget
{
    Q_OBJECT

    QSortFilterProxyModel* ipProxy{ nullptr };
    QStandardItemModel* ipModel{ nullptr };

    public:
        explicit IPBanWidget(QWidget *parent = 0);
        ~IPBanWidget();

        void addIPBan(QHostAddress& ipInfo, QString& reason);
        void addIPBan(QString ip, QString& reason);

        void removeIPBan(QHostAddress& ipInfo);
        void removeIPBan(QString& ip);

        static bool getIsIPBanned(QHostAddress& ipAddr);
        static bool getIsIPBanned(QString ipAddr);

    private:
        void loadBannedIPs();

        void addIPBanImpl(QString& ip, QString& reason);
        void removeIPBanImpl(QModelIndex& index);

    public slots:
        void ipBanTableChangedRowSlot(const QModelIndex& index,
                                      const QModelIndex&);

    private slots:
        void on_ipBanTable_clicked(const QModelIndex& index);
        void on_addIPBan_clicked();
        void on_removeIPBan_clicked();

    private:
        Ui::IPBanWidget *ui;
};

#endif // IPBANWIDGET_HPP
