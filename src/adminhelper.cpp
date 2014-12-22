
#include "includes.hpp"

//Initialize QStrings.
const QString AdminHelper::adminKeys[ 3 ] =
{
    "rank", "hash", "salt"
};

const QStringList AdminHelper::ranks
{
    QStringList() << "Game Master" << "Co-Admin" << "Admin" << "Owner"
};

void AdminHelper::setAdminData(const QString& key, const QString& subKey, QVariant& value)
{
    QSettings adminData( "adminData.ini", QSettings::IniFormat );
    adminData.setValue( key % "/" % subKey, value );
}

QVariant AdminHelper::getAdminData(const QString& key, const QString& subKey)
{
    QSettings adminData( "adminData.ini", QSettings::IniFormat );
    if ( subKey == QLatin1String( "rank" ) )
        return adminData.value( key % "/" % subKey, -1 );
    else
        return adminData.value( key % "/" % subKey );
}

void AdminHelper::setReqAdminAuth(QVariant& value)
{
    Helper::setSetting( Helper::keys[ Helper::Options ],
            Helper::subKeys[ Helper::ReqAdminAuth ], value );
}

bool AdminHelper::getReqAdminAuth()
{
    return Helper::getSetting( Helper::keys[ Helper::Options ],
            Helper::subKeys[ Helper::ReqAdminAuth ] ).toBool();
}

bool AdminHelper::getIsRemoteAdmin(QString& serNum)
{
    return getAdminData( serNum, adminKeys[ AdminHelper::RANK ] ).toInt() >= 0;
}

bool AdminHelper::cmpRemoteAdminPwd(QString& serNum, QVariant& value)
{
    QString recSalt = getAdminData( serNum, adminKeys[ AdminHelper::SALT ] ).toString();
    QString recHash = getAdminData( serNum, adminKeys[ AdminHelper::HASH ] ).toString();

    QVariant hash( recSalt + value.toString() );
    hash = Helper::hashPassword( hash );

    return hash == recHash;
}

qint32 AdminHelper::getRemoteAdminRank(QString& sernum)
{
    return getAdminData( sernum, adminKeys[ AdminHelper::RANK ] ).toUInt();
}

void AdminHelper::setRemoteAdminRank(QString& sernum, qint32 rank)
{
    QVariant value = rank;
    setAdminData( sernum, adminKeys[ AdminHelper::RANK ], value );
}

qint32 AdminHelper::changeRemoteAdminRank(QWidget* parent, QString& sernum)
{
    bool ok{ false };
    QString item = QInputDialog::getItem( parent, "Admin Rank:",
                                          "Rank:", ranks, 0, false, &ok );
    qint32 rank{ -1 };
    if ( ok && !item.isEmpty() )
    {
        rank = ranks.indexOf( item );
        setRemoteAdminRank( sernum, rank );
    }
    return rank;
}

bool AdminHelper::deleteRemoteAdmin(QWidget* parent, QString& sernum)
{
    QString title{ "Revoke Admin:" };
    QString prompt{ "Are you certain you want to REVOKE ( " % sernum % " )'s powers?" };

    if ( Helper::confirmAction( parent, title, prompt ) )
    {
        QSettings adminData( "adminData.ini", QSettings::IniFormat );
        adminData.remove( sernum );
        return true;
    }
    return false;
}

bool AdminHelper::createRemoteAdmin(QWidget* parent, QString& sernum)
{
    QString title{ "Create Admin:" };
    QString prompt{ "Are you certain you want to MAKE ( %1 ) a Remote Admin?\r\n\r\n"
                    "Please make sure you trust ( %2 ) as this will allow the them to utilize Admin "
                    "commands that can remove the ability for other users to connect to the Server." };
    prompt = prompt.arg( sernum )
             .arg( sernum );

    if ( Helper::confirmAction( parent, title, prompt ) )
        return true;

    return false;
}

