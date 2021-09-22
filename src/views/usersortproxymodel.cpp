
//Class includes.
#include "views/usersortproxymodel.hpp"

//ReMix includes.
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

        if ( !( vL.isValid() && vR.isValid() ))
            return QSortFilterProxyModel::lessThan( left, right );

        Q_ASSERT( vL.isValid( ) );
        Q_ASSERT( vR.isValid( ) );

        QString vlStr{ vL.toString() };
        QString vrStr{ vR.toString() };

        if ( column == static_cast<int>( UserCols::LastSeen )
          || column == static_cast<int>( UserCols::BanDate )
          || column == static_cast<int>( UserCols::BanDuration )
          || column == static_cast<int>( UserCols::MuteDate )
          || column == static_cast<int>( UserCols::MuteDuration ) )
        {
            vlStr = QString::number( QDateTime::fromString( vlStr, "ddd MMM dd HH:mm:ss yyyy" ).toSecsSinceEpoch() );

            vrStr = QString::number( QDateTime::fromString( vrStr, "ddd MMM dd HH:mm:ss yyyy" )
                                                    .toSecsSinceEpoch() );
        }
        else if ( column == static_cast<int>( UserCols::SerNum ) )
        {
            if ( !vlStr.contains( "SOUL" ) )
                vlStr = Helper::intSToStr( vlStr, static_cast<int>( IntBase::DEC ) );
            else
                vlStr = vlStr.remove( "SOUL", Qt::CaseInsensitive ).trimmed();

            if ( !vrStr.contains( "SOUL" ) )
                vrStr = Helper::intSToStr( vrStr, static_cast<int>( IntBase::DEC ) );
            else
                vrStr = vrStr.remove( "SOUL", Qt::CaseInsensitive ).trimmed();

            vlStr = Helper::intSToStr( vlStr, static_cast<int>( IntBase::DEC ) );
            vrStr = Helper::intSToStr( vrStr, static_cast<int>( IntBase::DEC ) );
        }

        bool res{ false };
        if ( Helper::naturalSort( vlStr, vrStr, res ) )
            return res;
    }
    return QSortFilterProxyModel::lessThan( left, right );
}
