
#ifndef LOGGERSORTMODEL_HPP
#define LOGGERSORTMODEL_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QObject>

class LoggerSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
        LoggerSortProxyModel();
        ~LoggerSortProxyModel();

    protected:
        bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
};

#endif // LOGGERSORTMODEL_HPP
