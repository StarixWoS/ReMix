
//Class includes.
#include "helper.hpp"

//ReMix includes.
#include "settings.hpp"
#include "randdev.hpp"
#include "logger.hpp"
#include "server.hpp"
#include "user.hpp"

//Qt Includes.
#include <QRegularExpression>
#include <QNetworkInterface>
#include <QInputDialog>
#include <QHostAddress>
#include <QMessageBox>
#include <QTcpSocket>
#include <QtCore>

const QList<qint32> Helper::blueCodedList =
{
    1004, 1024, 1043, 1046, 1052, 1054, 1055, 1062, 1068, 1072, 1099,
    1112, 1120, 1123, 1125, 1138, 1163, 1166, 1170, 1172, 1173, 1189,
    1204, 1210, 1217, 1275, 1292, 1307, 1308, 1312, 1332, 1338, 1367,
    1369, 1370, 1520, 1547, 1551, 1565, 1600, 1607, 1611, 1656, 1675,
    1681, 1695, 1706, 1715, 1751, 1754, 1840, 1965, 2003, 2058, 2062,
    2144, 2205, 2217, 2264, 2268, 2359, 1008, 1017, 1051, 1054, 1082,
    1099, 1104, 1105, 1181, 1199, 1222, 1279, 1343, 1358, 1388, 1456,
    1528, 1677, 1773, 1777, 1778, 1780, 1796, 1799, 2156, 2167, 2241,
    2248, 2362, 2421, 1098, 1220, 1264, 1342, 1361, 1823, 2302, 2488,
    2585, 2372, 1492, 1576, 1100, 1347, 1050, 1015, 1666, 1745, 2043,
    1200, 2628, 1016, 1739, 1853, 2708, 2757, 1498, 2448, 2801, 1031,
    1265, 1414, 1420, 1429, 1214, 1489, 1707, 2543, 1101, 1283, 1604,
    1428, 2707, 1023, 1069, 1071, 1132, 1286, 1854, 2910, 1005, 2682,
    1348, 2615, 2617, 1884, 1169, 1540, 1645, 1939, 1179, 3053, 1803,
    2377, 1000, 1021, 1500, 1501, 1515, 1547, 1803, 2377, 3111, 3202,
    3191, 3149, 3,
};

const QStringList Helper::byteUnits
{
    "B",    // Bytes 1024^0
    "KiB",  // KibiBytes 1024^1
    "MiB",  // MebiBytes 1024^2
    "GiB",  // GibiBytes 1024^3
    "TiB",  // TebiBytes 1024^4
    "PiB",  // PebiBytes 1024^5
    "EiB",  // ExbiBytes 1024^6
};

QInputDialog* Helper::createInputDialog(QWidget* parent, const QString& label, const QInputDialog::InputMode& mode, const int& width, const int& height)
{
    QInputDialog* dialog{ new QInputDialog( parent ) };
                  dialog->setInputMode( mode );
                  dialog->setLabelText( label );
                  dialog->resize( width, height );
    return dialog;
}

qint32 Helper::strToInt(const QString& str, const int& base)
{
    bool base16{ base != static_cast<int>( IntBase::DEC ) };
    bool ok{ false };

    qint32 val{ static_cast<qint32>( str.toUInt( &ok, base ) ) };
    if ( !ok && !base16 )
        val = str.toInt( &ok, static_cast<int>( IntBase::HEX ) );

    if ( !ok )
        val = -1;

    return val;
}

QString Helper::intSToStr(const QString& val, const int& base, const int& fill, QChar filler)
{
    /* base --- What numeric format the string will be in.
     * fill --- How much padding will be prepended to the string.
     * filler --- The char used to pad the string
     */

    int val_i{ val.toInt() };
    QString str{ "%1" };
    if ( val_i > 0 )
        str = str.arg( val_i, fill, base, filler );
    else
        str = str.arg( val.toInt( nullptr, static_cast<int>( IntBase::HEX ) ), fill, base, filler );

    return str.toUpper();
}

