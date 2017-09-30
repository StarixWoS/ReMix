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
        explicit SendMsg(const QString& serNum, QWidget* parent = nullptr);
        ~SendMsg();

        bool sendToAll() const;

    private:
        bool eventFilter(QObject* obj, QEvent* event);

    private slots:
        void on_sendMsg_clicked();

    signals:
        void forwardMessage(const QString&);

    private:
        Ui::SendMsg* ui;
};

#endif // SENDMSG_HPP
