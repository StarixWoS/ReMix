#ifndef SENDMSG_HPP
#define SENDMSG_HPP

#include "prototypes.hpp"


//Required Qt Includes.
#include <QDialog>

namespace Ui {
    class SendMsg;
}

class SendMsg : public QDialog
{
    Q_OBJECT

    public:
        explicit SendMsg(QString serNum, QWidget *parent = nullptr);
        ~SendMsg();

        bool sendToAll();

    private:
        bool eventFilter(QObject* obj, QEvent* event);

    private slots:
        void on_sendMsg_clicked();

    signals:
        void forwardMessage(QString);

    private:
        Ui::SendMsg *ui;
};

#endif // SENDMSG_HPP
