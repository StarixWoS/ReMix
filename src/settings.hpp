
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <QMessageBox>
#include <QDialog>

namespace Ui {
    class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

    public:
        explicit Settings(QWidget *parent = 0);
        ~Settings();

        enum Options{ ReqPwd = 0, ReqAdminPwd = 1, AllowDupIP = 2, BanDupIP = 3, BanHack = 4, ReqSernum = 5,
                      DisconnectIdle = 6, AllowSSV = 7, LogComments = 8 };

        void setCheckedState(Options option, bool val);

    private slots:
        void on_settingsView_doubleClicked(const QModelIndex &index);

    private:
        Ui::Settings *ui;
};

#endif // SETTINGS_HPP
