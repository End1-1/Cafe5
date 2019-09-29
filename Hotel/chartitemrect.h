#ifndef CHARTITEMRECT_H
#define CHARTITEMRECT_H

#include <QGraphicsItem>
#include <QDate>

class ChartItemRect : public QGraphicsItem
{
public:
    ChartItemRect(const QDate &date, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;

private:
    QRectF fRect;
    QDate fDate;
};


#endif // CHARTITEMRECT_H
