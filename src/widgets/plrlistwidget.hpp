#ifndef PLRLISTWIDGET_HPP
#define PLRLISTWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QModelIndex>
#include <QWidget>
#include <QIcon>

namespace Ui {
    class PlrListWidget;
}

class PlrListWidget : public QWidget
{
    Q_OBJECT

    static QHash<QSharedPointer<Server>, PlrListWidget*> plrViewInstanceMap;
    QHash<QSharedPointer<Player>, QStandardItem*> plrTableItems;

    QSortFilterProxyModel* plrProxy{ nullptr };
    QStandardItemModel* plrModel{ nullptr };
    TblEventFilter* tblEvFilter{ nullptr };
    ReMixWidget* remixWidget{ nullptr };

    QModelIndex prevIndex;

    SendMsg* messageDialog{ nullptr };
    QMenu* contextMenu{ nullptr };
    QSharedPointer<Player> menuTarget{ nullptr };

    QSharedPointer<Server> server;

    QIcon afkIcon{ ":/icon/AFK.png" };
    QIcon npkIcon{ ":/icon/NPK.png" };

    bool censorUIIPInfo{ false };

    public:
        explicit PlrListWidget(QSharedPointer<Server> svr, ReMixWidget* parent = nullptr);
        ~PlrListWidget() override;

        static PlrListWidget* getInstance(ReMixWidget* parent, QSharedPointer<Server> server);
        static void deleteInstance(QSharedPointer<Server> server);

        QStandardItemModel* getPlrModel() const;
        void resizeColumns();

        bool getCensorUIIPInfo() const;

    private:
        void updatePlrView(QStandardItem* object, const qint32& column, const QVariant& data, const qint32& role, const bool& isColor);
        void initContextMenu();

    public slots:
        void updatePlrViewSlot(QSharedPointer<Player> plr, const qint32& column, const QVariant& data, const qint32& role, const bool& isColor);
        void plrViewInsertRowSlot(QSharedPointer<Player> plr, const QString& ipPortStr, const QByteArray& data);
        void plrViewRemoveRowSlot(QSharedPointer<Player> plr);
        void censorUIIPInfoSlot(const bool& state);

    private slots:
        void on_playerView_customContextMenuRequested(const QPoint& pos);
        void on_actionSendMessage_triggered();
        void on_actionMakeAdmin_triggered();
        void on_actionMuteNetwork_triggered();
        void on_actionDisconnectUser_triggered();
        void on_actionBANISHUser_triggered();
        void selectRowSlot(const qint32& row);
        void themeChangedSlot();
        void forwardMessageSlot(const QString& message);

    signals:
        void insertLogSignal(const QString& source, const QString& message, const LKeys& type, const bool& logToFile, const bool& newLine) const;
        void insertMasterMessageSignal(const QString& message, QSharedPointer<Player> target, const bool& toAll);

    private:
        Ui::PlrListWidget* ui;
};

#endif // PLRLISTWIDGET_HPP
