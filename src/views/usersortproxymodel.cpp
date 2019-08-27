
//Class includes.
#include "views/usersortproxymodel.hpp"

//ReMix includes.
#include "helper.hpp"
#include "user.hpp"

//Qt Includes.
#include <QModelIndex>
#include <QDateTime>
#include <QVariant>

UserSortProxyModel::UserSortProxyModel()
{

}

UserSortProxyModel::~UserSortProxyModel()
{

}

bool UserSortProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
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

        if ( column == static_cast<int>( UserCols::LastSeen )
          || column == static_cast<int>( UserCols::BanDate )
          || column == static_cast<int>( UserCols::BanDuration ) )
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
        else if ( column == static_cast<int>( UserCols::SerNum ) )
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

        bool res = false;
        if ( Helper::naturalSort( vlStr, vrStr, res ) )
            return res;
    }
    return QSortFilterProxyModel::lessThan( left, right );
}
