
//Class includes.
#include "views/plrsortproxymodel.hpp"

//ReMix includes.
#include "helper.hpp"

//Qt Includes.
#include <QModelIndex>
#include <QVariant>

PlrSortProxyModel::PlrSortProxyModel() = default;
PlrSortProxyModel::~PlrSortProxyModel() = default;

bool PlrSortProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    int column{ sortColumn() };
    if ( column >= 0 )
    {
        QVariant vL{ sourceModel()->data( left ) };
        QVariant vR{ sourceModel()->data( right ) };

        if ( !( vL.isValid() && vR.isValid() ))
            return QSortFilterProxyModel::lessThan( left, right );

        Q_ASSERT( vL.isValid( ) );
        Q_ASSERT( vR.isValid( ) );

        QString vlStr{ vL.toString() };
        QString vrStr{ vR.toString() };

        if ( column == COLS::SERNUM )
        {
            if ( !vlStr.contains( "SOUL" ) )
                vlStr = Helper::intSToStr( vlStr, 10 );
            else
                vlStr = vlStr.remove( "SOUL", Qt::CaseInsensitive ).trimmed();

            if ( !vrStr.contains( "SOUL" ) )
                vrStr = Helper::intSToStr( vrStr, 10 );
            else
                vrStr = vrStr.remove( "SOUL", Qt::CaseInsensitive ).trimmed();

            vlStr = Helper::intSToStr( vlStr, 10 );
            vrStr = Helper::intSToStr( vrStr, 10 );
        }
        else if ( column == COLS::AGE
               || column == COLS::TIME )
        {
            vlStr = vlStr.remove( ":" );
            vrStr = vrStr.remove( ":" );
        }

        bool res{ false };
        if ( Helper::naturalSort( vlStr, vrStr, res ) )
            return res;
    }
    return QSortFilterProxyModel::lessThan( left, right );
}
