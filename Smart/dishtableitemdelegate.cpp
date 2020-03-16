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
    QFont font(painter->font());
    font.setBold(true);
    font.setPointSize(11);
    painter->setFont(font);

    painter->setPen(Qt::gray);
    painter->setBrush(QBrush(QColor::fromRgb(d->color)));
    QRect orect = option.rect;
    painter->drawRect(orect);

    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::black);
    QRectF rText = option.rect;
    QFontMetrics fm(painter->font());
    rText.adjust(4, 4, -1 * (fm.width("9999") - 2), -2);
    painter->drawText(rText, d->name);
    QRectF rPrice = option.rect;
    QString price = QString::number(d->price, 'f', 0);
    rPrice.adjust(rPrice.width() - fm.width(price) - 2, rPrice.height() - fm.height() - 2, -2, -2);
    painter->drawText(rPrice, price);
    painter->restore();
}
