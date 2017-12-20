#ifndef MESSAGESWIDGET_HPP
#define MESSAGESWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QWidget>
#include <QTimer>

namespace Ui {
    class MOTDWidget;
}

class MOTDWidget : public QWidget
{
    Q_OBJECT

    static QHash<ServerInfo*, MOTDWidget*> motdWidgets;
    QString serverName{ "" };
    QTimer motdUpdate;

    public:
        explicit MOTDWidget(const QString& name);
        ~MOTDWidget();

        static MOTDWidget* getWidget(ServerInfo* server);
        static void deleteWidget(ServerInfo* server);

        void setServerName(const QString& name);

    private slots:
        void on_motdEdit_textChanged();

    private:
        Ui::MOTDWidget* ui;
};

#endif // MESSAGESWIDGET_HPP
