
#ifndef ADMINSORTMODEL_HPP
#define ADMINSORTMODEL_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QObject>

class UserSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    enum COLS{ SERNUM = 0, SEEN = 1, RANK = 3, BANDATE = 6 };

    public:
        UserSortProxyModel();
        ~UserSortProxyModel();

    protected:
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif // ADMINSORTMODEL_HPP
