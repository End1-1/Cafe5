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
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(11);
    painter->setFont(font);
    QString text = index.data(Qt::EditRole).toString();
    QRect rt = option.rect;
    painter->drawRect(rt);
    rt.adjust(3, 3, -3, -3);
    QTextOption to;
    painter->drawText(rt, text, to);
    painter->restore();
}
