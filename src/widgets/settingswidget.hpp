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

//    enum Toggles{ /*REQPWD = 0, REQADMINPWD,*/ ALLOWDUPEDIP = 0,
//                  BANDUPEDIP,/* BANHACKERS,*/ REQSERNUM,
//                  DCBLUECODEDSERNUMS, DISCONNECTIDLES, ALLOWSSV,
//                  LOGCOMMENTS, FWDCOMMENTS, ECHOCOMMENTS,
//                  INFORMADMINLOGIN, MINIMIZETOTRAY,
//                  SAVEWINDOWPOSITIONS, LOGFILES, WORLDDIR = 13, };

    bool masterAddrCheckState{ false };
    bool worldCheckState{ false };
    bool pwdCheckState{ false };

    public:
        explicit SettingsWidget(QWidget* parent = nullptr);
        ~SettingsWidget() override;

        void setCheckedState(const SToggles& option, const bool& val);
        bool getCheckedState(const SToggles& option);

    private:
        void toggleSettingsModel(const qint32 &row);
        void toggleSettings(const qint32& row, Qt::CheckState value);

    private slots:
        void on_settingsView_doubleClicked(const QModelIndex& index);
        void on_settingsView_itemClicked(QTableWidgetItem* item);

    private:
        Ui::SettingsWidget* ui;
};

#endif // SETTINGSWIDGET_HPP
