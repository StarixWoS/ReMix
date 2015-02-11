
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

        void addIPBan(QHostAddress& ipInfo, QString& reason);
        void addIPBan(QString ip, QString& reason);

        void removeIPBan(QString& ip);
        void removeIPBan(QHostAddress& ipInfo);

        bool getIsIPBanned(QHostAddress& ipAddr);
        bool getIsIPBanned(QString ipAddr);

        //SerNum-Ban Tab
        bool getIsSernumBanned(QString sernum);

        void remoteAddSerNumBan(Player* admin, Player* target,
                                           QString& reason);

        void addSerNumBan(QString& sernum, QString& reason);

        void removeSerNumBan(QString& sernum);

    private:
        Ui::BanDialog *ui;
};

#endif // BANDIALOG_HPP
