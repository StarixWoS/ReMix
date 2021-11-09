
//Class includes.
#include "campexemption.hpp"
#include "helper.hpp"
#include "player.hpp"

//Qt Includes.
#include <QSettings>
#include <QObject>
#include <QMutex>
#include <QtCore>

QSettings* CampExemption::exemptData{ nullptr };
CampExemption* CampExemption::instance{ nullptr };
QMutex CampExemption::mutex;

CampExemption::CampExemption(QObject *parent) : QObject(parent)
{
    //Setup our QSettings Object.
    exemptData = new QSettings( "campExemptions.ini", QSettings::IniFormat );

    if ( instance == nullptr )
        this->setInstance( this );
}

CampExemption::~CampExemption()
{
    exemptData->sync();
    exemptData->deleteLater();
}

void CampExemption::setInstance(CampExemption* value)
{
    instance = value;
}

CampExemption* CampExemption::getInstance()
{
    if ( instance == nullptr )
        instance = new CampExemption( nullptr );

    return instance;
}

QString CampExemption::makePath(const QString& plrSernum, const QString& key)
{
    QString path{ "%1/%2" };
            path = path.arg( plrSernum )
                       .arg( key );
    return path;
}

QString CampExemption::makePath(const QString& plrSernum, const QString& key, const QString& exemptSerNum)
{
    QString path{ "%1/%2/%3" };
            path = path.arg( plrSernum )
                       .arg( key )
                       .arg( exemptSerNum );
    return path;
}

void CampExemption::setDataFromPath(const QString& path, const QVariant& value)
{
    QMutexLocker<QMutex> locker( &mutex );
    exemptData->sync();
    exemptData->setValue( path, value );
    exemptData->sync();
}

QVariant CampExemption::getDataFromPath(const QString& path)
{
    QMutexLocker<QMutex> locker( &mutex );
    exemptData->sync();
    return exemptData->value( path );
}

void CampExemption::removeSettingFromPath(const QString& path)
{
    QMutexLocker<QMutex> locker( &mutex );
    exemptData->sync();
    exemptData->remove( path );
    exemptData->sync();
}

void CampExemption::setIsLocked(const QString& sernum, const bool& state)
{
    if ( state == true )
        setDataFromPath( makePath( sernum, "isLocked" ), state );
    else
        removeSettingFromPath( makePath( sernum, "isLocked" ) );
}

bool CampExemption::getIsLocked(const QString& sernum)
{
    return getDataFromPath( makePath( sernum, "isLocked" ) ).toBool();
}

void CampExemption::setAllowCurrent(const QString& sernum, const bool& state)
{
    if ( state == true )
        setDataFromPath( makePath( sernum, "allowCurrent" ), state );
    else
        removeSettingFromPath( makePath( sernum, "allowCurrent" ) );
}

bool CampExemption::getAllowCurrent(const QString& sernum)
{
    return getDataFromPath( makePath( sernum, "allowCurrent" ) ).toBool();
}

void CampExemption::setIsWhitelisted(const QString& srcSerNum, const QString& targetSerNum, const bool& state)
{
    if ( state == false )
        removeSettingFromPath( makePath( srcSerNum, "whiteListed", targetSerNum ) );
    else
        setDataFromPath( makePath( srcSerNum, "whiteListed", targetSerNum ), state );
}

bool CampExemption::getIsWhitelisted(const QString& srcSerNum, const QString& targetSerNum)
{
    return getDataFromPath( makePath( srcSerNum, "whiteListed", targetSerNum ) ).toBool();
}

QString CampExemption::getWhiteListedUsers(const QString& srcSerNum)
{
    QMutexLocker<QMutex> locker( &mutex );

    exemptData->sync();
    exemptData->beginGroup( makePath( srcSerNum, "whiteListed" ) );

    QStringList whiteList{ exemptData->allKeys() };
    QString list{ "" };

    if ( !whiteList.isEmpty() )
    {
        for ( const auto& item : whiteList )
        {
            list.append( Helper::serNumToIntStr( item, true ) );
            list.append( ", " );
        }
    }
    exemptData->endGroup();
    return list;
}

void CampExemption::hexSerNumSetSlot(QSharedPointer<Player> plr)
{
    if ( plr != nullptr )
    {
        plr->setIsCampLocked( getIsLocked( plr->getSernumHex_s() ) );
        plr->setIsCampOptOut( getAllowCurrent( plr->getSernumHex_s() ) );
    }
}
