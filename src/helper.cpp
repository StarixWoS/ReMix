
//Class includes.
#include "helper.hpp"

//ReMix includes.
#include "serverinfo.hpp"
#include "settings.hpp"
#include "randdev.hpp"
#include "logger.hpp"
#include "user.hpp"

//Qt Includes.
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

QInputDialog* Helper::createInputDialog(QWidget* parent, QString& label,
                                        QInputDialog::InputMode mode,
                                        int width, int height)
{
    QInputDialog* dialog = new QInputDialog( parent );
                  dialog->setInputMode( mode );
                  dialog->setLabelText( label );
                  dialog->resize( width, height );
    return dialog;
}

qint32 Helper::strToInt(const QString& str, const int& base)
{
    bool base16 = ( base != 10 );
    bool ok{ false };

    qint32 val = str.toInt( &ok, base );
    if ( !ok && !base16 )
        val = str.toInt( &ok, 16 );

    if ( !ok )
        val = -1;

    return val;
}

QString Helper::intSToStr(QString& val, int base, int fill, QChar filler)
{
    /* This overload is mainly used to reformat a QString's numeric format
     * if the source is in an unknown format.
     *
     * base --- What numeric format the string will be in.
     * fill --- How much padding will be prepended to the string.
     * filler --- The char used to pad the string
     */

    int val_i = val.toInt();
    QString str{ "%1" };
    if ( val_i > 0 )
        str = str.arg( val_i, fill, base, filler );
    else
        str = str.arg( val.toInt( nullptr, 16 ), fill, base, filler );

    return str.toUpper();
}

