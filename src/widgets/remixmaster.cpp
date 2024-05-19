#include "remixmaster.hpp"
#include "ui_remixmaster.h"

ReMixMaster::ReMixMaster(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReMixMaster)
{
    ui->setupUi(this);
}

ReMixMaster::~ReMixMaster()
{
    delete ui;
}
