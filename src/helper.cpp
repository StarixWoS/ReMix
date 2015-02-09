
#include "includes.hpp"
#include "helper.hpp"

//Initialize Settings keys/subKeys lists
const QString Helper::keys[ 3 ] =
{
    "options", "wrongIPs", "General"
};

const QString Helper::subKeys[ 14 ] =
{
    "extension", "myPassword", "autoBanish", "discIdle", "requireSernum",
    "dupeOK", "serverSupportsVariables", "banishDupes", "requirePassword",
    "MOTD", "BANISHED", "RULES", "requireAdminAuth", "logComments"
};

QInputDialog* Helper::createInputDialog(QWidget* parent, QString& label,
                                        QInputDialog::InputMode mode,
                                        int width, quint32 height)
{
    QInputDialog* dialog = new QInputDialog( parent );
                  dialog->setInputMode( mode );
                  dialog->setLabelText( label );
                  dialog->resize( width, height );
    return dialog;
}

QString Helper::intSToStr(QString val, int base, int fill, QChar filler)
{
    /* This overload is mainly used to reformat a QString's numeric format
     * if the source is in an unknown format.
     *
     * base --- What numeric format the string will be in.
     * fill --- How much padding will be prepended to the string.
     * filler --- The char used to pad the string
     */

    int val_i = val.toInt();
    if ( val_i > 0 )
        return QString( "%1" ).arg( val_i, fill, base, filler ).toUpper();
    else
        return QString( "%1" ).arg( val.toInt( 0, 16 ), fill, base, filler )
                   .toUpper();
}

int Helper::strToInt(QString str, int base)
{
    bool base16 = ( base != 10 );
    bool ok{ false };

    int val = str.toInt( &ok, base );
    if ( !ok && !base16 )
        val = str.toInt( &ok, 16 );

    if ( !ok )
        val = -1;

    return val;
}

QString Helper::getStrStr(const QString& str, QString indStr,
                          QString mid, QString left)
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
            index = str.indexOf( indStr, 0, Qt::CaseInsensitive );
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
                index = tmp.indexOf( mid, 0, Qt::CaseInsensitive );
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
                index = tmp.indexOf( left, 0, Qt::CaseInsensitive );
                if ( index >= 0 )   //-1 if str didn't contain left.
                    tmp = tmp.left( index );
            }
        }

        if ( !tmp.isEmpty() )
            return tmp.trimmed();
    }
    return QString();
}

QString Helper::serNumToHexStr(QString sernum, int fillAmt)
{
    qint32 sernum_i{ sernum.toInt( 0, 16 ) };

    QString result{ "" };
    if ( sernum.contains( "SOUL" ) )
    {
        result = intToStr( getStrStr( sernum, "SOUL", "SOUL", "" )
                                 .toInt(), 16, fillAmt );
    }
    else
    {
        if ( sernum_i & MIN_HEX_SERNUM )
            result = intToStr( sernum_i, 16, fillAmt );
        else
            result = intToStr( sernum.toInt( 0, 10 ), 16, fillAmt );
    }

    //Remove Spaces.
    return result.trimmed();
}

QString Helper::serNumToIntStr(QString sernum)
{
    int sernum_i{ sernum.toInt( 0, 16 ) };
    if ( !( sernum_i & MIN_HEX_SERNUM ) )
        return QString( "SOUL %1" ).arg( intToStr( sernum_i, 10 ) );
    else
        return QString( "%1" ).arg( intToStr( sernum_i, 16 ) );
}

qint32 Helper::serNumtoInt(QString& sernum)
{
    int sernum_i{ 0 };
    if ( sernum.contains( "SOUL" ) )
        sernum_i = strToInt( getStrStr( sernum, "SOUL", " ", "" ), 10 );
    else
        sernum_i = strToInt( sernum, 16 );

    return sernum_i;
}

