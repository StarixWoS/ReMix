
//Class includes.
#include "upnp/upnpreply.hpp"

//Qt Includes.
#include <QTimer>

UPnPReply::UPnPReply(QObject* parent) : QObject(parent)
{

}

UPnPReply::~UPnPReply()
{

}

bool UPnPReply::getError() const
{
    return error;
}

QString UPnPReply::getErrorCode() const
{
    return errorCode;
}

QString UPnPReply::getErrorString() const
{
    return errorString;
}

QVariant UPnPReply::getValue() const
{
    return value;
}

void UPnPReply::finish()
{
    emit finished(this);
}

void UPnPReply::finishWithData(const QVariant& data)
{
    value = data;
    emit finished(this);
}

void UPnPReply::finishWithError(const QString& msg, const QString& code)
{
    error = true;
    errorString = msg;
    errorCode = code;
    emit finished(this);
}

void UPnPReply::finishWithErrorLater(const QString& msg, const QString& code)
{
    error = true;
    errorString = msg;
    errorCode = code;
    QTimer::singleShot(0, this, [this]
    {
       emit finished(this);
    });
}
