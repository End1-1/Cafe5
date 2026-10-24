#pragma once

#include <QApplication>
#include <QFontDatabase>
#include <QImage>
#include <QPainter>
#include <QPageSize>
#include <QPrinter>
#include <QRect>
#include <QSizeF>
#include <QStringList>
#include "c5config.h"
#include "c5printing.h"

/** Same point size for Smart receipt, kitchen and reports. */
inline int smartPrintPointSize()
{
    int bs = __c5config.getValue(param_service_print_font_size).toInt();

    if(bs < 6) {
        bs = 18;
    }

    return bs * 2 - 12;
}

inline QString smartPrintFontFamily()
{
    static QString family = QStringLiteral("Arial LatArm Unicode");
    static bool ready = false;

    if(ready) {
        return family;
    }

    ready = true;
    const int id = QFontDatabase::addApplicationFont(qApp->applicationDirPath() + QStringLiteral("/ahuni.ttf"));

    if(id >= 0) {
        const QStringList families = QFontDatabase::applicationFontFamilies(id);

        if(!families.isEmpty()) {
            family = families.constFirst();
        }
    }

    return family;
}

/** Width from driver; tall canvas so content is not clipped while drawing. */
inline void setupSmartPrintScene(C5Printing &p, QPrinter &printer)
{
    const QRectF pr = printer.pageRect(QPrinter::DevicePixel);
    const qreal dpiX = qMax<qreal>(1.0, printer.logicalDpiX());
    qreal width96 = pr.width() * 96.0 / dpiX;
    constexpr qreal safePx = 5.0 * 96.0 / 25.4;
    constexpr qreal height96 = 12000.0;

    if(width96 < 80.0) {
        width96 = 200.0;
    }

    p.setSceneParams(qMax<qreal>(80.0, width96 - safePx), height96, 96.0);
    p.setRightMarginMm(3.0);
}

inline void setupSmartReportScene(C5Printing &p, QPrinter &printer)
{
    setupSmartPrintScene(p, printer);
}

/**
 * Smart-only print. Many receipt drivers ignore custom page height and clip at ~279mm.
 * Draw the full image in page-height strips with newPage() — does not touch c5printing.cpp.
 */
inline bool smartPrint(C5Printing &c5, QPrinter &prn)
{
    QImage src = c5.resultImage();

    if(src.isNull() || src.width() < 1 || src.height() < 1) {
        return false;
    }

    QImage finalImage = src.convertToFormat(QImage::Format_Mono, Qt::ThresholdDither | Qt::AvoidDither);

    if(finalImage.isNull()) {
        return false;
    }

    finalImage = finalImage.convertToFormat(QImage::Format_RGB32);
    prn.setFullPage(true);

    QPainter painter(&prn);

    if(!painter.isActive()) {
        return false;
    }

    const QRect page = prn.pageRect(QPrinter::DevicePixel).toRect();

    if(page.width() < 1 || page.height() < 1) {
        painter.end();
        return false;
    }

    const qreal scale = (qreal)page.width() / (qreal)finalImage.width();
    const int maxSrcPerPage = qMax(1, qRound((qreal)page.height() / scale));
    int srcY = 0;
    bool first = true;

    while(srcY < finalImage.height()) {
        if(!first) {
            if(!prn.newPage()) {
                break;
            }
        }

        first = false;
        const int take = qMin(maxSrcPerPage, finalImage.height() - srcY);
        const QImage strip = finalImage.copy(0, srcY, finalImage.width(), take);
        const int outH = qMax(1, qRound(take * scale));
        painter.drawImage(QRect(0, 0, page.width(), outH), strip);
        srcY += take;
    }

    painter.end();
    return srcY >= finalImage.height();
}
