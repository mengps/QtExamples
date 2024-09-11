#include <QColor>
#include <QGuiApplication>
#include <QStyleHints>

#ifdef Q_OS_WIN
#include <QBasicTimer>
#include <QSettings>
#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib")
#endif

#include <QDebug>
#include <QApplication>
#include <QLabel>

class ThemeHelper : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QColor themeColor READ themeColor NOTIFY themeColorChanged)
    Q_PROPERTY(ThemeHelper::ColorScheme colorScheme READ colorScheme NOTIFY colorSchemeChanged)

public:
    enum class ColorScheme {
        None = 0,
        Dark = 1,
        Light = 2
    };
    Q_ENUM(ColorScheme);

    ThemeHelper(QObject *parent = nullptr) : QObject { parent }
    {
        m_themeColor = getThemeColor();
        m_colorScheme = getColorScheme();

#ifdef Q_OS_WIN
        m_timer.start(200, this);
#endif
    }

    Q_INVOKABLE ThemeHelper::ColorScheme getColorScheme() const
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        const auto scheme = QGuiApplication::styleHints()->colorScheme();
        return scheme == Qt::ColorScheme::Dark ? ColorScheme::Dark : ColorScheme::Light;
#else
#ifdef Q_OS_WIN
        //0：深色 - 1：浅色
        return !m_colorSchemeSettings.value("AppsUseLightTheme").toBool() ? ColorScheme::Dark : ColorScheme::Light;
#else //linux
        const QPalette defaultPalette;
        const auto text = defaultPalette.color(QPalette::WindowText);
        const auto window = defaultPalette.color(QPalette::Window);
        return text.lightness() > window.lightness() ? ColorScheme::Dark : ColorScheme::Light;
#endif // Q_OS_WIN
#endif // QT_VERSION
    }

    Q_INVOKABLE QColor getThemeColor() const
    {
#ifdef Q_OS_WIN
        return QColor::fromRgb(m_themeColorSettings.value("ColorizationColor").toUInt());
#endif
    }

    QColor themeColor() const
    {
        return m_themeColor;
    }

    ThemeHelper::ColorScheme colorScheme()
    {
        return m_colorScheme;
    }

signals:
    void themeColorChanged();
    void colorSchemeChanged();

#ifdef Q_OS_WIN
protected:
    virtual void timerEvent(QTimerEvent *)
    {
        auto nowThemeColor = getThemeColor();
        if (nowThemeColor != m_themeColor) {
            m_themeColor = nowThemeColor;
            emit themeColorChanged();
        }

        auto nowColorScheme = getColorScheme() ;
        if (nowColorScheme != m_colorScheme) {
            m_colorScheme = nowColorScheme;
            emit colorSchemeChanged();
        }
    }
#endif

private:
    QColor m_themeColor;
    ColorScheme m_colorScheme = ColorScheme::None;

#ifdef Q_OS_WIN
    QBasicTimer m_timer;
    QSettings m_themeColorSettings { QSettings::UserScope, "Microsoft", "Windows\\DWM" };
    QSettings m_colorSchemeSettings { QSettings::UserScope, "Microsoft", "Windows\\CurrentVersion\\Themes\\Personalize" };
#endif
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QLabel *window = new QLabel;
    window->setFixedSize(640, 480);
    window->setText("主题颜色");
    window->setFont(QFont("微软雅黑", 32));
    window->setAlignment(Qt::AlignCenter);
    window->show();

    auto setColorScheme = [window](bool isDark){
#ifdef Q_OS_WIN
        //win11支持
        DwmSetWindowAttribute(reinterpret_cast<HWND>(window->winId()), 20, &isDark, sizeof(isDark));
#endif
        auto color = isDark ? "black" : "white";
        auto palette = window->palette();
        palette.setColor(QPalette::Window, color);
        window->setPalette(palette);
    };

    auto setThemeColor = [window](const QString &color){
        auto palette = window->palette();
        palette.setColor(QPalette::WindowText, color);
        window->setPalette(palette);
    };

    ThemeHelper *helper = new ThemeHelper;
    QObject::connect(helper, &ThemeHelper::colorSchemeChanged, [helper, setColorScheme]{
        qDebug() << "colorSchemeChanged:" << helper->colorScheme();
        setColorScheme(helper->colorScheme() == ThemeHelper::ColorScheme::Dark);
    });
    QObject::connect(helper, &ThemeHelper::themeColorChanged, [helper, setThemeColor]{
        qDebug() << "themeColorChanged:" << helper->themeColor();
        setThemeColor(helper->themeColor().name());
    });

    setColorScheme(helper->colorScheme() == ThemeHelper::ColorScheme::Dark);
    setThemeColor(helper->themeColor().name());

    return app.exec();
}

#include "main.moc"
