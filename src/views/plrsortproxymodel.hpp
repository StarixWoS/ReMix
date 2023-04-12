
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

    public:
        PlrSortProxyModel();
        ~PlrSortProxyModel() override;

    protected:
        bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
};

#endif // PLRSORTMODEL_HPP
