#ifndef REMIXMASTER_HPP
#define REMIXMASTER_HPP

#include <QWidget>

namespace Ui {
    class ReMixMaster;
}

class ReMixMaster : public QWidget
{
        Q_OBJECT

    public:
        explicit ReMixMaster(QWidget *parent = nullptr);
        ~ReMixMaster();

    private:
        Ui::ReMixMaster *ui;
};

#endif // REMIXMASTER_HPP
