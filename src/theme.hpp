#ifndef THEMECHANGE_HPP
#define THEMECHANGE_HPP

//ReMix Includes
#include "prototypes.hpp"

//Qt Includes
#include <QStringList>
#include <QVector>
#include <QStyle>

class Theme : public QObject
{
    Q_OBJECT

    static QVector<QStringList> themeColors;
    static QStyle* themeStyle;
    static Themes themeType;
    static Theme* instance;

    public:
        explicit Theme(QWidget* = nullptr);
        ~Theme() override;

        static void applyTheme(const Themes& type = Themes::Light);
        static Themes getThemeType();
        void setThemeType(const Themes& value);
        static QBrush getThemeColor(const Colors& color);
        static QColor getThemeColor(const Themes& theme, const Colors& color);

        static Theme* getInstance();

    signals:
        void themeChangedSignal(const Themes& type);
};

#endif // THEMECHANGE_HPP
