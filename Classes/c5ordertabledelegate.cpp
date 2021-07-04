#include "c5ordertabledelegate.h"
#include "c5orderdriver.h"
#include "datadriver.h"
#include "c5utils.h"
#include <QJsonObject>
#include <QPainter>

C5OrderTableDelegate::C5OrderTableDelegate(C5OrderDriver *od)
{
    fOrder = od;
}

void C5OrderTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }
    int r = index.row();
    painter->save();
    QBrush bgBrush(Qt::white, Qt::SolidPattern);
    QPen pen(Qt::black, Qt::SolidLine);
    if (option.state & QStyle::State_Selected) {
        bgBrush.setColor(QColor::fromRgb(65, 65, 250));
        pen.setColor(Qt::white);
    }
    painter->setPen(pen);
    painter->fillRect(option.rect, bgBrush);
    QRect nameRect = option.rect;
    nameRect.adjust(1, 2, -75, -2);
    painter->drawText(nameRect, QString("%1. [%2] %3").arg(r + 1).arg(fOrder->dishesValue("f_timeorder", r).toInt()).arg(dbdish->name(fOrder->dishesValue("f_dish", r).toInt())));
    if (fOrder->dishesValue("f_comment", r).toString().length() > 0) {
        QRect commentRect = option.rect;
        QFont f(painter->font());
        f.setPointSize(f.pointSize() - 3);
        painter->setFont(f);
        QFontMetrics fm(f);
        commentRect.adjust(2, commentRect.height() - fm.height() - 2, -75, -2);
        painter->drawText(commentRect, fOrder->dishesValue("f_comment", r).toString());
        f.setPointSize(f.pointSize() + 3);
        painter->setFont(f);
    }
    int y = option.rect.top() + (option.rect.height() / 2);
    int x = option.rect.right() - 70;
    painter->drawLine(x, y, option.rect.right(), y);
    x = option.rect.right() - 35;
    y = option.rect.top() + option.rect.height() / 2;
    painter->drawLine(x, option.rect.top() + 2, x, y);
    QTextOption op;
    op.setAlignment(Qt::AlignHCenter);
    QRect re = option.rect;
    re.adjust(re.width() - 70, 2, -35, 0);
    painter->drawText(re, float_str(fOrder->dishesValue("f_qty1", r).toDouble(), 2), op);
    re = option.rect;
    re.adjust(re.width() - 35, 2, 0, 0);
    painter->drawText(re, float_str(fOrder->dishesValue("f_qty2", r).toDouble(), 2), op);
    re = option.rect;
    re.adjust(re.width() - 70, (re.height() / 2) + 5, 0, 0);
    painter->drawText(re, float_str(fOrder->dishesValue("f_total", r).toDouble(), 2), op);
    painter->restore();
}
