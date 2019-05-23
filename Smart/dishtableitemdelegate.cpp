#include "dishtableitemdelegate.h"
#include "dish.h"
#include <QPainter>

DishTableItemDelegate::DishTableItemDelegate()
{

}


void DishTableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }
    Dish *d = index.data(Qt::UserRole).value<Dish*>();
    if (d == nullptr) {
        return;
    }
    painter->save();
    painter->setPen(Qt::gray);
    painter->setBrush(QBrush(QColor::fromRgb(d->color)));
    painter->drawRect(option.rect);

    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::black);
    QRectF rText = option.rect;
    rText.adjust(2, 2, -2, 2);
    painter->drawText(rText, d->name);
    QRectF rPrice = option.rect;
    QFontMetrics fm(painter->font());
    QString price = QString::number(d->price, 'f', 0);
    rPrice.adjust(rPrice.width() - fm.width(price) - 2, rPrice.height() - fm.height() - 2, -2, -2);
    painter->drawText(rPrice, price);
    painter->restore();
}
