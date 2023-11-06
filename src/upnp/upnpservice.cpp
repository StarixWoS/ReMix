
//Class includes.
#include "upnpservice.hpp"

//ReMix includes.
#include "upnpdevice.hpp"

UPnPService::UPnPService(UPnPServicePrivate* priv, QObject* parent)
    : QObject(parent),
      servicePrivate(priv)
{

}

UPnPService::~UPnPService()
{

}

QString UPnPService::getId() const
{
    return servicePrivate->id;
}

QString UPnPService::getType() const
{
    return servicePrivate->type;
}

QUrl UPnPService::getControlUrl() const
{
    return servicePrivate->controlurl;
}

QUrl UPnPService::getEventsubUrl() const
{
    return servicePrivate->eventsuburl;
}

QUrl UPnPService::getScpdUrl() const
{
    return servicePrivate->scpdurl;
}
