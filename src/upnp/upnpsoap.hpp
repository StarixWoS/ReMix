#ifndef UPNPSOAP_HPP
#define UPNPSOAP_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QString>

class UPnPSoap
{
    public:
        UPnPSoap(const QString& action, const QString& actionNamespace);
        ~UPnPSoap();

        void writeTextElement(const QString& qualifiedName, const QString& text);

        QByteArray render();
        QNetworkRequest request(const QUrl& url) const;

        static QPair<QString, QString> responseError(const QByteArray& data);

    private:
        QXmlStreamWriter* xmlStream{ nullptr };
        QString soapAction{ "" };
        QString soapActionNamespace{ "" };
        QByteArray soapData{ "" };
        bool isOpen{ true };
};

#endif // UPNPSOAP_HPP
