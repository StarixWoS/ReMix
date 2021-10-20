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

    static QVector<QStringList> defaultColors;
    static QMap<Colors, QString> darkOverrides;
    static QMap<Colors, QString> lightOverrides;
    static QPalette currentPal;
    static QPalette defaultPal;
    static QStyle* themeStyle;
    static Themes themeType;
    static Theme* instance;

    public:
        explicit Theme(QWidget* = nullptr);
        ~Theme() override;

        static Themes& getThemeType();
        void setThemeType(const Themes& type);

        void loadColors();

        static void applyTheme(const Themes& type = Themes::Light);
        static QString getDefaultColor(const Themes& theme, const Colors& color);
        static QBrush getColorBrush(const Colors& color);
        static QColor getColor(const Themes& theme, const Colors& color);
        static QColor getColor(const Colors& color);

        static Theme* getInstance();

        static const QPalette& getCurrentPal();
        static const QPalette getDefaultPal();

    signals:
        void themeChangedSignal(const Themes& type);
        void colorOverrideSignal(const QString& oldColor, const QString& newColor);

    public slots:
        void colorOverrideSlot(const Colors& colorRole, const QString& color);
};

#endif // THEMECHANGE_HPP
