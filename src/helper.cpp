
#include "helper.hpp"

namespace RandDev
{
    std::mt19937 randDevice( QDateTime::currentMSecsSinceEpoch() );
}

int Helper::genRandNum(int min, int max)
{
    std::uniform_int_distribution<int> randInt( min, max );
    return randInt( RandDev::randDevice );
}

void Helper::setSetting(const QString& key, const QString& subKey, QVariant& value)
{
    QSettings setting( "preferences.ini", QSettings::IniFormat );

    if ( key == "General" )
        setting.setValue( subKey, value );
    else
        setting.setValue( key + "/" + subKey, value );
}

QVariant Helper::getSetting(const QString& key, const QString& subKey)
{
    QSettings setting( "preferences.ini", QSettings::IniFormat );

    if ( key == "General" )
        return setting.value( subKey );

    return setting.value( key + "/" + subKey );
}

void Helper::setMOTDMessage(QVariant& value)
{
    setSetting( keys[ Keys::General ], subKeys[ SubKeys::MOTD ], value );
}

QString Helper::getMOTDMessage()
{
    return getSetting( keys[ Keys::General ], subKeys[ SubKeys::MOTD ] ).toString();
}

void Helper::setBanishMesage(QVariant& value)
{
    setSetting( keys[ Keys::General ], subKeys[ SubKeys::BanishMsg ], value );
}

QString Helper::getBanishMesage()
{
    return getSetting( keys[ Keys::General ], subKeys[ SubKeys::BanishMsg ] ).toString();
}

void Helper::setPassword(QVariant& value)
{
    //Convert the password to a SHA3_512 hash.
    if ( !value.toString().isEmpty() )
        value = QString( QCryptographicHash::hash( value.toString().toLatin1(), QCryptographicHash::Sha3_512 ).toHex() );

    setSetting( keys[ Keys::Options ], subKeys[ SubKeys::Password ], value );
}

QString Helper::getPassword()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::Password ] ).toString();
}

void Helper::setRequirePassword(QVariant& value)
{
    setSetting( keys[ Keys::Options ], subKeys[ SubKeys::ReqPassword ], value );
}

bool Helper::getRequirePassword()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::ReqPassword ] ).toBool();
}

bool Helper::cmpPassword(QString& value)
{
    //Convert the password to a SHA3_512 hash.
    if ( !value.isEmpty() )
        value = QString( QCryptographicHash::hash( value.toLatin1(), QCryptographicHash::Sha3_512 ).toHex() );

    return ( getPassword() == value );
}

void Helper::setServerRules(QVariant& value)
{
    setSetting( keys[ Keys::General ], subKeys[ SubKeys::Rules ], value );
}

QString Helper::getServerRules()
{
    QVariant pending = getSetting( keys[ Keys::General ], subKeys[ SubKeys::Rules ] );

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
    setSetting( keys[ Keys::Options ], subKeys[ SubKeys::AllowDupe ], value );
}

bool Helper::getAllowDupedIP()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::AllowDupe ] ).toBool();
}

void Helper::setBanDupedIP(QVariant& value)
{
    setSetting( keys[ Keys::Options ], subKeys[ SubKeys::BanDupes ], value );
}

bool Helper::getBanDupedIP()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::BanDupes ] ).toBool();
}

void Helper::setBanHackers(QVariant& value)
{
    setSetting( keys[ Keys::Options ], subKeys[ SubKeys::AutoBan ], value );
}

bool Helper::getBanHackers()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::AutoBan ] ).toBool();
}

void Helper::setReqSernums(QVariant& value)
{
    setSetting( keys[ Keys::Options ], subKeys[ SubKeys::ReqSernum ], value );
}

bool Helper::getReqSernums()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::ReqSernum ] ).toBool();
}

void Helper::setDisconnectIdles(QVariant& value)
{
    setSetting( keys[ Keys::Options ], subKeys[ SubKeys::AllowIdle ], value );
}

bool Helper::getDisconnectIdles()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::AllowIdle ] ).toBool();
}

void Helper::setAllowSSV(QVariant& value)
{
    setSetting( keys[ Keys::Options ], subKeys[ SubKeys::AllowSSV ], value );
}

bool Helper::getAllowSSV()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::AllowSSV ] ).toBool();
}

void Helper::setServerID(QVariant& value)
{
    setSetting( keys[ Keys::Options ], subKeys[ SubKeys::Extension ], value );
}

int Helper::getServerID()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::Extension ] ).toInt();
}

bool Helper::isInvalidIPAddress(const QString& value)
{
    return getSetting( keys[ Keys::WrongIP ], value ).toBool();
}
