#ifndef SETTINGSWIDGET_HPP
#define SETTINGSWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QWidget>

namespace Ui {
    class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

    enum Toggles{ REQPWD = 0, REQADMINPWD = 1, ALLOWDUPEDIP = 2,
                  BANDUPEDIP = 3, BANHACKERS = 4, REQSERNUM = 5,
                  DISCONNECTIDLES = 6, ALLOWSSV = 7, LOGCOMMENTS = 8,
                  FWDCOMMENTS = 9, ECHOCOMMENTS = 10, INFORMADMINLOGIN = 11,
                  MINIMIZETOTRAY = 12, SAVEWINDOWPOSITIONS = 13, LOGFILES = 14,
                  WORLDDIR = 15 };

    bool pwdCheckState{ false };

    public:
        explicit SettingsWidget(QWidget* parent = nullptr);
        ~SettingsWidget();

        void setCheckedState(Toggles option, bool val);

    private:
        void toggleSettings(quint32 row, Qt::CheckState value);

    private slots:
        void on_settingsView_doubleClicked(const QModelIndex &index);
        void on_settingsView_itemClicked(QTableWidgetItem *item);

    private:
        Ui::SettingsWidget *ui;
};

#endif // SETTINGSWIDGET_HPP
