
//Class includes.
#include "campexemption.hpp"

//Qt Includes.
#include <QSettings>
#include <QObject>
#include <QtCore>

QSettings* CampExemption::exemptData{ nullptr };
CampExemption* CampExemption::instance{ nullptr };

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

QString CampExemption::makePath(const QString& plrSernum, const QString& exemptSerNum)
{
    QString path{ "%1/%2" };
            path = path.arg( plrSernum )
                       .arg( exemptSerNum );
    return path;
}

bool CampExemption::getPlayerExpemption(const QString& plrSernum, const QString& exemptSerNum)
{
    return exemptData->value( makePath( plrSernum, exemptSerNum ) ).toBool();
}

bool CampExemption::setPlayerExemption(const QString& plrSernum, const QString& exemptSerNum)
{
    bool ret{ true };
    if ( getPlayerExpemption( plrSernum, exemptSerNum ) )
    {
        //Remove friend. Return false to signal a message to the User.
        exemptData->remove( makePath( plrSernum, exemptSerNum ) );
        ret = false;
    }
    else
        exemptData->setValue( makePath( plrSernum, exemptSerNum ), true );

    exemptData->sync();
    return ret;
}
