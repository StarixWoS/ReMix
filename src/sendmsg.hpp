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
    ServerInfo* server{ nullptr };
    Player* target{ nullptr };

    public:
        explicit SendMsg(QWidget *parent = nullptr, ServerInfo* svr = nullptr,
                         Player* trg = nullptr);
        ~SendMsg();

    private:
        bool eventFilter(QObject* obj, QEvent* event);

    private slots:
        void on_sendMsg_clicked();

    private:
        Ui::SendMsg *ui;
};

#endif // SENDMSG_HPP
