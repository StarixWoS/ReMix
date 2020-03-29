
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
        ServerInfo* server{ nullptr };

    public:
        explicit Comments(QWidget* parent = nullptr,
                          ServerInfo* serverInfo = nullptr);
        ~Comments() override;

        void setTitle(const QString& name);

    public slots:
        void newUserCommentSlot(const QString& sernum, const QString& alias, const QString& message);

    private:
        Ui::Comments* ui;
};

#endif // COMMENTS_HPP
