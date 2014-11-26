
#include "adminhelper.hpp"
#include "helper.hpp"

namespace AdminHelper
{
    void setAdminData(const QString& key, const QString& subKey, QVariant& value)
    {
        QSettings adminData( "adminData.ini", QSettings::IniFormat );
        adminData.setValue( key + "/" + subKey, value );
    }

    QVariant getAdminData(const QString& key, const QString& subKey)
    {
        QSettings adminData( "adminData.ini", QSettings::IniFormat );
        if ( subKey == "rank" )
            return adminData.value( key + "/" + subKey, -1 );
        else
            return adminData.value( key + "/" + subKey );
    }

    void setReqAdminAuth(QVariant& value)
    {
        Helper::setSetting( Helper::keys[ Helper::Options ],
                            Helper::subKeys[ Helper::ReqAdminAuth ], value );
    }

    bool getReqAdminAuth()
    {
        return Helper::getSetting( Helper::keys[ Helper::Options ],
                                   Helper::subKeys[ Helper::ReqAdminAuth ] ).toBool();
    }

    bool getIsRemoteAdmin(QString& serNum)
    {
        return getAdminData( serNum, adminKeys[ AdminHelper::RANK ] ).toInt() >= 0;
    }

    bool cmpRemoteAdminPwd(QString& serNum, QVariant& value)
    {
        QString recSalt = getAdminData( serNum, adminKeys[ AdminHelper::SALT ] ).toString();
        QString recHash = getAdminData( serNum, adminKeys[ AdminHelper::HASH ] ).toString();

        QVariant pwd( value.toString() + recSalt );
        QString hash = Helper::hashPassword( pwd );

        return hash == recHash;
    }

    quint32 getRemoteAdminRank(QString& sernum)
    {
        return getAdminData( sernum, adminKeys[ AdminHelper::RANK ] ).toUInt();
    }

    void setRemoteAdminRank(QString& sernum, quint32 rank)
    {
        QVariant value = rank;
        setAdminData( sernum, adminKeys[ AdminHelper::RANK ], value );
    }

    quint32 changeRemoteAdminRank(QWidget* parent, QString& sernum)
    {
        bool ok;
        QString item = QInputDialog::getItem( parent, "Admin Rank:",
                                              "Rank:", ranks, 0, false, &ok );
        quint32 rank = -1;
        if ( ok && !item.isEmpty() )
        {
            rank = ranks.indexOf( item );
            setRemoteAdminRank( sernum, rank );
        }
        return rank;
    }

    bool deleteRemoteAdmin(QWidget* parent, QString& sernum)
    {
        if ( QMessageBox::question( parent, "Revoke Admin:", "Are you certain you want to REVOKE ( " + sernum + " )'s powers?",
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::No ) == QMessageBox::Yes )
        {
            QSettings adminData( "adminData.ini", QSettings::IniFormat );
                      adminData.remove( sernum );
            return true;
        }
        return false;
    }
}
