
#ifndef ADMINSORTMODEL_HPP
#define ADMINSORTMODEL_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QObject>

class AdminSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    enum COLS{ SERNUM = 0 };

    public:
        AdminSortProxyModel();
        ~AdminSortProxyModel();

    protected:
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif // ADMINSORTMODEL_HPP
