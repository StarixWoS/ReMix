
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

void Rules::setRule(const QString& key, QVariant& value, QString& svrID)
{
    Settings::prefs->setValue( svrID % "/" % Settings::keys[ Settings::Rules ]
                               % "/" % key, value );
}

QVariant Rules::getRule(const QString& key, QString& svrID)
{
    return Settings::prefs->value( svrID % "/"
                                   % Settings::keys[ Settings::Rules ]
                                   % "/" % key );
}

QString Rules::getRuleSet(QString& svrID)
{
    Settings::prefs->beginGroup( svrID % "/" %
                                 Settings::keys[ Settings::Rules ] );

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

        rules.append( ", " );
    }
    Settings::prefs->endGroup();
    return rules;
}

bool Rules::getRequireWorld(QString& svrID)
{
    return getRule( subKeys[ SubKeys::world ], svrID ).toString().isEmpty();
}

void Rules::setWorldName(QString& value, QString& svrID)
{
    QVariant data{ value };
    setRule( subKeys[ SubKeys::world ], data, svrID );
}

QString Rules::getWorldName(QString& svrID)
{
    return getRule( subKeys[ SubKeys::world ], svrID ).toString();
}

bool Rules::getRequireURL(QString& svrID)
{
    return getRule( subKeys[ SubKeys::url ], svrID ).toString().isEmpty();
}

void Rules::setURLAddress(QString& value, QString& svrID)
{
    QVariant data{ value };
    setRule( subKeys[ SubKeys::url ], data, svrID );
}

QString Rules::getURLAddress(QString& svrID)
{
    return getRule( subKeys[ SubKeys::url ], svrID ).toString();
}

void Rules::setAllPKing(QVariant& value, QString& svrID)
{
    setRule( subKeys[ SubKeys::allPK ], value, svrID );
}

bool Rules::getAllPKing(QString& svrID)
{
    return getRule( subKeys[ SubKeys::allPK ], svrID ).toBool();
}

bool Rules::getRequireMaxPlayers(QString& svrID)
{
    return getRule( subKeys[ SubKeys::maxP ], svrID ).toUInt() == 0;
}

void Rules::setMaxPlayers(quint32 value, QString& svrID)
{
    QVariant data{ value };
    setRule( subKeys[ SubKeys::maxP ], data, svrID );
}

quint32 Rules::getMaxPlayers(QString& svrID)
{
    return getRule( subKeys[ SubKeys::maxP ], svrID ).toUInt();
}

bool Rules::getRequireMaxAFK(QString& svrID)
{
    return getRule( subKeys[ SubKeys::maxAFK ], svrID ).toUInt() == 0;
}

void Rules::setMaxAFK(quint32 value, QString& svrID)
{
    QVariant data{ value };
    setRule( subKeys[ SubKeys::maxAFK ], data, svrID );
}

quint32 Rules::getMaxAFK(QString& svrID)
{
    return getRule( subKeys[ SubKeys::maxAFK ], svrID ).toUInt();
}

bool Rules::getRequireMinVersion(QString& svrID)
{
    return getRule( subKeys[ SubKeys::minV ], svrID).toString().isEmpty();
}

void Rules::setMinVersion(QString& value, QString& svrID)
{
    QVariant data{ value };
    setRule( subKeys[ SubKeys::minV ], data, svrID );
}

QString Rules::getMinVersion(QString& svrID)
{
    return getRule( subKeys[ SubKeys::minV ], svrID ).toString();
}

void Rules::setReportLadder(QVariant& value, QString& svrID)
{
    setRule( subKeys[ SubKeys::ladder ], value, svrID );
}

bool Rules::getReportLadder(QString& svrID)
{
    return getRule( subKeys[ SubKeys::ladder ], svrID ).toBool();
}

void Rules::setNoCursing(QVariant& value, QString& svrID)
{
    setRule( subKeys[ SubKeys::noBleep ], value, svrID );
}

bool Rules::getNoCursing(QString& svrID)
{
    return getRule( subKeys[ SubKeys::noBleep ], svrID ).toBool();
}

void Rules::setNoCheating(QVariant& value, QString& svrID)
{
    setRule( subKeys[ SubKeys::noCheat ], value, svrID );
}

bool Rules::getNoCheating(QString& svrID)
{
    return getRule( subKeys[ SubKeys::noCheat ], svrID ).toBool();
}

void Rules::setNoEavesdropping(QVariant& value, QString& svrID)
{
    setRule( subKeys[ SubKeys::noEavesdrop ], value, svrID );
}

bool Rules::getNoEavesdropping(QString& svrID)
{
    return getRule( subKeys[ SubKeys::noEavesdrop ], svrID ).toBool();
}

void Rules::setNoMigrating(QVariant& value, QString& svrID)
{
    setRule( subKeys[ SubKeys::noMigrate ], value, svrID );
}

bool Rules::getNoMigrating(QString& svrID)
{
    return getRule( subKeys[ SubKeys::noMigrate ], svrID ).toBool();
}

void Rules::setNoModding(QVariant& value, QString& svrID)
{
    setRule( subKeys[ SubKeys::noMod ], value, svrID );
}

bool Rules::getNoModding(QString& svrID)
{
    return getRule( subKeys[ SubKeys::noMod ], svrID ).toBool();
}

void Rules::setNoPets(QVariant& value, QString& svrID)
{
    setRule( subKeys[ SubKeys::noPets ], value, svrID );
}

bool Rules::getNoPets(QString& svrID)
{
    return getRule( subKeys[ SubKeys::noPets ], svrID ).toBool();
}

void Rules::setNoPKing(QVariant& value, QString& svrID)
{
    setRule( subKeys[ SubKeys::noPK ], value, svrID );
}

bool Rules::getNoPKing(QString& svrID)
{
    return getRule( subKeys[ SubKeys::noPK ], svrID ).toBool();
}

void Rules::setArenaPKing(QVariant& value, QString& svrID)
{
    setRule( subKeys[ SubKeys::arenaPK ], value, svrID );
}

bool Rules::getArenaPKing(QString& svrID)
{
    return getRule( subKeys[ SubKeys::arenaPK ], svrID ).toBool();
}
