#ifndef SETTINGSWIDGET_HPP
#define SETTINGSWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QWidget>
#include <QMap>

namespace Ui {
    class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

    static SettingsWidget* instance;

    QMap<QTableWidgetItem*, Qt::CheckState> stateMap;

    public:
        explicit SettingsWidget(QWidget* parent = nullptr);
        ~SettingsWidget() override;

        void setCheckedState(const SToggles& option, const bool& val);
        bool getCheckedState(const SToggles& option);

        static SettingsWidget* getInstance(QWidget* parent = nullptr);
        static void setInstance(SettingsWidget* value);

    private:
        void toggleSettingsModel(const qint32 &row);
        void toggleSettings(const qint32& row, Qt::CheckState value);

    private slots:
        void on_settingsView_doubleClicked(const QModelIndex& index);
        void on_settingsView_itemClicked(QTableWidgetItem* item);

    signals:
        void masterMixInfoChangedSignal();
        void masterMixIPChangedSignal();
        void censorUIIPInfoSignal(const bool& state);

    private:
        Ui::SettingsWidget* ui;
};

#endif // SETTINGSWIDGET_HPP
