
#include <QDebug>
#include <QFile>

#include "preferences.hpp"

void Preferences::setSetting(const QString& key, const QString& subKey, QVariant& value)
{
    QSettings setting( "preferences.ini", QSettings::IniFormat );

    if ( key == "General" )
        setting.setValue( subKey, value );
    else
        setting.setValue( key + "/" + subKey, value );
}

QVariant Preferences::getSetting(const QString& key, const QString& subKey)
{
    QSettings setting( "preferences.ini", QSettings::IniFormat );

    if ( key == "General" )
        return setting.value( subKey );

    return setting.value( key + "/" + subKey );
}

void Preferences::setMOTDMessage(QVariant& value)
{
    setSetting( keys[ Keys::General ], subKeys[ SubKeys::MOTD ], value );
}

QString Preferences::getMOTDMessage()
{
    return getSetting( keys[ Keys::General ], subKeys[ SubKeys::MOTD ] ).toString();
}

void Preferences::setBanishMesage(QVariant& value)
{
    setSetting( keys[ Keys::General ], subKeys[ SubKeys::BanishMsg ], value );
}

QString Preferences::getBanishMesage()
{
    return getSetting( keys[ Keys::General ], subKeys[ SubKeys::BanishMsg ] ).toString();
}

void Preferences::setPassword(QVariant& value)
{
    setSetting( keys[ Keys::Options ], subKeys[ SubKeys::Password ], value );
}

QString Preferences::getPassword()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::Password ] ).toString();
}

void Preferences::setRequirePassword(QVariant& value)
{
    setSetting( keys[ Keys::Options ], subKeys[ SubKeys::ReqPassword ], value );
}

bool Preferences::getRequirePassword()
{
    return getSetting( keys[ Keys::Options ], subKeys[ SubKeys::ReqPassword ] ).toBool();
}

void Preferences::setServerRules(QVariant& value)
{
    setSetting( keys[ Keys::General ], subKeys[ SubKeys::Rules ], value );
}

QString Preferences::getServerRules()
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

bool Preferences::isInvalidIPAddress(const QString& value)
{
    return getSetting( keys[ Keys::WrongIP ], value ).toBool();
}
