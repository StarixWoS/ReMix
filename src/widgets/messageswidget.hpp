#ifndef MESSAGESWIDGET_HPP
#define MESSAGESWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QWidget>
#include <QTimer>

namespace Ui {
    class MessagesWidget;
}

class MessagesWidget : public QWidget
{
    Q_OBJECT

    QTimer motdUpdate;
    QTimer banMUpdate;

    public:
        explicit MessagesWidget(QWidget *parent = 0);
        ~MessagesWidget();

    private slots:
        void on_motdEdit_textChanged();
        void on_banishedEdit_textChanged();

    private:
        Ui::MessagesWidget *ui;
};

#endif // MESSAGESWIDGET_HPP
