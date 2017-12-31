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
                  DCBLUECODEDSERNUMS = 6, DISCONNECTIDLES = 7, ALLOWSSV = 8,
                  LOGCOMMENTS = 9, FWDCOMMENTS = 10, ECHOCOMMENTS = 11,
                  INFORMADMINLOGIN = 12, MINIMIZETOTRAY = 13,
                  SAVEWINDOWPOSITIONS = 14, LOGFILES = 15, WORLDDIR = 16, };

    bool pwdCheckState{ false };

    public:
        explicit SettingsWidget(QWidget* parent = nullptr);
        ~SettingsWidget();

        void setCheckedState(const Toggles& option, const bool& val);

    private:
        void toggleSettings(const qint32& row, Qt::CheckState value);

    private slots:
        void on_settingsView_doubleClicked(const QModelIndex& index);
        void on_settingsView_itemClicked(QTableWidgetItem* item);

    private:
        Ui::SettingsWidget* ui;
};

#endif // SETTINGSWIDGET_HPP
