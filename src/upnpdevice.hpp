#ifndef UPNPDEVICE_HPP
#define UPNPDEVICE_HPP

#include <QDialog>

namespace Ui {
    class UPNPDevice;
}

class UPNPDevice : public QDialog
{
        Q_OBJECT

    public:
        explicit UPNPDevice(QWidget *parent = nullptr);
        ~UPNPDevice();

    public slots:
        void addDevice(const QString& device);

    signals:
        void selectDevice(const QString& device);

    private:
        Ui::UPNPDevice *ui;
};

#endif // UPNPDEVICE_HPP
