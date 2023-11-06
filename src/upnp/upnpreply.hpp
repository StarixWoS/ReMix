#ifndef UPNPREPLY_HPP
#define UPNPREPLY_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QVariant>
#include <QObject>

class UPnPReply : public QObject
{
    Q_OBJECT

    public:
        explicit UPnPReply(QObject* parent = nullptr);
        virtual ~UPnPReply();

        bool getError() const;
        QString getErrorCode() const;
        QString getErrorString() const;
        QVariant getValue() const;

        void finish();
        void finishWithData(const QVariant& data);
        void finishWithError(const QString& msg, const QString& code = "");
        void finishWithErrorLater(const QString& msg, const QString& code = "");

    signals:
        void finished(UPnPReply* reply);

    private:
        QVariant value;
        QString errorCode{ "" };
        QString errorString{ "" };
        bool error{ false };
};

#endif // UPNPREPLY_HPP
