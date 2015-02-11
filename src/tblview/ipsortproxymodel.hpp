#ifndef IPBANMODEL_HPP
#define IPBANMODEL_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QObject>

class IPSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    enum COLS{ IPADDR = 0, BANDATE = 2 };

    public:
        IPSortProxyModel();
        ~IPSortProxyModel();

    protected:
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif // IPBANMODEL_HPP
