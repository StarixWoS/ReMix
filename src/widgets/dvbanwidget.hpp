#ifndef DVBANWIDGET_HPP
#define DVBANWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QWidget>

namespace Ui {
    class DVBanWidget;
}

class DVBanWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit DVBanWidget(QWidget *parent = 0);
        ~DVBanWidget();

    private:
        Ui::DVBanWidget *ui;
};

#endif // DVBANWIDGET_HPP