void Helper::logToFile(QString& file, QString& text, bool timeStamp,
                       bool newLine)
{
    QFile log( file );
    QFileInfo logInfo( log );
    if ( !logInfo.dir().exists() )
        logInfo.dir().mkdir( "." );

    if ( log.open( QFile::WriteOnly | QFile::Append ) )
    {
        if ( timeStamp )
        {
            quint64 date = QDateTime::currentDateTime().toTime_t();
            text.prepend( QDateTime::fromTime_t( date )
                               .toString( "[ ddd MMM dd HH:mm:ss yyyy ] " ) );
        }

        if ( newLine )
            text.prepend( "\r\n" );

        log.write( text.toLatin1() );

        log.close();
    }
}

bool Helper::confirmAction(QWidget* parent, QString& title, QString& prompt)
{
    qint32 value = QMessageBox::question( parent, title, prompt,
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::No );
    return value == QMessageBox::Yes;
}

qint32 Helper::warningMessage(QWidget* parent, QString& title, QString& prompt )
{
    return QMessageBox::warning( parent, title, prompt,
                                 QMessageBox::NoButton,
                                 QMessageBox::Ok );
}

QString Helper::getTextResponse(QWidget* parent, QString& title,
                                QString& prompt, bool* ok, int type)
{
    QString response{ "" };
    if ( type == 0 )    //Single-line message.
    {
        response = QInputDialog::getText( parent, title, prompt,
                                          QLineEdit::Normal,
                                          "", ok );
    }
    else if ( type == 1 )   //Multi-line message.
    {
        response = QInputDialog::getMultiLineText( parent, title, prompt,
                                                   "", ok );
    }
    return response;
}

void Helper::setSetting(const QString& key, const QString& subKey,
                        QVariant& value)
{
    QSettings setting( "preferences.ini", QSettings::IniFormat );

    if ( key == QLatin1String( "General" ) )
        setting.setValue( subKey, value );
    else
        setting.setValue( key % "/" % subKey, value );
}

QVariant Helper::getSetting(const QString& key, const QString& subKey)
{
    QSettings setting( "preferences.ini", QSettings::IniFormat );

    if ( key == QLatin1String( "General" ) )
        return setting.value( subKey );

    return setting.value( key % "/" % subKey );
}

void Helper::setMOTDMessage(QVariant& value)
{
    setSetting( keys[ Keys::General ],
                subKeys[ SubKeys::MOTD ], value );
}

QString Helper::getMOTDMessage()
{
    return getSetting( keys[ Keys::General ], subKeys[ SubKeys::MOTD ] )
              .toString();
}

void Helper::setBanishMesage(QVariant& value)
{
    setSetting( keys[ Keys::General ],
                subKeys[ SubKeys::BanishMsg ], value );
}

QString Helper::getBanishMesage()
{
    return getSetting( keys[ Keys::General ], subKeys[ SubKeys::BanishMsg ] )
              .toString();
}

QString Helper::getBanishReason(QWidget* parent)
{
    QString label{ "Ban Reason ( Sent to User ):" };
    QInputDialog* dialog{
        createInputDialog( parent, label,
                           QInputDialog::TextInput,
                           355, 170 ) };

    dialog->exec();
    dialog->deleteLater();

    return dialog->textValue();
}

QString Helper::getDisconnectReason(QWidget* parent)
{
    QString label{ "Disconnect Reason ( Sent to User ):" };
    QInputDialog* dialog{
        createInputDialog( parent, label,
                           QInputDialog::TextInput,
                           355, 170 ) };

    dialog->exec();
    dialog->deleteLater();

    return dialog->textValue();
}

void Helper::setPassword(QString& value)
{
    QVariant pwd{ value };

    //Convert the password to a SHA3_512 hash.
    if ( !value.isEmpty() )
    {
        pwd = hashPassword( pwd );
    }
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::Password ], pwd );
}

QString Helper::getPassword()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::Password ] )
              .toString();
}

void Helper::setRequirePassword(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::ReqPassword ], value );
}

bool Helper::getRequirePassword()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::ReqPassword ] )
              .toBool();
}

bool Helper::cmpServerPassword(QVariant& value)
{
    //Convert the password to a SHA3_512 hash.
    if ( !value.toString().isEmpty() )
        value = hashPassword( value );

    return ( getPassword() == value.toString() );
}

