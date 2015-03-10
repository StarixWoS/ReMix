
#ifndef BANSORTPROXYMODEL_HPP
#define BANSORTPROXYMODEL_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QObject>

class BanSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
        BanSortProxyModel();
        ~BanSortProxyModel();

    protected:
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif // BANSORTPROXYMODEL_HPP
