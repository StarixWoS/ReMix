#ifndef COLORWIDGET_HPP
#define COLORWIDGET_HPP

#include "prototypes.hpp"

#include <QModelIndex>
#include <QWidget>

namespace Ui {
    class ColorWidget;
}

class ColorWidget : public QWidget
{
    Q_OBJECT

    static ColorWidget* instance;
    const static QStringList rowList;

    QStandardItemModel* colorModel{ nullptr };

    public:
        explicit ColorWidget();
        ~ColorWidget();
        void loadColors();

        static ColorWidget* getInstance();
        static void deleteInstance();
        void selectColor(int currentRow);

    signals:
        void colorOverrideSignal(const Colors& colorRole, const QString& color);

    private slots:
        void on_resetColor_clicked();
        void on_colorView_doubleClicked(const QModelIndex &index);
        void on_selectColor_clicked();
        void themeChangedSlot();

    private:
        Ui::ColorWidget *ui;
};


#endif // COLORWIDGET_HPP
