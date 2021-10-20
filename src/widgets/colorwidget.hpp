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

    QModelIndex currentIndex;

    public:
        explicit ColorWidget();
        ~ColorWidget();

        static ColorWidget* getInstance();
        static void deleteInstance();
        void selectColor(int currentRow);
        void setColorView(const QColor& color);

    signals:
        void colorOverrideSignal(const Colors& colorRole, const QString& color);

    private slots:
        void on_colorWidget_currentRowChanged(int currentRow);
        void on_resetColor_clicked();
        void on_colorWidget_doubleClicked(const QModelIndex &index);
        void on_selectColor_clicked();

    private:
        Ui::ColorWidget *ui;
};


#endif // COLORWIDGET_HPP
