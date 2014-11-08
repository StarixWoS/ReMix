#include "usermessage.hpp"
#include "ui_usermessage.h"

UserMessage::UserMessage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserMessage)
{
    ui->setupUi(this);

    this->setWindowModality( Qt::WindowModal );
}

UserMessage::~UserMessage()
{
    delete ui;
}
