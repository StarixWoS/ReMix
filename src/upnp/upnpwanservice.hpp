#ifndef UPNPWANSERVICE_HPP
#define UPNPWANSERVICE_HPP

#include "prototypes.hpp"

//Required ReMix includes.
#include "upnp/upnpservice.hpp"

//Required Qt Includes.
#include <QAbstractSocket>
#include <QHostAddress>
#include <QObject>

class UPnPWanService : public UPnPService
{
    Q_OBJECT

    public:
        explicit UPnPWanService(UPnPServicePrivate* priv, QObject* parent = nullptr);

        struct UPnPPortMap
        {
            QAbstractSocket::SocketType sockType;
            QString internalAddress{ "" };
            QString description{ "" };
            QString remoteHost{ "" };

            qint32 leaseDuration{ 0 };
            quint16 externalPort{ 0 };
            quint16 internalPort{ 0 };
            bool enabled{ true };
        };

        UPnPReply* addPortMapping(quint16 externalPort, const QString& internalAddress,
                                  quint16 internalPort, QAbstractSocket::SocketType sockType,
                                  const QString& description, bool enabled = true,
                                  int leaseDuration = 0, const QString& remoteHost = "");

        UPnPReply* addPortMapping(quint16 externalPort, const QHostAddress &internalAddress,
                                  quint16 internalPort, QAbstractSocket::SocketType sockType,
                                  const QString& description, bool enabled = true,
                                  int leaseDuration = 0, const QHostAddress& remoteHost = QHostAddress());

        UPnPReply* deletePortMapping(quint16 externalPort, QAbstractSocket::SocketType sockType, const QString& remoteHost = "");

        UPnPReply* deletePortMapping(quint16 externalPort, QAbstractSocket::SocketType sockType, const QHostAddress& remoteHost);

        UPnPReply* getSpecificPortMappingEntry(quint16 externalPort,
                                           QAbstractSocket::SocketType sockType,
                                           const QString &remoteHost = "");

        UPnPReply* getSpecificPortMappingEntry(quint16 externalPort, QAbstractSocket::SocketType sockType, const QHostAddress& remoteHost);
        UPnPReply* getGenericPortMapping();
        UPnPReply* getStatusInfo();
        UPnPReply* getExternalIp();

    private:
        void getGenericPortMapping(UPnPReply* ret, int index = 0, const QVector<UPnPPortMap>& portMaps = QVector<UPnPPortMap>());
};

#endif // UPNPWANSERVICE_HPP
