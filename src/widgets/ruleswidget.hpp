#ifndef RULESWIDGET_HPP
#define RULESWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QWidget>

namespace Ui {
    class RulesWidget;
}

class RulesWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit RulesWidget(QWidget *parent = 0);
        ~RulesWidget();

    private:
        Ui::RulesWidget *ui;
};

#endif // RULESWIDGET_HPP
