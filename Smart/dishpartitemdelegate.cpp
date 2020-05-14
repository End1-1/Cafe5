#include "dishpartitemdelegate.h"
#include <QPainter>

DishPartItemDelegate::DishPartItemDelegate()
{

}

void DishPartItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }
    painter->save();
    painter->setBrush(QColor::fromRgb(index.data(Qt::BackgroundColorRole).toInt()));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(11);
    painter->setFont(font);
    QString text = index.data(Qt::EditRole).toString();
    QRect rt = option.rect;
    painter->drawRect(rt);
    QTextOption to;
    if (option.state & QStyle::State_Selected) {
        painter->setPen(Qt::red);
        rt = option.rect;
        rt.adjust(5, 5, -3, -3);
        painter->drawText(rt, text, to);
        painter->setPen(Qt::black);
        rt = option.rect;
        rt.adjust(3, 3, -3, -3);
        painter->drawText(rt, text, to);
    } else {
        rt.adjust(3, 3, -3, -3);
        painter->drawText(rt, text, to);
    }
    painter->restore();
}
