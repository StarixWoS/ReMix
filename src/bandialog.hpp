#ifndef BANDIALOG_HPP
#define BANDIALOG_HPP

#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QHostAddress>
#include <QtEndian>
#include <QDialog>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QChar>

namespace Ui {
    class BanDialog;
}

class BanDialog : public QDialog
{
    Q_OBJECT

    QSortFilterProxyModel* ipProxy{ nullptr };
    QStandardItemModel* ipModel{ nullptr };

    QSortFilterProxyModel* snProxy{ nullptr };
    QStandardItemModel* snModel{ nullptr };

    bool newFormat{ false };

    public:
        explicit BanDialog(QWidget *parent = 0);
        ~BanDialog();

        //IP-Ban Tab
        void loadBannedIPs();
        bool getIsIPBanned(QHostAddress& ipAddr);
        bool getIsIPBanned(QString ipAddr);

        void addIPBan(QHostAddress& ipInfo, QString& reason);
        void addIPBan(QString& ip, QString& reason);

        //SerNum-Ban Tab
        void loadBannedSernums();
        bool getIsSernumBanned(QString sernum);
        void addSerNumBan(QString& sernum, QString& reason);

    private:
        void addIPBanImpl(QString& ip, QString& reason);
        void addSernumBanImpl(QString& sernum, QString& reason);

    private slots:
        //IP-Ban Tab Slots
        void on_addIPBan_clicked();
        void on_ipBanTable_clicked(const QModelIndex &index);
        void ipBanTableChangedRowSlot(const QModelIndex &index, const QModelIndex&);
        void on_removeIPBan_clicked();

        //SerNum-Ban Tab Slots
        void on_addSernumBan_clicked();
        void on_snBanTable_clicked(const QModelIndex &index);
        void snBanTableChangedRowSlot(const QModelIndex &index, const QModelIndex&);
        void on_forgiveButton_clicked();

    private:
        Ui::BanDialog *ui;
};

#endif // BANDIALOG_HPP
