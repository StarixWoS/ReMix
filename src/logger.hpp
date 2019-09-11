#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QGraphicsPixmapItem>
#include <QCollator>
#include <QDialog>

namespace Ui{
    class Logger;
}

class Logger : public QDialog
{
    Q_OBJECT

    static const QStringList logType;
    static const QString website;
    static LoggerSortProxyModel* tblProxy;
    static QStandardItemModel* tblModel;
    static Logger* logInstance;

    QGraphicsPixmapItem* iconViewerItem{ nullptr };
    QGraphicsScene* iconViewerScene{ nullptr };

    public:
        explicit Logger(QWidget *parent = nullptr);
        ~Logger() override;

        static Logger* getInstance();
        static void setInstance(Logger* logger = nullptr);

        void scrollToBottom();
        void insertLog(const QString& source,
                       const QString& message, const LogTypes& type,
                       const bool& logToFile = false,
                       const bool& newLine = false);
        void updateRowData(const qint32& row, const qint32& col,
                           const QVariant& data);
        void logToFile(const LogTypes& type, const QString& text,
                       const QString& timeStamp = 0,
                       const bool& newLine = false);

    private slots:
        void on_websiteLabel_linkActivated(const QString&);

        void on_autoScroll_clicked();

    private:
        Ui::Logger *ui;
};

#endif // LOGGER_HPP
