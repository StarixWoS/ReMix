
//Class includes.
#include "rules.hpp"

//ReMix includes.
#include "settings.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QSettings>
#include <QtCore>

const QStringList Rules::ruleKeys =
{
    "svrPassword",
    "world",
    "url",
    "allPK",
    "maxP",
    "maxAFK",
    "minV",
    "ladder",
    "noBleep",
    "noCheat",
    "noEavesdrop",
    "noMigrate",
    "noMod",
    "noPets",
    "noPK",
    "arenaPK",
};

QMutex Rules::mutex;

QString Rules::makeRulePath(const QString& serverName, const RuleKeys& key)
{
    QString path{ "%1/%2/%3" };
            path = path.arg( serverName )
                       .arg( Settings::pKeys[ static_cast<int>( SettingKeys::Rules ) ] )
                       .arg( ruleKeys[ static_cast<int>( key ) ] );
    return path;
}

void Rules::removeRule(const QString& path)
{
    Settings::prefs->sync();
    Settings::prefs->remove( path );
}

bool Rules::canRemoveRule(const QVariant& value)
{
    bool remove{ false };
    if ( !value.toBool()
      && ( value.toInt() == 0 )
      && ( Helper::cmpStrings( "false", value.toString() )
        || Helper::cmpStrings( "0", value.toString() )
        || value.toString().isEmpty() ) )
    {
        remove = true;   //Keys with a 'disabled' value will be removed from storage.
    }
    return remove;
}

void Rules::setRule(const QString& serverName, const QVariant& value, const RuleKeys& key)
{
    QMutexLocker locker( &mutex );
    QVariant val{ value };

    QString path{ makeRulePath( serverName, key ) };
    if ( !canRemoveRule( value ) )
    {
        if ( RuleKeys::SvrPassword == key )    //Hash the Server Password.
        {
            QString pwd{ value.toString() };
            if ( pwd.isEmpty() )
                val = Helper::hashPassword( pwd );
        }
        Settings::prefs->setValue( path, val );
    }
    else
        removeRule( path );

    Settings::prefs->sync();
}

QVariant Rules::getRule(const QString& serverName, const RuleKeys& key)
{
    QMutexLocker locker( &mutex );
    Settings::prefs->sync();
    return Settings::prefs->value( makeRulePath( serverName, key ) );
}

QString Rules::getRuleSet(const QString& svrID)
{
    QMutexLocker locker( &mutex );
    Settings::prefs->sync();
    Settings::prefs->beginGroup( svrID % "/" % Settings::pKeys[ static_cast<int>( SettingKeys::Rules ) ] );

    QStringList ruleList{ Settings::prefs->allKeys() };
    QString rules{ "" };

    bool valIsBool{ false };

    QVariant value;
    for ( const QString& rule : ruleList )
    {
        value = Settings::prefs->value( rule );

        valIsBool = false;

        if ( Helper::cmpStrings( value.toString(), "true" ) )
            valIsBool = true;

        if ( !Helper::cmpStrings( rule, "svrPassword" ) )
        {
            rules += rule % "=";
            if ( valIsBool )
                rules = rules.append( QString::number( value.toBool() ) );
            else
                rules = rules.append( value.toString() );

            rules.append( ", " );
        }
    }

    Settings::prefs->endGroup();
    return rules;
}

bool Rules::cmpServerPassword(const QString& serverName, const QString& value)
{
    QString hash{ value };
    if ( !hash.isEmpty() )
        return ( getRule( serverName, RuleKeys::SvrPassword ).toString() == Helper::hashPassword( hash ) );

    return false;
}
