
//Class includes.
#include "rules.hpp"

//ReMix includes.
#include "settings.hpp"
#include "helper.hpp"

//Qt Includes.
#include <QSettings>
#include <QtCore>

const QStringList Rules::subKeys =
{
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
    "arenaPK"
};

QMutex Rules::mutex;

void Rules::setRule(const QString& key, const QVariant& value, const QString& svrID)
{
    QMutexLocker locker( &mutex );
    bool remove{ false };
    if ( !value.toBool()
      && ( value.toInt() == 0 )
      && ( Helper::cmpStrings( "false", value.toString() )
        || Helper::cmpStrings( "0", value.toString() )
        || value.toString().isEmpty() ) )
    {
        //Keys with a 'disabled' value will be removed from storage
        //instead of being set.
        remove = true;
    }

    if ( !remove )
    {
        Settings::prefs->setValue( svrID % "/" % Settings::keys[ Settings::Rules ] % "/" % key, value );
    }
    else
        removeRule( key, svrID );
}

QVariant Rules::getRule(const QString& key, const QString& svrID)
{
    QMutexLocker locker( &mutex );
    return Settings::prefs->value( svrID % "/" % Settings::keys[ Settings::Rules ] % "/" % key );
}

void Rules::removeRule(const QString& key, const QString& svrID)
{
    QMutexLocker locker( &mutex );
    Settings::prefs->remove( svrID % "/" % Settings::keys[ Settings::Rules ] % "/" % key );
}

QString Rules::getRuleSet(const QString& svrID)
{
    QMutexLocker locker( &mutex );
    Settings::prefs->beginGroup( svrID % "/" % Settings::keys[ Settings::Rules ] );

    QStringList ruleList{ Settings::prefs->allKeys() };
    QString rules{ "" };

    bool valIsBool{ false };

    QString tmpRule{ "" };
    QVariant value;

    for ( int i = 0; i < ruleList.count(); ++i )
    {
        tmpRule = ruleList.at( i );
        value = Settings::prefs->value( tmpRule );

        valIsBool = false;

        if ( Helper::cmpStrings( value.toString(), "true" ) )
            valIsBool = true;

        rules += tmpRule % "=";
        if ( valIsBool )
            rules = rules.append( QString::number( value.toBool() ) );
        else
            rules = rules.append( value.toString() );

        if ( i < ( ruleList.count() - 1 ) )
            rules.append( ", " );
    }
    Settings::prefs->endGroup();
    return rules;
}

bool Rules::getRequireWorld(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::world ], svrID )
              .toString().isEmpty();
}

void Rules::setWorldName(const QString& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::world ], value, svrID );
}

QString Rules::getWorldName(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::world ], svrID )
              .toString();
}

bool Rules::getRequireURL(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::url ], svrID )
              .toString().isEmpty();
}

void Rules::setURLAddress(const QString& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::url ], value, svrID );
}

QString Rules::getURLAddress(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::url ], svrID )
              .toString();
}

void Rules::setAllPKing(const bool& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::allPK ], value, svrID );
}

bool Rules::getAllPKing(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::allPK ], svrID )
              .toBool();
}

bool Rules::getRequireMaxPlayers(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::maxP ], svrID )
              .toUInt() == 0;
}

void Rules::setMaxPlayers(const quint32& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::maxP ], value, svrID );
}

quint32 Rules::getMaxPlayers(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::maxP ], svrID )
              .toUInt();
}

bool Rules::getRequireMaxAFK(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::maxAFK ], svrID )
              .toUInt() == 0;
}

void Rules::setMaxAFK(const quint32& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::maxAFK ], value, svrID );
}

quint32 Rules::getMaxAFK(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::maxAFK ], svrID )
              .toUInt();
}

bool Rules::getRequireMinVersion(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::minV ], svrID)
              .toString().isEmpty();
}

void Rules::setMinVersion(const QString& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::minV ], value, svrID );
}

QString Rules::getMinVersion(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::minV ], svrID )
              .toString();
}

void Rules::setReportLadder(const bool& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::ladder ], value, svrID );
}

bool Rules::getReportLadder(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::ladder ], svrID )
              .toBool();
}

void Rules::setNoCursing(const bool& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::noBleep ], value, svrID );
}

bool Rules::getNoCursing(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::noBleep ], svrID )
              .toBool();
}

void Rules::setNoCheating(const bool& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::noCheat ], value, svrID );
}

bool Rules::getNoCheating(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::noCheat ], svrID )
              .toBool();
}

void Rules::setNoEavesdropping(const bool& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::noEavesdrop ], value, svrID );
}

bool Rules::getNoEavesdropping(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::noEavesdrop ], svrID )
              .toBool();
}

void Rules::setNoMigrating(const bool& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::noMigrate ], value, svrID );
}

bool Rules::getNoMigrating(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::noMigrate ], svrID )
              .toBool();
}

void Rules::setNoModding(const bool& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::noMod ], value, svrID );
}

bool Rules::getNoModding(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::noMod ], svrID )
              .toBool();
}

void Rules::setNoPets(const bool& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::noPets ], value, svrID );
}

bool Rules::getNoPets(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::noPets ], svrID )
              .toBool();
}

void Rules::setNoPKing(const bool& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::noPK ], value, svrID );
}

bool Rules::getNoPKing(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::noPK ], svrID )
              .toBool();
}

void Rules::setArenaPKing(const bool& value, const QString& svrID)
{
    setRule( subKeys[ SubKeys::arenaPK ], value, svrID );
}

bool Rules::getArenaPKing(const QString& svrID)
{
    return getRule( subKeys[ SubKeys::arenaPK ], svrID )
              .toBool();
}
