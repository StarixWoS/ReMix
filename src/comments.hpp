
#ifndef COMMENTS_HPP
#define COMMENTS_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QDialog>

namespace Ui {
    class Comments;
}

class Comments : public QDialog
{
        Q_OBJECT

    public:
        explicit Comments(QWidget *parent = 0);
        ~Comments();

        void setTitle(QString name);

    public slots:
        void newUserCommentSlot(QString& sernum, QString& alias,
                                QString& message);

    private:
        Ui::Comments *ui;
};

#endif // COMMENTS_HPP
