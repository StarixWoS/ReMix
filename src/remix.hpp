
#ifndef REMIX_HPP
#define REMIX_HPP

#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QMainWindow>

class Messages;
class BannedIP;

namespace Ui {
class ReMix;
}

class ReMix : public QMainWindow
{
    Q_OBJECT

    QSortFilterProxyModel* plrViewProxy{ nullptr };
    QStandardItemModel* plrViewModel{ nullptr };

    QSortFilterProxyModel* svrViewProxy{ nullptr };
    QStandardItemModel* svrViewModel{ nullptr };

    Messages* sysMessages{ nullptr };
    BannedIP* banIP{ nullptr };

    public:
        explicit ReMix(QWidget *parent = 0);
        ~ReMix();

    private slots:
        void on_openSysMessages_clicked();

        void on_openBanIP_clicked();

    private:
        Ui::ReMix *ui;
};

#endif // REMIX_HPP
