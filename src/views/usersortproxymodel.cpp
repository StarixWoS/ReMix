
//Class includes.
#include "views/usersortproxymodel.hpp"

//ReMix includes.
#include "sortutil.hpp"
#include "helper.hpp"
#include "user.hpp"

//Qt Includes.
#include <QModelIndex>
#include <QDateTime>
#include <QVariant>

UserSortProxyModel::UserSortProxyModel()= default;
UserSortProxyModel::~UserSortProxyModel() = default;

bool UserSortProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
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

        if ( column == static_cast<int>( UserCols::LastSeen )
          || column == static_cast<int>( UserCols::BanDate )
          || column == static_cast<int>( UserCols::BanDuration )
          || column == static_cast<int>( UserCols::MuteDate )
          || column == static_cast<int>( UserCols::MuteDuration ) )
        {
            //Pull Date out of the UserRole. This is faster than converting the date string back to a qint64.
            QVariant valLeft{ sourceModel()->data( left, Qt::UserRole ) };
            QVariant valRight{ sourceModel()->data( right, Qt::UserRole ) };

            return valLeft.toLongLong() >= valRight.toLongLong();
        }
        else if ( column == static_cast<int>( UserCols::SerNum ) )
        {
            if ( vlStr.contains( "SOUL" ) )
                vlStr = vlStr.remove( "SOUL", Qt::CaseInsensitive ).trimmed();

            if ( vrStr.contains( "SOUL" ) )
                vrStr = vrStr.remove( "SOUL", Qt::CaseInsensitive ).trimmed();

            vlStr = Helper::intSToStr( vlStr, static_cast<int>( IntBase::DEC ) );
            vrStr = Helper::intSToStr( vrStr, static_cast<int>( IntBase::DEC ) );
        }
        return SortUtil::naturalLessThan( vlStr, vrStr, Qt::CaseInsensitive );
    }
    return QSortFilterProxyModel::lessThan( left, right );
}
