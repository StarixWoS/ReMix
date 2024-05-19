#include "upnpdevice.hpp"
#include "ui_upnpdevice.h"

UPNPDevice::UPNPDevice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UPNPDevice)
{
    ui->setupUi(this);
}

UPNPDevice::~UPNPDevice()
{
    delete ui;
}

void UPNPDevice::addDevice(const QString& device)
{

}
