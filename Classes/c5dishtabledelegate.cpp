#include "c5dishtabledelegate.h"
#include <QJsonObject>
#include <QPainter>

C5DishTableDelegate::C5DishTableDelegate()
{

}

void C5DishTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }
    QJsonObject o = index.data(Qt::UserRole).toJsonObject();
    QRect rectName = option.rect;
    rectName.adjust(2, 2, -2, -2);
    painter->drawText(rectName, o["f_name"].toString());
    QRect rectPrice = option.rect;
    rectPrice.adjust(rectPrice.width() - 50, rectPrice.height() - 20, -2, -2);
    painter->drawText(rectPrice, o["f_price"].toString());
}
