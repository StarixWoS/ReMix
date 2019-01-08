
#ifndef USERSORTMODEL_HPP
#define USERSORTMODEL_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QObject>

class UserSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
        UserSortProxyModel();
        ~UserSortProxyModel();

    protected:
        bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
};

#endif // USERSORTMODEL_HPP
