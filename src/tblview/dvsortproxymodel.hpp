
#ifndef DVSORTPROXYMODEL_HPP
#define DVSORTPROXYMODEL_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QObject>

class DVSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    enum COLS{ DEVICE = 0, BANDATE = 3 };

    public:
        DVSortProxyModel();
        ~DVSortProxyModel();

    protected:
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif // DVSORTPROXYMODEL_HPP
