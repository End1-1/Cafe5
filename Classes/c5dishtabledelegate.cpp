#include "c5dishtabledelegate.h"
#include <QJsonObject>
#include <QPainter>

C5DishTableDelegate::C5DishTableDelegate()
{

}

void C5DishTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        painter->fillRect(option.rect, Qt::white);
        return;
    }
    QJsonObject o = index.data(Qt::UserRole).toJsonObject();
    if (o.count() == 0) {
        QItemDelegate::paint(painter, option, index);
        return;
    }
    painter->save();
    painter->fillRect(option.rect, QColor::fromRgb(o["dish_color"].toString().toInt()));
    QRect rectName = option.rect;
    rectName.adjust(2, 2, -2, -2);
    painter->drawText(rectName, o["f_name"].toString());
    QRect rectPrice = option.rect;
    QFontMetrics fm(painter->font());
    QString price = o["f_price"].toString();
    rectPrice.adjust(rectPrice.width() - (fm.width(price) + 5), rectPrice.height() - (fm.height() + 5), -2, -2);
    painter->drawText(rectPrice, price);
    painter->restore();
}
