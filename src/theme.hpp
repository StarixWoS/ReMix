#ifndef THEMECHANGE_HPP
#define THEMECHANGE_HPP

//ReMix Includes
#include "prototypes.hpp"

//Qt Includes
#include <QStringList>
#include <QVector>

class Theme
{
    static Themes themeType;
    static QVector<QStringList> themeColors;

    public:

        static void applyTheme(const Themes& type = Themes::Light);

        static Themes getThemeType();

        static void setThemeType(const Themes& value);

        static QBrush getThemeColor(const Colors& color);
};

#endif // THEMECHANGE_HPP
