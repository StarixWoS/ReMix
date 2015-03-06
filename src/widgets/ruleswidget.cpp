
#include "includes.hpp"
#include "ruleswidget.hpp"
#include "ui_ruleswidget.h"

RulesWidget::RulesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RulesWidget)
{
    ui->setupUi(this);
}

RulesWidget::~RulesWidget()
{
    delete ui;
}
