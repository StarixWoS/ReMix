#ifndef THEMECHANGE_HPP
#define THEMECHANGE_HPP

#include "prototypes.hpp"

class Theme
{
    static Themes themeType;
    static QString themeColors[ UI_THEME_COUNT ][ UI_THEME_COLORS ];

    public:
        static void applyTheme(const Themes& type = Themes::LIGHT);

        static Themes getThemeType();

        static void setThemeType(const Themes& value);

        static QBrush getThemeColor(const Colors& color);
};

#endif // THEMECHANGE_HPP
