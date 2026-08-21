#pragma once

#include <QColor>
#include <QString>

struct WaiterGoodsDishStyle
{
    static constexpr char borderColorCss[] = "#2c4383";

    int spacing = 1;
    int borderWidth = 1;
    QColor colorDefault = QColor(Qt::white);
    int height = 60;
    int fontSize = 12;
    bool fontBold = false;
    QColor fontColor = QColor(Qt::black);

    bool loadFromCssFile(const QString &path, QString *error = nullptr);
    bool saveToCssFile(const QString &path, QString *error = nullptr) const;

    QString dishStylesheet() const;

    static QString cssFilePath();
    static bool loadAtStartup(QString *error = nullptr);
    static void updateCacheFromCss(const QString &css);
    static const WaiterGoodsDishStyle &cachedStyle();
};
