#ifndef USERMESSAGE_HPP
#define USERMESSAGE_HPP

#include <QDialog>

namespace Ui {
    class UserMessage;
}

class UserMessage : public QDialog
{
        Q_OBJECT

    public:
        explicit UserMessage(QWidget *parent = 0);
        ~UserMessage();

    private:
        Ui::UserMessage *ui;
};

#endif // USERMESSAGE_HPP
