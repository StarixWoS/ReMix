#ifndef BANNEDIP_HPP
#define BANNEDIP_HPP

#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QHostAddress>
#include <QtEndian>
#include <QDialog>
#include <QDebug>
#include <QFile>

struct IpBans
{
    unsigned int ip{ 0 };
    QChar reason[ 256 ]{ 0 };
    unsigned int banDate{ 0 };
};

namespace Ui {
    class BannedIP;
}

class BannedIP : public QDialog
{
    Q_OBJECT

    QSortFilterProxyModel* tableProxy{ nullptr };
    QStandardItemModel* tableModel{ nullptr };

    bool newFormat{ false };
    IpBans ipBans[ 400 ];

    public:
        explicit BannedIP(QWidget *parent = 0);
        ~BannedIP();

        void loadBannedIPs();
        void updateFileFormat();

    private slots:
        void on_allowDupedIP_clicked();
        void on_banDupedIP_clicked();
        void on_banHackers_clicked();
        void on_reqSernums_clicked();
        void on_disconnectIdles_clicked();
        void on_allowSSV_clicked();
        void on_removeIPBan_clicked();
        void on_addIPBan_clicked();

    private:
        Ui::BannedIP *ui;
};

#endif // BANNEDIP_HPP
