#pragma once

#include <QLabel>
#include <QPixmap>
#include <QString>

inline QPixmap loadDishPixmap(const QString &path)
{
    QPixmap pm(path);
    if (!pm.isNull()) {
        return pm;
    }
    return QPixmap(QStringLiteral(":/res/dish_placeholder.png"));
}

inline QPixmap scaleDishPixmapToHeight(const QPixmap &source, int targetHeight)
{
    if (source.isNull() || targetHeight <= 0) {
        return source;
    }
    return source.scaledToHeight(targetHeight, Qt::SmoothTransformation);
}

inline void setDishImageOnLabel(QLabel *label, const QString &path, int targetHeight, int maxWidth = 0)
{
    if (!label || targetHeight <= 0) {
        return;
    }

    const QPixmap pm = scaleDishPixmapToHeight(loadDishPixmap(path), targetHeight);
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
