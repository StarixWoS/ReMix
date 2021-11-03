#ifndef PLRLISTWIDGET_HPP
#define PLRLISTWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QModelIndex>
#include <QWidget>

namespace Ui {
    class PlrListWidget;
}

class PlrListWidget : public QWidget
{
    Q_OBJECT

    static QHash<Server*, PlrListWidget*> plrViewInstanceMap;
    QHash<Player*, QStandardItem*> plrTableItems;

    QSortFilterProxyModel* plrProxy{ nullptr };
    QStandardItemModel* plrModel{ nullptr };
    TblEventFilter* tblEvFilter{ nullptr };
    ReMixWidget* remixWidget{ nullptr };

    QModelIndex prevIndex;

    SendMsg* messageDialog{ nullptr };
    QMenu* contextMenu{ nullptr };
    Player* menuTarget{ nullptr };

    Server* server{ nullptr };

    bool censorUIIPInfo{ false };

    public:
        explicit PlrListWidget(ReMixWidget* parent = nullptr, Server* svr = nullptr);
        ~PlrListWidget() override;

        static PlrListWidget* getInstance(ReMixWidget* parent, Server* server);
        static void deleteInstance(Server* server);

        QStandardItemModel* getPlrModel() const;
        void resizeColumns();

        bool getCensorUIIPInfo() const;

    private:
        void updatePlrView(QStandardItem* object, const qint32& column, const QVariant& data, const qint32& role, const bool& isColor);
        void initContextMenu();

    public slots:
        void updatePlrViewSlot(Player* plr, const qint32& column, const QVariant& data, const qint32& role, const bool& isColor);
        void plrViewInsertRowSlot(Player* plr, const QString& ipPortStr, const QByteArray& data);
        void plrViewRemoveRowSlot(Player* plr);
        void censorUIIPInfoSlot(const bool& state);

    private slots:
        void on_playerView_customContextMenuRequested(const QPoint& pos);
        void on_actionSendMessage_triggered();
        void on_actionMakeAdmin_triggered();
        void on_actionMuteNetwork_triggered();
        void on_actionDisconnectUser_triggered();
        void on_actionBANISHUser_triggered();
        void selectRowSlot(const qint32& row);

    signals:
        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine) const;

    private:
        Ui::PlrListWidget* ui;
};

#endif // PLRLISTWIDGET_HPP
