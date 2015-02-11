
#include "includes.hpp"
#include "dabanwidget.hpp"
#include "ui_dabanwidget.h"

DABanWidget::DABanWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DABanWidget)
{
    ui->setupUi(this);
}

DABanWidget::~DABanWidget()
{
    delete ui;
}
