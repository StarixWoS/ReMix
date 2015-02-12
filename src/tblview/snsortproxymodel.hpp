
#ifndef SNSORTMODEL_HPP
#define SNSORTMODEL_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QObject>

class SNSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    enum COLS{ SERNUM = 0, BANDATE = 2 };

    public:
        SNSortProxyModel();
        ~SNSortProxyModel();

    protected:
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif // SNSORTMODEL_HPP