QString Helper::getStrStr(const QString& str, const QString& indStr, const QString& mid, const QString& left)
{
    /* indStr --- Sub-string to search for.
     * mid --- Obtain data after this sub-string.
     * left --- Obtain data before this sub-string.
     */

    QString tmp{ "" };
    if ( !str.isEmpty() )
    {
        if ( !indStr.isEmpty() )
        {
            const int index( getStrIndex( str, indStr ) );
            if ( index >= 0 )   //-1 if str didn't contain indStr.
            {
                if ( !mid.isEmpty() )
                    tmp = str.mid( index + indStr.length() );
                else
                    tmp = str.mid( index ); //Get the actual search string.
            }
        }

        if ( !mid.isEmpty()
          || !left.isEmpty() )
        {
            if ( indStr.isEmpty() )
                tmp = str;

            if ( !mid.isEmpty() )
            {
                const int index( getStrIndex( tmp, mid ) );
                if ( index >= 0 )   //-1 if str didn't contain mid.
                {
                    //Append the lookup string's length if it's greater than 1
                    if ( mid.length() >= 1 )
                        tmp = tmp.mid( index + mid.length() );
                    else
                        tmp = tmp.mid( index );
                }
            }

            if ( !left.isEmpty() )
            {
                const int index( getStrIndex( tmp, left ) );
                if ( index >= 0 )   //-1 if str didn't contain left.
                    tmp = tmp.left( index );
            }
        }

        if ( !tmp.isEmpty() )
            return tmp.trimmed();
    }
    return QString();
}

void Helper::stripNewlines(QString& string)
{
    if ( string.contains( "\r\n" ) ) //Replace Unix NewLines with Spaces.
        string = string.replace( "\r\n", " " );
    else if ( string.contains( "\r" ) ) //Replace Carriage Returns with Spaces.
        string = string.replace( "\r", " " );
    else if ( string.contains( "\n" ) ) //Replace NewLines with Spaces.
        string = string.replace( "\n", " " );
}

QString Helper::stripSerNumHeader(const QString& sernum)
{
    QString serNum{ sernum };
    if ( strContainsStr( sernum, "SOUL" ) )
        serNum = serNum.remove( "SOUL", Qt::CaseInsensitive ).trimmed();

    if ( strContainsStr( sernum, "WP" ) )
        serNum = serNum.remove( "WP", Qt::CaseInsensitive ).trimmed();

    return serNum;
}

QString Helper::sanitizeSerNum(const QString& value)
{
    QString sernum{ value };
            sernum = stripSerNumHeader( sernum );

    quint32 sernum_i{ sernum.toUInt( nullptr, static_cast<int>( IntBase::HEX ) ) };
    if ( sernum_i & static_cast<int>( Globals::MIN_HEX_SERNUM ) )
        return value;

    return serNumToHexStr( sernum, 8 );
}

QString Helper::serNumToHexStr(QString sernum, const int& fillAmt)
{
    sernum = stripSerNumHeader( sernum );

    qint32 sernum_i{ static_cast<qint32>( sernum.toUInt( nullptr, static_cast<int>( IntBase::HEX ) ) ) };
    QString result{ "" };

    if ( !( sernum_i & static_cast<int>( Globals::MIN_HEX_SERNUM ) ) )
    {
        bool ok{ false };

        result = intToStr( sernum.toInt( &ok, static_cast<int>( IntBase::DEC ) ), static_cast<int>( IntBase::HEX ), fillAmt );
        if ( !ok )
            result = intToStr( sernum.toInt( &ok, static_cast<int>( IntBase::HEX ) ), static_cast<int>( IntBase::HEX ), fillAmt );
    }
    else
        result = intToStr( sernum_i, static_cast<int>( IntBase::HEX ), fillAmt );

    if ( result.length() > 8 )
        result = result.mid( result.length() - 8 );

    return result;
}

