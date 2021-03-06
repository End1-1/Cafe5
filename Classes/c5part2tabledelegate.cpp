#include "c5part2tabledelegate.h"
#include <QPainter>
#include <QApplication>

C5Part2TableDelegate::C5Part2TableDelegate()
{

}

void C5Part2TableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }
    painter->save();
    QColor bgcolor = QColor::fromRgb(index.data(Qt::UserRole).toInt());
    painter->fillRect(option.rect, bgcolor);
    QRect textRect = option.rect;
    textRect.adjust(2, 2, -2, -2);
    QFont f(qApp->font());
    f.setBold(true);
    painter->setFont(f);
    painter->drawText(textRect, index.data(Qt::DisplayRole).toString());
    painter->restore();
}
