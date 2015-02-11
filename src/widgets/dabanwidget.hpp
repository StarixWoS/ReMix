#ifndef DABANWIDGET_HPP
#define DABANWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QWidget>

namespace Ui {
    class DABanWidget;
}

class DABanWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit DABanWidget(QWidget *parent = 0);
        ~DABanWidget();

    private:
        Ui::DABanWidget *ui;
};

#endif // DABANWIDGET_HPP
