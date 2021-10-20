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

    static QHash<Server*, MOTDWidget*> motdWidgets;
    QString serverName{ "" };
    QTimer motdUpdate;

    public:
        explicit MOTDWidget();
        ~MOTDWidget() override;

        static MOTDWidget* getInstance(Server* server);
        static void deleteInstance(Server* server);

        void setServerName(const QString& name);
        void changeMotD(const QString& message);

    public slots:
        void nameChangedSlot(const QString& name);

    private slots:
        void on_motdEdit_textChanged();

    private:
        Ui::MOTDWidget* ui;
};

#endif // MESSAGESWIDGET_HPP
