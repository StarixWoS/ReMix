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

    QTimer motdUpdate;
    QString serverID{ "" };

    public:
        explicit MOTDWidget(QString svrID = "0");
        ~MOTDWidget();

    private slots:
        void on_motdEdit_textChanged();

    private:
        Ui::MOTDWidget *ui;
};

#endif // MESSAGESWIDGET_HPP
