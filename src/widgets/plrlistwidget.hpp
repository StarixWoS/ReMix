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

    QSortFilterProxyModel* plrProxy{ nullptr };
    QStandardItemModel* plrModel{ nullptr };

    QModelIndex prevIndex;

    QMenu* contextMenu{ nullptr };
    Player* menuTarget{ nullptr };

    ServerInfo* server{ nullptr };
    Admin* admin{ nullptr };

    public:
        explicit PlrListWidget(QWidget *parent = nullptr,
                               ServerInfo* svr = nullptr,
                               Admin* adm = nullptr);
        ~PlrListWidget();

        QStandardItemModel* getPlrModel();
        void resizeColumns();

    private:
        void initContextMenu();

    private slots:
        void on_playerView_customContextMenuRequested(const QPoint &pos);

        void on_actionSendMessage_triggered();
        void on_actionMakeAdmin_triggered();
        void on_actionMuteNetwork_triggered();
        void on_actionDisconnectUser_triggered();
        void on_actionBANISHUser_triggered();

    private:
        Ui::PlrListWidget *ui;
};

#endif // PLRLISTWIDGET_HPP
