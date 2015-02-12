
#ifndef PLRSORTMODEL_HPP
#define PLRSORTMODEL_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QObject>

class PlrSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    enum COLS{ IPADDR = 0, SERNUM = 1, AGE = 2, TIME = 3};

    public:
        PlrSortProxyModel();
        ~PlrSortProxyModel();

    protected:
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif // PLRSORTMODEL_HPP
