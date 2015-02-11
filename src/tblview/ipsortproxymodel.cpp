
#include "includes.hpp"

IPSortProxyModel::IPSortProxyModel()
{

}

IPSortProxyModel::~IPSortProxyModel()
{

}

bool IPSortProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    int column = sortColumn();
    if ( column >= 0 )
    {
        QVariant vL = sourceModel()->data( left );
        QVariant vR = sourceModel()->data( right );

        if ( !( vL.isValid() && vR.isValid() ))
            return QSortFilterProxyModel::lessThan( left, right );

        Q_ASSERT( vL.isValid( ) );
        Q_ASSERT( vR.isValid( ) );

        QString vlStr{ vL.toString() };
        QString vrStr{ vR.toString() };
        if ( column == COLS::BANDATE )
        {
            vlStr = QString::number(
                        QDateTime::fromString( vlStr,
                                               "ddd MMM dd HH:mm:ss yyyy" )
                             .toTime_t() );

            vrStr = QString::number(
                        QDateTime::fromString( vrStr,
                                               "ddd MMM dd HH:mm:ss yyyy" )
                             .toTime_t() );
        }

        bool res = false;
        if ( Helper::naturalSort( vlStr, vrStr, res ) )
            return res;
    }
    return QSortFilterProxyModel::lessThan( left, right );
}

