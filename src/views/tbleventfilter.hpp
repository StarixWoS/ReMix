
#ifndef TBLEVENTFILTER_HPP
#define TBLEVENTFILTER_HPP

#include "prototypes.hpp"

#include <QModelIndex>
#include <QObject>
#include <QHash>

class TblEventFilter : public QObject
{
    Q_OBJECT

    static QHash<TblEventFilter*, QTableView*> tableMap;

    QTableView* tableView{ nullptr };
    QModelIndex prevIndex;

    public:
        explicit TblEventFilter(QTableView* tbl = nullptr);
        ~TblEventFilter() override;

    static TblEventFilter* getInstance(QTableView* tbl);
    static void deleteInstance(QTableView* tbl);

    public slots:
        bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // TBLEVENTFILTER_HPP
