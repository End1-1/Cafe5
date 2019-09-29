#include "chartitemrect.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QApplication>

static QMap<int, QString> DAYS_OF_WEEK;
static const QColor LIGHT_GRAY = QColor::fromRgb(72, 72, 72);
static const QColor VSELECTION_COLOR = QColor::fromRgb(0, 255, 0, 100);

ChartItemRect::ChartItemRect(const QDate &date, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    if (DAYS_OF_WEEK.count() == 0) {
        DAYS_OF_WEEK[1] = "Mo";
        DAYS_OF_WEEK[2] = "Tu";
        DAYS_OF_WEEK[3] = "We";
        DAYS_OF_WEEK[4] = "Th";
        DAYS_OF_WEEK[5] = "Fr";
        DAYS_OF_WEEK[6] = "Sa";
        DAYS_OF_WEEK[7] = "Su";
    }
    fRect = QRectF(x, y, w, h);
    fDate = date;
}

void ChartItemRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (option->state & QStyle::State_Selected) {
        QRect selRect = option->rect;
        painter->fillRect(selRect, VSELECTION_COLOR);
    } else {
        painter->fillRect(option->rect, Qt::white);
    }
    painter->setPen(LIGHT_GRAY);
    painter->drawRect(option->rect);
    if (fDate.dayOfWeek() == 6 || fDate.dayOfWeek() == 7) {
        painter->setPen(Qt::red);
    } else {
        painter->setPen(LIGHT_GRAY);
    }
    QTextOption to;
    to.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QFont f(qApp->font().family(), 8);
    f.setBold(true);
    painter->setFont(f);
    QRect dayRect = option->rect;
    dayRect.adjust(0, 0, 0, (option->rect.height() / 2) * -1);
    QString day = QString::number(fDate.day());
    painter->drawText(dayRect, day, to);

    QRect dayOfWeekRect = option->rect;
    dayOfWeekRect.adjust(0, option->rect.height() / 2, 0, 0);
    QString dayOfWeek = DAYS_OF_WEEK[fDate.dayOfWeek()];
    painter->drawText(dayOfWeekRect, dayOfWeek, to);
}

QRectF ChartItemRect::boundingRect() const
{
    return fRect;
}
