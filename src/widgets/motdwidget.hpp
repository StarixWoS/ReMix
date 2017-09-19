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

    QString serverName{ "" };
    QTimer motdUpdate;

    public:
        explicit MOTDWidget();
        ~MOTDWidget();

        void setServerName(const QString& name);

    private slots:
        void on_motdEdit_textChanged();

    private:
        Ui::MOTDWidget* ui;
};

#endif // MESSAGESWIDGET_HPP
