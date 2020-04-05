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

    enum Toggles{ /*REQPWD = 0, REQADMINPWD,*/ ALLOWDUPEDIP = 0,
                  BANDUPEDIP,/* BANHACKERS,*/ REQSERNUM,
                  DCBLUECODEDSERNUMS, DISCONNECTIDLES, ALLOWSSV,
                  LOGCOMMENTS, FWDCOMMENTS, ECHOCOMMENTS,
                  INFORMADMINLOGIN, MINIMIZETOTRAY,
                  SAVEWINDOWPOSITIONS, LOGFILES, WORLDDIR = 13, };

    bool pwdCheckState{ false };

    public:
        explicit SettingsWidget(QWidget* parent = nullptr);
        ~SettingsWidget() override;

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