QString Helper::serNumToIntStr(const QString& sernum, const bool& isHex)
{
    QString serNum{ sernum };
    if ( isHex
      && ( serNum.length() > 8 ) )
    {
        serNum = serNum.mid( serNum.length() - 8 );
    }

    qint32 sernum_i{ serNumtoInt( serNum, isHex ) };
    QString retn{ "" };

    if ( !( sernum_i & static_cast<int>( Globals::MIN_HEX_SERNUM ) ) || !isHex )
        retn = QString( "SOUL %1" ).arg( intToStr( sernum_i, static_cast<int>( IntBase::DEC ) ) );
    else
        retn = QString( "%1" ).arg( intToStr( sernum_i, static_cast<int>( IntBase::HEX ) ) );

    if ( !strStartsWithStr( retn, "SOUL" )
      && retn.length() > 8 )
    {
        retn = retn.mid( retn.length() - 8 );
    }
    return retn;
}

qint32 Helper::serNumtoInt(const QString& sernum, const bool& isHex)
{
    QString serNum{ sernum };
            serNum = stripSerNumHeader( sernum );

    bool ok{ false };
    qint32 sernum_i{ static_cast<qint32>( serNum.toUInt( &ok, static_cast<int>( IntBase::HEX ) ) ) };
    if ( !ok )
    {
        if ( isHex )
            sernum_i = strToInt( serNum, static_cast<int>( IntBase::HEX ) );
        else
            sernum_i = strToInt( serNum, static_cast<int>( IntBase::DEC ) );
    }
    return sernum_i;
}

bool Helper::isBlueCodedSerNum(const qint32& sernum)
{
    return blueCodedList.contains( sernum );
}

bool Helper::confirmAction(QWidget* parent, const QString& title, const QString& prompt)
{
    qint32 value{ QMessageBox::question( parent, title, prompt, QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) };
    return value == QMessageBox::Yes;
}

qint32 Helper::warningMessage(QWidget* parent, const QString& title, const QString& prompt)
{
    return QMessageBox::warning( parent, title, prompt, QMessageBox::NoButton, QMessageBox::Ok );
}

QString Helper::getTextResponse(QWidget* parent, const QString& title, const QString& prompt, const QString& defaultInput, bool* ok, const MessageBox& type)
{
    QString response{ "" };
    if ( type == MessageBox::SingleLine )    //Single-line message.
        response = QInputDialog::getText( parent, title, prompt, QLineEdit::Normal, defaultInput, ok );
    else if ( type == MessageBox::MultiLine )   //Multi-line message.
        response = QInputDialog::getMultiLineText( parent, title, prompt, defaultInput, ok );

    return response;
}

QString Helper::getDisconnectReason(QWidget* parent)
{
    static const QString label{ "Disconnect Reason ( Sent to User ):" };
    QInputDialog* dialog{ createInputDialog( parent, label, QInputDialog::TextInput, 355, 170 ) };

    dialog->exec();
    dialog->deleteLater();

    return dialog->textValue();
}

QString Helper::hashPassword(const QString& password)
{
    QCryptographicHash hash( QCryptographicHash::Sha3_512 );
                       hash.addData( password.toLatin1() );

    return QString( hash.result().toHex() );
}

QHostAddress Helper::getPrivateIP()
{
    QList<QHostAddress> ipList{ QNetworkInterface::allAddresses() };

    //Default to our localhost address if nothing valid is found.
    QHostAddress ipAddress{ QHostAddress::Null };
    for ( const QHostAddress& ip : ipList )
    {
        QString tmp{ ip.toString() };

        if ( ip != QHostAddress::LocalHost    //Remove localhost addresses.
          && ip.toIPv4Address()    //Remove any ipv6 addresses.
          && !strStartsWithStr( tmp, "169" ) )    //Remove Windows generated APIPA addresses.
        {
            ipAddress = QHostAddress( ip );    //Use first non-local IP address.
            break;
        }
    }

    //Null ip address was selected, default to '0.0.0.0'.
    if ( ipAddress.isNull() )
        ipAddress = QHostAddress::AnyIPv4;

    return ipAddress;
}

bool Helper::strStartsWithStr(const QString& strA, const QString& strB)
{
    //Returns true if strA starts with strB.
    return strA.startsWith( strB, Qt::CaseInsensitive );
}