QString Helper::getStrStr(const QString& str, const QString& indStr,
                          const QString& mid, const QString& left)
{
    /* Search an input string and return a sub-string based on the input strings.
     * indStr --- Sub-string to search for.
     * mid --- Obtain data after this sub-string.
     * left --- Obtain data before this sub-string.
     */

    QString tmp{ "" };
    int index{ 0 };

    if ( !str.isEmpty() )
    {
        if ( !indStr.isEmpty() )
        {
            index = getStrIndex( str,indStr );
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
                index = getStrIndex( tmp, mid );
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
                index = getStrIndex( tmp, left );
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

void Helper::stripSerNumHeader(QString& sernum)
{
    if ( strContainsStr( sernum, "SOUL" ) )
    {
        sernum = sernum.remove( "SOUL", Qt::CaseInsensitive )
                       .trimmed();
    }

    if ( strContainsStr( sernum, "WP" ) )
    {
        sernum = sernum.remove( "WP", Qt::CaseInsensitive )
                       .trimmed();
    }
}

QString Helper::sanitizeSerNum(const QString& value)
{
    QString sernum{ value };
    stripSerNumHeader( sernum );

    quint32 sernum_i{ sernum.toUInt( nullptr, 16 ) };
    if ( sernum_i & MIN_HEX_SERNUM )
        return value;

    return serNumToHexStr( sernum, 8 );
}

QString Helper::serNumToHexStr(QString sernum, int fillAmt)
{
    stripSerNumHeader( sernum );

    quint32 sernum_i{ sernum.toUInt( nullptr, 16 ) };
    QString result{ "" };

    if ( !( sernum_i & MIN_HEX_SERNUM ) )
    {
        bool ok{ false };
        sernum.toUInt( &ok, 10 );

        if ( !ok )
        {
            result = intToStr( sernum.toUInt( &ok, 16 ), 16, fillAmt );
            if ( !ok )
                result = intToStr( sernum.toInt( &ok, 16 ), 16, fillAmt );
        }
        else
        {
            result = intToStr( sernum.toUInt( &ok, 10 ), 16, fillAmt );
            if ( !ok )
                result = intToStr( sernum.toInt( &ok, 10 ), 16, fillAmt );
        }
    }
    else
        result = intToStr( sernum_i, 16, fillAmt );

    if ( result.length() > 8 )
        result = result.mid( result.length() - 8 );

    return result;
}

QString Helper::serNumToIntStr(const QString& sernum)
{
    quint32 sernum_i{ sernum.toUInt( nullptr, 16 ) };
    QString retn{ "" };

    if ( !( sernum_i & MIN_HEX_SERNUM ) )
        retn = QString( "SOUL %1" ).arg( intToStr( sernum_i, 10 ) );
    else
        retn = QString( "%1" ).arg( intToStr( sernum_i, 16 ) );

    if ( !strStartsWithStr( retn, "SOUL" )
      && retn.length() > 8 )
    {
        retn = retn.mid( retn.length() - 8 );
    }
    return retn;
}

qint32 Helper::serNumtoInt(QString& sernum)
{
    stripSerNumHeader( sernum );

    qint32 sernum_i{ sernum.toInt( nullptr, 16 ) };
    if ( sernum_i & MIN_HEX_SERNUM )
        sernum_i = strToInt( sernum, 16 );
    else
        sernum_i = strToInt( sernum, 10 );

    return sernum_i;
}

bool Helper::isBlueCodedSerNum(const quint32& sernum)
{
    return blueCodedList.contains( static_cast<int>( sernum ) );
}

bool Helper::confirmAction(QWidget* parent, QString& title, QString& prompt)
{
    qint32 value = QMessageBox::question( parent, title, prompt, QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
    return value == QMessageBox::Yes;
}

qint32 Helper::warningMessage(QWidget* parent, const QString& title,
                              const QString& prompt)
{
    return QMessageBox::warning( parent, title, prompt, QMessageBox::NoButton, QMessageBox::Ok );
}

QString Helper::getTextResponse(QWidget* parent, const QString& title,
                                const QString& prompt,
                                const QString& defaultInput,
                                bool* ok, int type)
{
    QString response{ "" };
    if ( type == 0 )    //Single-line message.
    {
        response = QInputDialog::getText( parent, title, prompt, QLineEdit::Normal, defaultInput, ok );
    }
    else if ( type == 1 )   //Multi-line message.
    {
        response = QInputDialog::getMultiLineText( parent, title, prompt, defaultInput, ok );
    }
    return response;
}

QString Helper::getDisconnectReason(QWidget* parent)
{
    QString label{ "Disconnect Reason ( Sent to User ):" };
    QInputDialog* dialog{ createInputDialog( parent, label, QInputDialog::TextInput, 355, 170 ) };

    dialog->exec();
    dialog->deleteLater();

    return dialog->textValue();
}

QString Helper::hashPassword(QString& password)
{
    QCryptographicHash hash( QCryptographicHash::Sha3_512 );
                       hash.addData( password.toLatin1() );

    return QString( hash.result().toHex() );
}

QString Helper::genPwdSalt(const qint32& length)
{
    RandDev* randGen{ RandDev::getDevice() };

    QString salt{ "" };
    QString charList
    {
        "0123456789 `~!@#$%^&*-_=+{([])}|;:'\"\\,./?<>"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz"
    };

    qint32 chrPos{ -1 };
    while ( salt.length() < length )
    {
        chrPos = randGen->genRandNum( 0, charList.length() - 1 );
        salt.append( charList.at( chrPos ) );
    }

    if ( !validateSalt( salt ) )
        salt = genPwdSalt( length );

    return salt;
}

bool Helper::validateSalt(QString& salt)
{
    QSettings* userData = User::getUserData();
    QStringList groups = userData->childGroups();
    QString j{ "" };

    for ( int i = 0; i < groups.count(); ++i )
    {
        j = userData->value( groups.at( i ) % "/salt" ).toString();

        if ( j == salt )
            return false;
    }
    return true;
}

bool Helper::naturalSort(QString& left, QString& right, bool& result)
{
    do
    {
        int posL = left.indexOf( QRegExp( "[0-9]" ) );
        int posR = right.indexOf( QRegExp( "[0-9]" ) );
        if ( posL == -1 || posR == -1 )
            break;

        if ( posL != posR )
            break;

        if ( left.left( posL ) != right.left( posR ) )
            break;

        QString temp;
        while ( posL < left.size( ) )
        {
            if ( left.at( posL ).isDigit( ) )
                temp += left.at( posL );
            else
                break;
            posL++;
        }
        int numL = temp.toInt( );
        temp.clear( );

        while ( posR < right.size( ) )
        {
            if ( right.at( posR ).isDigit( ) )
                temp += right.at( posR );
            else
                break;
            posR++;
        }
        int numR = temp.toInt( );

        if ( numL != numR )
        {
            result = ( numL < numR );
            return true;
        }
        left.remove( 0, posL );
        right.remove( 0, posR );

    } while ( true );
    return false;
}

void Helper::delay(const qint32& time)
{
    //Delay the next Port refresh by /time/ seconds.
    QTime delayedTime = QTime::currentTime().addSecs( time );
    while ( QTime::currentTime() < delayedTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

QHostAddress Helper::getPrivateIP()
{
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();

    //Default to our localhost address if nothing valid is found.
    QHostAddress ipAddress{ QHostAddress::Null };
    for ( const auto& ip : ipList )
    {
        QString tmp = ip.toString();
        //Remove localhost addresses.
        if ( ip != QHostAddress::LocalHost
          //Remove any ipv6 addresses.
          && ip.toIPv4Address()
          //Remove any addresses the User manually marked invalid.
          && !Settings::getIsInvalidIPAddress( tmp )
          //Remove Windows generated APIPA addresses.
          && !strStartsWithStr( tmp, "169" ) )
        {
            //Use first non-local IP address.
            ipAddress = QHostAddress( ip );
            break;
        }
    }

    //Null ip address was selected, default to '0.0.0.0'.
    if ( ipAddress.isNull() )
        ipAddress = QHostAddress::AnyIPv4;

    return ipAddress;
}

void Helper::getSynRealData(ServerInfo* svr)
{
    if ( svr == nullptr )
        return;

    QString message{ "Fetching Master Info from [ %1 ]." };
            message = message.arg( svr->getMasterInfoHost() );
    Logger::getInstance()->insertLog( svr->getServerName(), message, LogTypes::USAGE, true, true );

    QFileInfo synRealFile( "synReal.ini" );

    bool downloadFile = true;
    if ( synRealFile.exists() )
    {
        qint64 curTime = static_cast<qint64>( QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000 );
        qint64 modTime = static_cast<qint64>( synRealFile.lastModified().toMSecsSinceEpoch() / 1000 );

        //Check if the file is 48 hours old and set our bool.
        downloadFile = ( curTime - modTime >= 172800 );
    }

    //The file was older than 48 hours or did not exist. Request a fresh copy.
    if ( downloadFile )
    {
        auto* socket = new QTcpSocket;
        QUrl url( svr->getMasterInfoHost() );

        socket->connectToHost( url.host(), 80 );
        QObject::connect( socket, &QTcpSocket::connected, socket,
        [=]()
        {
            socket->write( QString( "GET %1\r\n" )
                               .arg( svr->getMasterInfoHost() ).toLatin1() );
        }, Qt::QueuedConnection );

        QObject::connect( socket, &QTcpSocket::readyRead, socket,
        [=]()
        {
            QFile synreal( "synReal.ini" );
            if ( synreal.open( QIODevice::WriteOnly ) )
            {
                socket->waitForReadyRead();
                synreal.write( socket->readAll() );
            }

            synreal.close();

            QSettings settings( "synReal.ini", QSettings::IniFormat );
            QString str = settings.value( svr->getGameName() % "/master" ).toString();
            int index = str.indexOf( ":" );
            if ( index > 0 )
            {
                svr->setMasterIP( str.left( index ) );
                svr->setMasterPort( static_cast<quint16>( str.midRef( index + 1 ).toInt() ) );

                QString msg{ "Got Master Server [ %1:%2 ] for Game [ %3 ]." };
                        msg = msg.arg( svr->getMasterIP() )
                                 .arg( svr->getMasterPort() )
                                 .arg( svr->getGameName() );
                Logger::getInstance()->insertLog( svr->getServerName(), msg, LogTypes::USAGE, true, true );
            }
        }, Qt::QueuedConnection );

        QObject::connect( socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater, Qt::QueuedConnection );
    }
    else
    {
        QSettings settings( "synReal.ini", QSettings::IniFormat );
        QString str = settings.value( svr->getGameName() % "/master" ).toString();
        if ( !str.isEmpty() )
        {
            int index = str.indexOf( ":" );
            if ( index > 0 )
            {
                svr->setMasterIP( str.left( index ) );
                svr->setMasterPort(
                            static_cast<quint16>(
                                str.midRef( index + 1 ).toInt() ) );

                message = "Got Master Server [ %1:%2 ] for Game [ %3 ].";
                message = message.arg( svr->getMasterIP() )
                                 .arg( svr->getMasterPort() )
                                 .arg( svr->getGameName() );
                Logger::getInstance()->insertLog( svr->getServerName(), message, LogTypes::USAGE, true, true );
            }
        }
    }
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
    return strA.indexOf( strB, 0, Qt::CaseInsensitive );
}

QString Helper::getTimeAsString(const quint64& time)
{
    quint64 date{ time };
    if ( date == 0 )
        date = QDateTime::currentDateTimeUtc().toTime_t();

    return QDateTime::fromTime_t( static_cast<uint>( date ) )
            .toString( "ddd MMM dd HH:mm:ss yyyy" );
}

QString Helper::getTimeFormat(const quint64& time)
{
    return QString( "%1:%2:%3" )
            .arg( getTimeIntFormat( time, TimeFormat::Hours ), 2, 10, QChar( '0' ) )
            .arg( getTimeIntFormat( time, TimeFormat::Minutes ), 2, 10, QChar( '0' ) )
            .arg( getTimeIntFormat( time, TimeFormat::Seconds ), 2, 10, QChar( '0' ) );
}

quint64 Helper::getTimeIntFormat(const quint64& time, const TimeFormat& format)
{
    quint64 retn{ time };
    switch ( format )
    {
        case TimeFormat::Hours:
            retn = ( time / static_cast<int>( TimeFormat::HoursDiv ) );
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
        case TimeFormat::SecDiv:
        case TimeFormat::HoursDiv:
            retn = time;
        break;
    }
    return retn;
}
