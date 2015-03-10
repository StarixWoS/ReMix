
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

    BanWidget* banWidget{ nullptr };

    public:
        explicit BanDialog(QWidget *parent = 0);
        ~BanDialog();

        void addBan(Player* plr, QString& reason);
        void removeBan(QString& value, qint32 type);
        void remoteAddBan(Player* admin, Player* target, QString& reason);

        static bool getIsBanned(QString value);

    private:
        Ui::BanDialog *ui;
};

#endif // BANDIALOG_HPP
