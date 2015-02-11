
#include "includes.hpp"
#include "dvbanwidget.hpp"
#include "ui_dvbanwidget.h"

DVBanWidget::DVBanWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DVBanWidget)
{
    ui->setupUi(this);
}

DVBanWidget::~DVBanWidget()
{
    delete ui;
}
