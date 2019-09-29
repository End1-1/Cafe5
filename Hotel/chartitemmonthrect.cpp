#include "chartitemmonthrect.h"
#include "chartparams.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>

ChartItemMonthRect::ChartItemMonthRect(int len, const QDate &d, qreal x, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    fRect = QRectF(x, 0, len * srect_side, srect_side);
    fDate = d.toString("MMMM, yy");
}

QRectF ChartItemMonthRect::boundingRect() const
{
    return fRect;
}

void ChartItemMonthRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    painter->fillRect(option->rect, Qt::white);
    painter->drawRect(option->rect);
    QTextOption to;
    to.setAlignment(Qt::AlignCenter);
    painter->drawText(option->rect, fDate, to);
}
