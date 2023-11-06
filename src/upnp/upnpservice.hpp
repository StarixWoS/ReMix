#ifndef UPNPSERVICE_HPP
#define UPNPSERVICE_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QObject>
#include <QString>
#include <QUrl>

class UPnPServicePrivate
{
    public:
        QString id;
        QString type;
        QUrl controlurl;
        QUrl eventsuburl;
        QUrl scpdurl;
};

class UPnPService : public QObject
{
    Q_OBJECT

    public:
        explicit UPnPService(UPnPServicePrivate* priv, QObject* parent = nullptr);
        ~UPnPService();

        QString getId() const;
        QString getType() const;
        QUrl getControlUrl() const;
        QUrl getEventsubUrl() const;
        QUrl getScpdUrl() const;

    private:
        UPnPServicePrivate* servicePrivate;
};

#endif // UPNPSERVICE_HPP
