
#ifndef ADMINHELPER_HPP
#define ADMINHELPER_HPP

#include <QInputDialog>
#include <QMessageBox>
#include <QStringList>
#include <QVariant>
#include <QString>
#include <QObject>

namespace AdminHelper
{
    enum Keys{ RANK = 0, HASH = 1, SALT = 2 };
    const QString adminKeys[ 3 ]{ "rank", "hash", "salt" };
    const QStringList ranks
    {
        QStringList() << "Game Master" << "Co-Admin" << "Admin" << "Owner"
    };

    void setAdminData(const QString& key, const QString& subKey, QVariant& value);
    QVariant getAdminData(const QString& key, const QString& subKey);

    void setReqAdminAuth(QVariant& value);
    bool getReqAdminAuth();

    bool getIsRemoteAdmin(QString& serNum);
    bool cmpRemoteAdminPwd(QString& serNum, QVariant& value);

    qint32 getRemoteAdminRank(QString& sernum);
    void setRemoteAdminRank(QString& sernum, qint32 rank);

    qint32 changeRemoteAdminRank(QWidget* parent, QString& sernum);
    bool deleteRemoteAdmin(QWidget* parent, QString& sernum);
    bool createRemoteAdmin(QWidget* parent, QString& sernum);
}

#endif // ADMINHELPER_HPP
