#ifndef SORTUTIL_HPP
#define SORTUTIL_HPP

#include <QString>

namespace SortUtil
{
    class NaturalCompare
    {
        const Qt::CaseSensitivity sensitivity;

        public:
            explicit NaturalCompare(const Qt::CaseSensitivity caseSensitivity = Qt::CaseSensitive);

            int compare( const QString &left, const QString &right ) const;
    };

    int naturalCompare(const QString& left, const QString& right, const Qt::CaseSensitivity& caseSensitivity);
    bool naturalLessThan(const QString& left, const QString& right, const Qt::CaseSensitivity& caseSensitivity);
}

#endif // SORTUTIL_HPP
