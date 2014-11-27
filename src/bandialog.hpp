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

    struct IpBans
    {
        quint32 ip{ 0 };
        QChar reason[ 256 ]{ 0 };
        quint32 banDate{ 0 };
    };
    IpBans ipBans[ 400 ];

    public:
        explicit BanDialog(QWidget *parent = 0);
        ~BanDialog();

        //IP-Ban Tab
        void updateFileFormat();
        void loadBannedIPs();

        //SerNum-Ban Tab
        void loadBannedSernums();

        //Check for banned Info.
        bool getIsIPBanned(QHostAddress& ipAddr);
        bool getisSernumBanned(QString& sernum);

        void addIPBan(QHostAddress& ipInfo, QString& reason);
        void addSerNumBan(QString& sernum, QString& reason);

    private slots:
        //IP-Ban Tab Slots
        void on_removeIPBan_clicked();
        void on_addIPBan_clicked();

        void addIPBanImpl(QString& ip, QString& reason);
        void addSernumBanImpl(QString& sernum, QString& reason);

        //SerNum-Ban Tab Slots
        void currentRowChangedSlot(const QModelIndex& index, const QModelIndex&);
        void on_sernumTable_clicked(const QModelIndex &index);
        void on_forgiveButton_clicked();

    private:
        Ui::BanDialog *ui;
};

#endif // BANDIALOG_HPP
