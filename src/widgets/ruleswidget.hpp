#ifndef RULESWIDGET_HPP
#define RULESWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QWidget>

namespace Ui {
    class RulesWidget;
}

class RulesWidget : public QWidget
{
    Q_OBJECT

    enum Toggles{ world = 0, url = 1, allPK = 2, maxP = 3, maxAFK = 4, minV = 5,
                  ladder = 6, noBleep = 7, noCheat = 8, noEavesdrop = 9,
                  noMigrate = 10, noMod = 11, noPets = 12, noPK = 13,
                  arenaPK = 14 };

    SelectWorld* selectWorld{ nullptr };
    bool maxPlayersCheckState{ false };
    bool minVersionCheckState{ false };
    bool maxAFKCheckState{ false };
    bool worldCheckState{ false };
    bool urlCheckState{ false };
    QString serverID{ "" };

    public:
        explicit RulesWidget(QString svrID = "0");
        ~RulesWidget();

        void setCheckedState(Toggles option, bool val);
        void setSelectedWorld(QString worldName, bool state);

    private:
        void toggleRules(qint32 row, Qt::CheckState value);

    private slots:
        void on_rulesView_doubleClicked(const QModelIndex &index);
        void on_rulesView_itemClicked(QTableWidgetItem *item);

    private:
        Ui::RulesWidget *ui;
};

#endif // RULESWIDGET_HPP
