#ifndef COMMENTVIEWWIDGET_HPP
#define COMMENTVIEWWIDGET_HPP

#include <QWidget>

namespace Ui {
    class CommentViewWidget;
}

class CommentViewWidget : public QWidget
{
    Q_OBJECT

    public:
        explicit CommentViewWidget(QWidget *parent = nullptr);
        ~CommentViewWidget();

        void setTitle(const QString& name);
        void insertComment(const QString& sernum, const QString& alias,
                           const QString& message);

    private:
        Ui::CommentViewWidget *ui;
};

#endif // COMMENTVIEWWIDGET_HPP
