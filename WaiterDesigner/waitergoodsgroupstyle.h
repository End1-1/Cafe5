#pragma once

#include <QColor>
#include <QString>

struct WaiterGoodsGroupStyle
{
    static constexpr int buttonHeight = 48;
    static constexpr char borderColorCss[] = "#445566";

    int spacing = 1;
    int borderWidth = 0;
    QColor colorDefault = QColor(QStringLiteral("#85a4b8"));
    int visibleRows = 3;
    int fontSize = 12;
    bool fontBold = true;
    QColor fontColor = QColor(Qt::black);

    bool loadFromCssFile(const QString &path, QString *error = nullptr);
    bool saveToCssFile(const QString &path, QString *error = nullptr) const;

    QString groupStylesheet() const;
    int stripMaxHeight() const;

    static QString cssFilePath();
    static bool loadAtStartup(QString *error = nullptr);
    static void updateCacheFromCss(const QString &css);
    static const WaiterGoodsGroupStyle &cachedStyle();
};
