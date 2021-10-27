
#include "sortutil.hpp"

#include <QThreadStorage>

SortUtil::NaturalCompare::NaturalCompare( const Qt::CaseSensitivity caseSensitivity )
    : sensitivity( caseSensitivity )
{

}

int SortUtil::NaturalCompare::compare( const QString& left, const QString& right ) const
{
    int posL = 0;
    int posR = 0;
    while ( true )
    {
        if ( ( posL == left.size() ) || ( posR == right.size() ) )
            return static_cast<int>( left.size() - right.size() );

        const QChar leftChar = ( sensitivity == Qt::CaseSensitive ) ? left[ posL ] : left[ posL ].toLower();
        const QChar rightChar = ( sensitivity == Qt::CaseSensitive ) ? right[ posR ] : right[ posR ].toLower();

        if ( ( leftChar == rightChar ) && !leftChar.isDigit() )
        {
            ++posL;
            ++posR;
        }
        else if ( leftChar.isDigit() && rightChar.isDigit() )
        {
            const auto intView = []( const QString &str, int &pos )
            {
                const int start = pos;
                while ( ( pos < str.size() ) && str[ pos ].isDigit() )
                {
                    ++pos;
                }
                return str.mid( start, ( pos - start ) );
            };

            const QString numViewL = intView( left, posL );
            const QString numViewR = intView( right, posR );

            if ( numViewL.length() != numViewR.length() )
                return static_cast<int>( numViewL.length() - numViewR.length() );

            for ( int i = 0; i < numViewL.length(); ++i )
            {
                const QChar numL = numViewL[ i ];
                const QChar numR = numViewR[ i ];

                if (numL != numR)
                    return ( numL.unicode() - numR.unicode() );
            }
        }
        else
            return ( leftChar.unicode() - rightChar.unicode() );
    }
}

int SortUtil::naturalCompare(const QString &left, const QString &right, const Qt::CaseSensitivity& caseSensitivity)
{
    if ( caseSensitivity == Qt::CaseSensitive )
    {
        static QThreadStorage<SortUtil::NaturalCompare> natCompare;
        if ( !natCompare.hasLocalData() )
            natCompare.setLocalData( SortUtil::NaturalCompare( Qt::CaseSensitive ) );

        return natCompare.localData().compare( left, right );
    }
    else
    {
        static QThreadStorage<SortUtil::NaturalCompare> natCompare;
        if ( !natCompare.hasLocalData() )
            natCompare.setLocalData( SortUtil::NaturalCompare( Qt::CaseInsensitive ) );

        return natCompare.localData().compare( left, right );
    }
}

bool SortUtil::naturalLessThan(const QString& left, const QString& right, const Qt::CaseSensitivity& caseSensitivity)
{
    return ( naturalCompare( left, right, caseSensitivity ) < 0 );
}
