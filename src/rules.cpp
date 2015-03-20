
#include "includes.hpp"
#include "rules.hpp"

const QString Rules::subKeys[ RULES_SUBKEY_COUNT ] =
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

void Rules::setRule(const QString& key, QVariant& value)
{
    Settings::prefs->setValue( Settings::keys[ Settings::Rules ] % "/" % key,
                               value );
}

QVariant Rules::getRule(const QString& key)
{
    return Settings::prefs->value( Settings::keys[ Settings::Rules ]
                                 % "/" % key );
}

QString Rules::getRuleSet()
{
    Settings::prefs->beginGroup( Settings::keys[ Settings::Rules ] );

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

        if ( value.toString().compare( "true", Qt::CaseInsensitive ) == 0 )
            valIsBool = true;

        rules += tmpRule % "=";
        if ( valIsBool )
            rules = rules.append( QString::number( value.toBool() ) );
        else
            rules = rules.append( value.toString() );

        rules.append( ", " );
    }
    Settings::prefs->endGroup();
    return rules;
}

bool Rules::getRequireWorld()
{
    return getRule( subKeys[ SubKeys::world ] ).toString().isEmpty();
}

void Rules::setWorldName(QString& value)
{
    QVariant data{ value };
    setRule( subKeys[ SubKeys::world ], data );
}

QString Rules::getWorldName()
{
    return getRule( subKeys[ SubKeys::world ] ).toString();
}

bool Rules::getRequireURL()
{
    return getRule( subKeys[ SubKeys::url ] ).toString().isEmpty();
}

void Rules::setURLAddress(QString& value)
{
    QVariant data{ value };
    setRule( subKeys[ SubKeys::url ], data );
}

QString Rules::getURLAddress()
{
    return getRule( subKeys[ SubKeys::url ] ).toString();
}

void Rules::setAllPKing(QVariant& value)
{
    setRule( subKeys[ SubKeys::allPK ], value );
}

bool Rules::getAllPKing()
{
    return getRule( subKeys[ SubKeys::allPK ] ).toBool();
}

bool Rules::getRequireMaxPlayers()
{
    return getRule( subKeys[ SubKeys::maxP ] ).toUInt() == 0;
}

void Rules::setMaxPlayers(quint32 value)
{
    QVariant data{ value };
    setRule( subKeys[ SubKeys::maxP ], data );
}

quint32 Rules::getMaxPlayers()
{
    return getRule( subKeys[ SubKeys::maxP ] ).toUInt();
}

bool Rules::getRequireMaxAFK()
{
    return getRule( subKeys[ SubKeys::maxAFK ] ).toUInt() == 0;
}

void Rules::setMaxAFK(quint32 value)
{
    QVariant data{ value };
    setRule( subKeys[ SubKeys::maxAFK ], data );
}

quint32 Rules::getMaxAFK()
{
    return getRule( subKeys[ SubKeys::maxAFK ] ).toUInt();
}

bool Rules::getRequireMinVersion()
{
    return getRule( subKeys[ SubKeys::minV ] ).toString().isEmpty();
}

void Rules::setMinVersion(QString& value)
{
    QVariant data{ value };
    setRule( subKeys[ SubKeys::minV ], data );
}

QString Rules::getMinVersion()
{
    return getRule( subKeys[ SubKeys::minV ] ).toString();
}

void Rules::setReportLadder(QVariant& value)
{
    setRule( subKeys[ SubKeys::ladder ], value );
}

bool Rules::getReportLadder()
{
    return getRule( subKeys[ SubKeys::ladder ] ).toBool();
}

void Rules::setNoCursing(QVariant& value)
{
    setRule( subKeys[ SubKeys::noBleep ], value );
}

bool Rules::getNoCursing()
{
    return getRule( subKeys[ SubKeys::noBleep ] ).toBool();
}

void Rules::setNoCheating(QVariant& value)
{
    setRule( subKeys[ SubKeys::noCheat ], value );
}

bool Rules::getNoCheating()
{
    return getRule( subKeys[ SubKeys::noCheat ] ).toBool();
}

void Rules::setNoEavesdropping(QVariant& value)
{
    setRule( subKeys[ SubKeys::noEavesdrop ], value );
}

bool Rules::getNoEavesdropping()
{
    return getRule( subKeys[ SubKeys::noEavesdrop ] ).toBool();
}

void Rules::setNoMigrating(QVariant& value)
{
    setRule( subKeys[ SubKeys::noMigrate ], value );
}

bool Rules::getNoMigrating()
{
    return getRule( subKeys[ SubKeys::noMigrate ] ).toBool();
}

void Rules::setNoModding(QVariant& value)
{
    setRule( subKeys[ SubKeys::noMod ], value );
}

bool Rules::getNoModding()
{
    return getRule( subKeys[ SubKeys::noMod ] ).toBool();
}

void Rules::setNoPets(QVariant& value)
{
    setRule( subKeys[ SubKeys::noPets ], value );
}

bool Rules::getNoPets()
{
    return getRule( subKeys[ SubKeys::noPets ] ).toBool();
}

void Rules::setNoPKing(QVariant& value)
{
    setRule( subKeys[ SubKeys::noPK ], value );
}

bool Rules::getNoPKing()
{
    return getRule( subKeys[ SubKeys::noPK ] ).toBool();
}

void Rules::setArenaPKing(QVariant& value)
{
    setRule( subKeys[ SubKeys::arenaPK ], value );
}

bool Rules::getArenaPKing()
{
    return getRule( subKeys[ SubKeys::arenaPK ] ).toBool();
}
