
//Class includes.
#include "views/plrsortproxymodel.hpp"

//ReMix includes.
#include "sortutil.hpp"
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

        if ( !( vL.isValid() || vR.isValid() ) )
            return QSortFilterProxyModel::lessThan( left, right );

        QString vlStr{ vL.toString() };
        QString vrStr{ vR.toString() };

        if ( column == COLS::SERNUM )
        {
            if ( vlStr.contains( "SOUL" ) )
                vlStr = vlStr.remove( "SOUL", Qt::CaseInsensitive ).trimmed();

            if ( vrStr.contains( "SOUL" ) )
                vrStr = vrStr.remove( "SOUL", Qt::CaseInsensitive ).trimmed();

            vlStr = Helper::intSToStr( vlStr, static_cast<int>( IntBase::DEC ) );
            vrStr = Helper::intSToStr( vrStr, static_cast<int>( IntBase::DEC ) );
        }
        else if ( column == COLS::AGE
               || column == COLS::TIME )
        {
            vlStr = vlStr.remove( ":" );
            vrStr = vrStr.remove( ":" );
        }
        return SortUtil::naturalLessThan( vlStr, vrStr, Qt::CaseInsensitive );
    }
    return QSortFilterProxyModel::lessThan( left, right );
}
