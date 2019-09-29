#ifndef CHARTITEMMONTHRECT_H
#define CHARTITEMMONTHRECT_H

#include <QGraphicsItem>
#include <QDate>

class ChartItemMonthRect : public QGraphicsItem
{
public:
    ChartItemMonthRect(int len, const QDate &d, qreal x, QGraphicsItem *parent = nullptr);
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QString fDate;
    QRectF fRect;
};

#endif // CHARTITEMMONTHRECT_H