QString Helper::hashPassword(QVariant& password)
{
    QCryptographicHash hash( QCryptographicHash::Sha3_512 );
                       hash.addData( password.toString().toLatin1() );

    return QString( hash.result().toHex() );
}

QString Helper::genPwdSalt(RandDev* randGen, qint32 length)
{
    bool newRNG{ false };
    if ( randGen == nullptr )
    {
        randGen = new RandDev();
        newRNG = true;
    }

    QString salt{ "" };
    QString charList
    {
        "0123456789"
        "`~!@#$%^&*-_=+{([])}|;:'\"\\,./?<>"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
    };

    qint32 chrPos{ -1 };
    while ( salt.length() < length )
    {
        chrPos = randGen->genRandNum( 0, charList.length() - 1 );
        salt.append( charList.at( chrPos ) );
    }

    if ( newRNG )
        delete randGen;

    return salt;
}

void Helper::setServerRules(QVariant& value)
{
    setSetting( keys[ Keys::General ],
                subKeys[ SubKeys::Rules ], value );
}

QString Helper::getServerRules()
{
    QVariant pending = getSetting( keys[ Keys::General ],
                                   subKeys[ SubKeys::Rules ] );

    QString rules;
    if ( pending.type() == QVariant::StringList )
    {
        QStringList ruleList = pending.toStringList();
        for ( int i = 0; i < ruleList.count(); ++i )
        {
            if ( i > 0 )
                rules.append( ", " );

            rules.append( ruleList.at( i ) );
        }
    }
    else if ( pending.type() == QVariant::String )
        rules = pending.toString();

    return rules;
}

void Helper::setAllowDupedIP(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::AllowDupe ], value );
}

bool Helper::getAllowDupedIP()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::AllowDupe ] )
              .toBool();
}

void Helper::setBanDupedIP(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::BanDupes ], value );
}

bool Helper::getBanDupedIP()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::BanDupes ] )
              .toBool();
}

void Helper::setBanHackers(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::AutoBan ], value );
}

bool Helper::getBanHackers()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::AutoBan ] )
              .toBool();
}

void Helper::setReqSernums(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::ReqSernum ], value );
}

bool Helper::getReqSernums()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::ReqSernum ] )
              .toBool();
}

void Helper::setDisconnectIdles(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::AllowIdle ], value );
}

bool Helper::getDisconnectIdles()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::AllowIdle ] )
              .toBool();
}

void Helper::setAllowSSV(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::AllowSSV ], value );
}

bool Helper::getAllowSSV()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::AllowSSV ] )
              .toBool();
}

void Helper::logBIOData(QString& serNum, QHostAddress& ip,
                        quint16 port, QString& bio)
{
    QString sernum{ serNum };
    if ( sernum.contains( "SOUL", Qt::CaseInsensitive ) )
        sernum = Helper::serNumToHexStr( serNum, 8 );

    QString ipAddr{ ip.toString() % ":" % Helper::intToStr( port, 10, 0 ) };
    qint64 date{ QDateTime::currentDateTime().toTime_t() };

    QSettings bioData( "userInfo.ini", QSettings::IniFormat );
              bioData.setValue( sernum % "/bioInfo", bio.mid( 1 ) );
              bioData.setValue( sernum % "/ipInfo", ipAddr );
              bioData.setValue( sernum % "/date", QDateTime::fromTime_t( date )
                                                       .toString( "ddd MMM dd"
                                                                  " HH:mm:ss "
                                                                  "yyyy" ));
}

void Helper::setLogComments(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::LogComments ], value );
}

bool Helper::getLogComments()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::LogComments ] )
              .toBool();
}

void Helper::setServerID(QVariant& value)
{
    setSetting( keys[ Keys::Options ],
                subKeys[ SubKeys::Extension ], value );
}

int Helper::getServerID()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::Extension ] )
              .toInt();
}

bool Helper::isInvalidIPAddress(const QString& value)
{
    return getSetting( keys[ Keys::WrongIP ], value ).toBool();
}
