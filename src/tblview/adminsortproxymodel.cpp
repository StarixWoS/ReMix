
#include "includes.hpp"

AdminSortProxyModel::AdminSortProxyModel()
{

}

AdminSortProxyModel::~AdminSortProxyModel()
{

}

bool AdminSortProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    int column = sortColumn();
    if ( column == COLS::SERNUM )
    {
        QVariant vL = sourceModel()->data( left );
        QVariant vR = sourceModel()->data( right );

        if ( !( vL.isValid() && vR.isValid() ))
            return QSortFilterProxyModel::lessThan( left, right );

        Q_ASSERT( vL.isValid( ) );
        Q_ASSERT( vR.isValid( ) );

        QString vlStr{ vL.toString() };
        if ( !vlStr.contains( "SOUL" ) )
            vlStr = Helper::intSToStr( vlStr, 10 );
        else
            vlStr = vlStr.remove( "SOUL", Qt::CaseInsensitive ).trimmed();

        QString vrStr{ vR.toString() };
        if ( !vrStr.contains( "SOUL" ) )
            vrStr = Helper::intSToStr( vrStr, 10 );
        else
            vrStr = vrStr.remove( "SOUL", Qt::CaseInsensitive ).trimmed();

        vlStr = Helper::intSToStr( vlStr, 10 );
        vrStr = Helper::intSToStr( vrStr, 10 );

        bool res = false;
        if ( Helper::naturalSort( vlStr, vrStr, res ) )
            return res;
    }
    return QSortFilterProxyModel::lessThan( left, right );
}

