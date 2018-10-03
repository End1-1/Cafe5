#include "c5halltabledelegate.h"
#include <QJsonObject>
#include <QPainter>

C5HallTableDelegate::C5HallTableDelegate()
{

}

void C5HallTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }
    QJsonObject o = index.data(Qt::UserRole).toJsonObject();
    QBrush bgBrush(Qt::white, Qt::SolidPattern);
    if (o["f_lock"].toString().toInt() > 0) {
        bgBrush.setColor(QColor::fromRgb(255, 151, 151));
    }
    QRect fillRect = option.rect;
    fillRect.adjust(2, 2, -2, -2);
    painter->fillRect(fillRect, bgBrush);
    QRect tableRect = fillRect;
    tableRect.adjust(2, 2, -2, -2);
    painter->drawText(tableRect, o["f_name"].toString());
}
