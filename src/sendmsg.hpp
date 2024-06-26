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
        explicit SendMsg(QWidget* parent = nullptr);
        ~SendMsg() override;

        void setTitle(const QString& title);
        bool sendToAll() const;

    private:
        bool eventFilter(QObject* obj, QEvent* event) override;

    private slots:
        void on_sendMsg_clicked();

    signals:
        void forwardMessageSignal(const QString&);

    private:
        Ui::SendMsg* ui;
};

#endif // SENDMSG_HPP