bool Helper::strContainsStr(const QString& strA, const QString& strB)
{
    //Returns true if strA contains strB.
    return strA.contains( strB, Qt::CaseInsensitive );
}

bool Helper::cmpStrings(const QString& strA, const QString& strB)
{
    //Returns true if strA is equal to strB.
    return ( strA.compare( strB, Qt::CaseInsensitive ) == 0 );
}

qint32 Helper::getStrIndex(const QString& strA, const QString& strB)
{
    //Returns the position of strB within strA.
    return static_cast<qint32>( strA.indexOf( strB, 0, Qt::CaseInsensitive ) );
}

QString Helper::getTimeAsString(const quint64& time)
{
    quint64 date{ time };
    if ( date == 0 )
        date = static_cast<quint64>( QDateTime::currentDateTimeUtc().toSecsSinceEpoch() );

    return QDateTime::fromSecsSinceEpoch( static_cast<uint>( date ) ).toString( "ddd MMM dd HH:mm:ss yyyy" );
}

QString Helper::getTimeFormat(const qint64& time)
{
    QString format{ "%1d:%2h:%3m:%4s" };
            format = format.arg( getTimeIntFormat( time, TimeFormat::Days ), 2, static_cast<int>( IntBase::DEC ), QChar( '0' ) )
                           .arg( getTimeIntFormat( time, TimeFormat::Hours ), 2, static_cast<int>( IntBase::DEC ), QChar( '0' ) )
                           .arg( getTimeIntFormat( time, TimeFormat::Minutes ), 2, static_cast<int>( IntBase::DEC ), QChar( '0' ) )
                           .arg( getTimeIntFormat( time, TimeFormat::Seconds ), 2, static_cast<int>( IntBase::DEC ), QChar( '0' ) );
    return format;
}

qint64 Helper::getTimeIntFormat(const qint64& time, const TimeFormat& format)
{
    qint64 retn{ time };
    switch ( format )
    {
        case TimeFormat::Days:
            retn = ( ( time / static_cast<int>( TimeFormat::HoursDiv ) )
                     / static_cast<int>( TimeFormat::DaysDiv ) );
        break;
        case TimeFormat::Hours:
            retn = ( ( time / static_cast<int>( TimeFormat::HoursDiv ) )
                     % static_cast<int>( TimeFormat::DaysDiv ) );
        break;
        case TimeFormat::Minutes:
            retn = ( ( time / static_cast<int>( TimeFormat::MinsDiv ) )
                     % static_cast<int>( TimeFormat::SecDiv ) );
        break;
        case TimeFormat::Seconds:
            retn = ( time % static_cast<int>( TimeFormat::SecDiv ) );
        break;
        case TimeFormat::Default:
            retn = time;
        break;
        case TimeFormat::HoursDiv:
        case TimeFormat::DaysDiv:
        case TimeFormat::SecDiv:
            retn = time;
        break;
    }
    return retn;
}

qint32 Helper::sanitizeToFriendlyUnits(const quint64& bytes, QString &retVal, QString& unit)
{
    qreal val{ static_cast<qreal>( bytes ) };
    qint32 iter{ 0 };

    if ( bytes == 0 )
        return false;

    while ( ( val >= 1024 ) && ( iter <= static_cast<int>( ByteUnits::ExbiByte ) ) )
    {
        val /= 1024;
        ++iter;
    }

    retVal = QString::number( val, 'f', sanitizeFriendlyPrecision( static_cast<ByteUnits>( iter ) ) );
    unit = byteUnits.at( iter );
    return true;
}

qint32 Helper::sanitizeFriendlyPrecision(const ByteUnits& unit)
{
    //Return the value of precision required for the ByteUnit passed in.
    switch ( unit )
    {
        case ByteUnits::Byte:
        {
            return 0;
        }
        case ByteUnits::KibiByte:
        case ByteUnits::MebiByte:
        {
            return 1;
        }
        case ByteUnits::GibiByte:
        {
            return 2;
        }
        default:
            return 3;
    }
}
