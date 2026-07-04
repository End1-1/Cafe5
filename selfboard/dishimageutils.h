#pragma once

#include <QLabel>
#include <QPixmap>
#include <QString>

#include <QImage>

inline QPixmap loadDishPixmap(const QString &path)
{
    QPixmap pm(path);
    if (!pm.isNull()) {
        return pm;
    }
    return QPixmap(QStringLiteral(":/res/dish_placeholder.png"));
}

inline QPixmap pixmapPreservingAlpha(const QPixmap &source,
                                     int width,
                                     int height,
                                     Qt::AspectRatioMode mode = Qt::KeepAspectRatio)
{
    if (source.isNull() || width <= 0 || height <= 0) {
        return source;
    }

    QImage image = source.toImage().convertToFormat(QImage::Format_ARGB32);
    image = image.scaled(width, height, mode, Qt::SmoothTransformation);
    return QPixmap::fromImage(image);
}

inline void styleTransparentTextLabel(QLabel *label)
{
    if (!label) {
        return;
    }

    label->setAutoFillBackground(false);
    label->setAttribute(Qt::WA_StyledBackground, false);
    label->setStyleSheet(QStringLiteral("background: transparent; border: none;"));
}

inline void styleTransparentImageLabel(QLabel *label)
{
    styleTransparentTextLabel(label);
    label->setAttribute(Qt::WA_TranslucentBackground, true);
}

inline QPixmap scaleDishPixmapToHeight(const QPixmap &source, int targetHeight)
{
    if (source.isNull() || targetHeight <= 0) {
        return source;
    }
    return pixmapPreservingAlpha(source, targetHeight, targetHeight, Qt::KeepAspectRatio);
}

inline void setCenteredDishImageOnLabel(QLabel *label, const QString &path, int boxWidth, int boxHeight)
{
    if (!label || boxWidth <= 0 || boxHeight <= 0) {
        return;
    }

    styleTransparentImageLabel(label);
    label->setFixedSize(boxWidth, boxHeight);
    label->setScaledContents(false);
    label->setAlignment(Qt::AlignCenter);
    label->setPixmap(pixmapPreservingAlpha(loadDishPixmap(path), boxWidth, boxHeight, Qt::KeepAspectRatio));
}

inline void setDishImageOnLabel(QLabel *label, const QString &path, int targetHeight, int maxWidth = 0)
{
    if (!label || targetHeight <= 0) {
        return;
    }

    const QPixmap source = loadDishPixmap(path);
    const QPixmap pm = scaleDishPixmapToHeight(source, targetHeight);
    styleTransparentImageLabel(label);
    label->setScaledContents(false);
    label->setAlignment(Qt::AlignCenter);

    int width = pm.isNull() ? targetHeight : qMax(1, pm.width());
    if (maxWidth > 0) {
        width = qMin(width, maxWidth);
    }

    label->setFixedHeight(targetHeight);
    label->setFixedWidth(width);
    label->setPixmap(pm);
}

inline void setTransparentPixmapOnLabel(QLabel *label,
                                        const QPixmap &source,
                                        int width,
                                        int height)
{
    if (!label || width <= 0 || height <= 0) {
        return;
    }

    styleTransparentImageLabel(label);
    label->setFixedSize(width, height);
    label->setScaledContents(false);
    label->setAlignment(Qt::AlignCenter);
    label->setPixmap(pixmapPreservingAlpha(source, width, height));
}
