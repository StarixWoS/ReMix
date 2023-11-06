
//Class includes.
#include "upnpsoap.hpp"

//Qt Includes.
#include <QCoreApplication>
#include <QXmlStreamWriter>
#include <QNetworkRequest>
#include <QDomDocument>

UPnPSoap::UPnPSoap(const QString& action, const QString& actionNamespace)
    : xmlStream( new QXmlStreamWriter( &soapData ) ),
      soapAction( action ),
      soapActionNamespace( actionNamespace )
{
    xmlStream->setAutoFormatting( true );
    xmlStream->writeStartDocument();

    const QString envelopeNS{ QStringLiteral( "http://schemas.xmlsoap.org/soap/envelope/" ) };

    xmlStream->writeNamespace( envelopeNS, QStringLiteral( "s" ) );
    xmlStream->writeStartElement( envelopeNS, QStringLiteral( "Envelope" ) );
    xmlStream->writeAttribute( envelopeNS,
                               QStringLiteral( "encodingStyle" ),
                               QStringLiteral( "http://schemas.xmlsoap.org/soap/encoding/" ) );
    xmlStream->writeStartElement( envelopeNS, QStringLiteral( "Body" ) );
    xmlStream->writeStartElement( QLatin1String( "u:" ) + action );
    xmlStream->writeNamespace( actionNamespace, QStringLiteral( "u" ) );
}

UPnPSoap::~UPnPSoap()
{
    delete xmlStream;
}

void UPnPSoap::writeTextElement(const QString&qualifiedName, const QString&text)
{
    if ( isOpen )
        xmlStream->writeTextElement( qualifiedName, text );
}

QByteArray UPnPSoap::render()
{
    if ( isOpen )
    {
        xmlStream->writeEndElement(); // ACTION
        xmlStream->writeEndElement(); // Body
        xmlStream->writeEndElement(); // Envelope
        xmlStream->writeEndDocument(); // XML
        isOpen = false;
    }
    return soapData;
}

QNetworkRequest UPnPSoap::request(const QUrl& url) const
{
    QNetworkRequest req( url );
    static const QByteArray ua{ QCoreApplication::applicationName().toLatin1()
                              + "/"
                              + QCoreApplication::applicationVersion().toLatin1()
                              + ", ReMix/"
                              + REMIX_VERSION };

    req.setHeader( QNetworkRequest::ContentTypeHeader, QByteArrayLiteral( "text/xml" ) );
    req.setHeader( QNetworkRequest::UserAgentHeader, ua );
    req.setRawHeader( QByteArrayLiteral( "SOAPAction" ),
                      soapActionNamespace.toLatin1() + "#" + soapAction.toLatin1() );

    return req;
}

QPair<QString, QString> parseUPnPError(QXmlStreamReader& xml)
{
    QPair<QString, QString> ret;
    while ( !xml.atEnd() )
    {
        QXmlStreamReader::TokenType type{ xml.readNext() };
        if ( type == QXmlStreamReader::StartElement )
        {
            if ( xml.name() == QLatin1String( "errorCode" ) )
                ret.first = xml.readElementText();
            else if ( xml.name() == QLatin1String( "errorDescription" ) )
                ret.second = xml.readElementText();
            else
                xml.skipCurrentElement();
        }
    }
    return ret;
}

QPair<QString, QString> parseDetail(QXmlStreamReader& xml)
{
    QPair<QString, QString> ret;
    while ( !xml.atEnd() )
    {
        QXmlStreamReader::TokenType type{ xml.readNext() };
        if ( type == QXmlStreamReader::StartElement )
        {
            if ( xml.name() == QLatin1String( "UPnPError" )
              ||  xml.name() == QLatin1String( "error" ) )
            {
                ret = parseUPnPError( xml );
            }
            else
                xml.skipCurrentElement();
        }
    }
    return ret;
}

QPair<QString, QString> parseFault(QXmlStreamReader& xml)
{
    QPair<QString, QString> ret;
    while ( !xml.atEnd() )
    {
        QXmlStreamReader::TokenType type{ xml.readNext() };
        if (type == QXmlStreamReader::StartElement)
        {
            if ( xml.name() == QLatin1String( "detail" ) )
                ret = parseDetail( xml );
            else
                xml.skipCurrentElement();
        }
    }
    return ret;
}

QPair<QString, QString> parseBody(QXmlStreamReader& xml)
{
    QPair<QString, QString> ret;
    while ( !xml.atEnd() )
    {
        QXmlStreamReader::TokenType type{ xml.readNext() };
        if ( type == QXmlStreamReader::StartElement )
        {
            if ( xml.name() == QLatin1String( "Fault" ) )
                ret = parseFault( xml );
            else
                xml.skipCurrentElement();
        }
    }
    return ret;
}

QPair<QString, QString> parseEnvelope(QXmlStreamReader& xml)
{
    QPair<QString, QString> ret;
    while ( !xml.atEnd() )
    {
        QXmlStreamReader::TokenType type{ xml.readNext() };
        if ( type == QXmlStreamReader::StartElement )
        {
            if ( xml.name() == QLatin1String( "Body" ) )
                ret = parseBody( xml );
            else
                xml.skipCurrentElement();
        }
    }
    return ret;
}

QPair<QString, QString> UPnPSoap::responseError(const QByteArray& data)
{
    QPair<QString, QString> ret;
    QXmlStreamReader xml{ data };

    while ( !xml.atEnd() )
    {
        QXmlStreamReader::TokenType type{ xml.readNext() };
        if ( type == QXmlStreamReader::StartElement )
        {
            if ( xml.name() == QLatin1String( "Envelope" ) )
                ret = parseEnvelope( xml );
            else
                xml.skipCurrentElement();
        }
    }
    return ret;
}
