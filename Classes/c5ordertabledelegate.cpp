#include "c5ordertabledelegate.h"
#include <QJsonObject>
#include <QPainter>

C5OrderTableDelegate::C5OrderTableDelegate()
{

}

void C5OrderTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }
    QBrush bgBrush(Qt::white, Qt::SolidPattern);
    QPen pen(Qt::black, Qt::SolidLine);
    if (option.state & QStyle::State_Selected) {
        bgBrush.setColor(QColor::fromRgb(65, 65, 250));
        pen.setColor(Qt::white);
    }
    painter->setPen(pen);
    painter->fillRect(option.rect, bgBrush);
    QJsonObject o = index.data(Qt::UserRole).toJsonObject();
    QRect nameRect = option.rect;
    nameRect.adjust(1, 2, -75, -2);
    painter->drawText(nameRect, QString("%1. %2").arg(index.row() + 1).arg(o["f_name"].toString()));
    int y = option.rect.top() + (option.rect.height() / 2);
    int x = option.rect.right() - 70;
    painter->drawLine(x, y, option.rect.right(), y);
    x = option.rect.right() - 35;
    y = option.rect.top() + option.rect.height() / 2;
    painter->drawLine(x, option.rect.top() + 2, x, y);
    QTextOption op;
    op.setAlignment(Qt::AlignHCenter);
    QRect r = option.rect;
    r.adjust(r.width() - 70, 2, -35, 0);
    painter->drawText(r, o["f_qty1"].toString(), op);
    r = option.rect;
    r.adjust(r.width() - 35, 2, 0, 0);
    painter->drawText(r, o["f_qty2"].toString(), op);
    r = option.rect;
    r.adjust(r.width() - 70, (r.height() / 2) + 5, 0, 0);
    painter->drawText(r, o["f_total"].toString(), op);
}
