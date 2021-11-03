#ifndef RULESWIDGET_HPP
#define RULESWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QWidget>
#include <QMutex>
#include <QMap>

namespace Ui {
    class RulesWidget;
}

class RulesWidget : public QWidget
{
    Q_OBJECT

    static QHash<Server*, RulesWidget*> ruleWidgets;
    QMap<QTableWidgetItem*, Qt::CheckState> stateMap;

    QString serverName{ "" };
    QString gameInfo{ "" };

    public:
        explicit RulesWidget();
        ~RulesWidget() override;

        static RulesWidget* getInstance(Server* server);
        static void deleteInstance(Server* server);

        void setServerName(const QString& name);
        void setCheckedState(const RToggles& option, const bool& val);
        bool getCheckedState(const RToggles& option);

        void setGameInfo(const QString& gInfo);
        const QString& getGameInfo() const;

    private:
        void toggleRulesModel(const RToggles& row);
        void toggleRules(const qint32& row, const Qt::CheckState& value);

    private slots:
        void on_rulesView_doubleClicked(const QModelIndex& index);
        void on_rulesView_itemClicked(QTableWidgetItem *item);

    signals:
        void gameInfoChangedSignal(const QString& gameInfo);
        void setMaxIdleTimeSignal();
        void setMaxPlayersSignal(const qint32& maxPlayers);

    public slots:
        void nameChangedSlot(const QString& name);

    private:
        Ui::RulesWidget* ui;
        static QMutex mutex;
};

#endif // RULESWIDGET_HPP
