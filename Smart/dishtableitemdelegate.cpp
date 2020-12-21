#include "dishtableitemdelegate.h"
#include "dish.h"
#include <QPainter>

static QPixmap *minusPixmap = nullptr;
static QPixmap *dangerPixmap = nullptr;

DishTableItemDelegate::DishTableItemDelegate()
{
    if (minusPixmap == nullptr) {
        minusPixmap = new QPixmap(":/minus.png");
        dangerPixmap = new QPixmap(":/danger.png");
    }
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

    int iconright = 22;
    if (d->netWeight < 0.00001) {
        painter->drawPixmap(option.rect.right() - iconright, option.rect.top() + 2, 20, 20, *minusPixmap);
        iconright += 22;
    }

    if (d->price <= d->cost) {
        painter->drawPixmap(option.rect.right() - iconright, option.rect.top() +2, 20, 20, *dangerPixmap);
        iconright += 22;
    }
    painter->restore();
}
