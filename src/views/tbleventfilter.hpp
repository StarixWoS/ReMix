
#ifndef TBLEVENTFILTER_HPP
#define TBLEVENTFILTER_HPP

#include "prototypes.hpp"

#include <QModelIndex>
#include <QObject>

class TblEventFilter : public QObject
{
    Q_OBJECT

    QTableView* tableView{ nullptr };
    QModelIndex prevIndex;

    public:
        explicit TblEventFilter(QTableView* tbl = nullptr);
        ~TblEventFilter() override;

    signals:

    public slots:
        bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // TBLEVENTFILTER_HPP
