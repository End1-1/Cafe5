#include "dishitemdelegate.h"
#include "dish.h"
#include "c5utils.h"
#include <QPainter>

static const QColor selColor = QColor::fromRgb(174, 217, 255);

DishItemDelegate::DishItemDelegate()
{

}

void DishItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    QFont f(painter->font());
    f.setBold(true);
    painter->setFont(f);
    QColor bgColor = (option.state & QStyle::State_Selected) ? selColor : Qt::white;
    painter->fillRect(option.rect, bgColor);
    painter->drawRect(option.rect);
    if (index.isValid()) {
        Dish d = index.data(Qt::UserRole).value<Dish>();
        QRectF rText = option.rect;
        rText.adjust(2, 2, -80, -2);
        painter->drawText(rText, d.name);
        int xDiv = option.rect.width() - 80;
        int yDiv = option.rect.height() / 2;
        QRectF rQtyPrice = option.rect;
        rQtyPrice.adjust(xDiv + 2, 2, -2, -1 * yDiv);
        painter->drawText(rQtyPrice, QString ("%1 X %2").arg(float_str(d.qty, 2)).arg(float_str(d.price, 2)));
        QRectF rTotal = option.rect;
        rTotal.adjust(xDiv + 2, 2 + yDiv, -2, -2);
        painter->drawText(rTotal, float_str(d.qty * d.price, 2));
        painter->setPen(QPen(QColor::fromRgb(140, 140, 140)));
        painter->drawLine(option.rect.left() + xDiv, option.rect.top(), option.rect.left() + xDiv, option.rect.bottom());
        painter->drawLine(option.rect.left() + xDiv, option.rect.top() + yDiv, option.rect.width(), option.rect.top() + yDiv);
    }
    painter->restore();
}
