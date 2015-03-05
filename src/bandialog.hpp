
#ifndef BANDIALOG_HPP
#define BANDIALOG_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QDialog>

namespace Ui {
    class BanDialog;
}

class BanDialog : public QDialog
{
    Q_OBJECT

    DABanWidget* daBanWidget{ nullptr };
    DVBanWidget* dvBanWidget{ nullptr };
    IPBanWidget* ipBanWidget{ nullptr };
    SNBanWidget* snBanWidget{ nullptr };

    public:
        explicit BanDialog(QWidget *parent = 0);
        ~BanDialog();

        //IP-Ban Tab
        void remoteAddIPBan(Player* admin, Player* target,
                            QString& reason);

        void addIPBan(QString ip, QString& reason);
        void removeIPBan(QString ip);

        //SerNum-Ban Tab
        void remoteAddSerNumBan(Player* admin, Player* target,
                                           QString& reason);

        void addSerNumBan(QString& sernum, QString& reason);
        void removeSerNumBan(QString& sernum);

        void remoteAddDVBan(Player* admin, Player* target, QString& reason);
        void addDVBan(QString& dVar, QString& reason);
        void removeDVBan(QString& dVar);

        void remoteAddDABan(Player* admin, Player* target, QString& reason);
        void addDABan(QString& dVar, QString& reason);
        void removeDABan(QString& dVar);

    private:
        Ui::BanDialog *ui;
};

#endif // BANDIALOG_HPP
