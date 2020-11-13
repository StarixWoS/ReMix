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
    TblEventFilter* tblEvFilter{ nullptr };

    QModelIndex prevIndex;

    SendMsg* messageDialog{ nullptr };
    QMenu* contextMenu{ nullptr };
    Player* menuTarget{ nullptr };

    ServerInfo* server{ nullptr };

    public:
        explicit PlrListWidget(QWidget* parent = nullptr, ServerInfo* svr = nullptr);
        ~PlrListWidget() override;

        QStandardItemModel* getPlrModel() const;
        void resizeColumns();

    private:
        void initContextMenu();

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
