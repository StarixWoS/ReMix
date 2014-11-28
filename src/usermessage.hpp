#ifndef USERMESSAGE_HPP
#define USERMESSAGE_HPP

#include <QTextCursor>
#include <QScrollBar>
#include <QTextEdit>
#include <QDateTime>
#include <QDialog>
#include <QDebug>
#include <QDir>

namespace Ui {
    class UserMessage;
}

class UserMessage : public QDialog
{
        Q_OBJECT

    public:
        explicit UserMessage(QWidget *parent = 0);
        ~UserMessage();

    public slots:
        void newUserCommentSlot(QString& sernum, QString& alias, QString& message);

    private:
        Ui::UserMessage *ui;
};

#endif // USERMESSAGE_HPP
