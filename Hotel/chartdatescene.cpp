#include "chartdatescene.h"
#include "chartparams.h"
#include "chartitemrect.h"
#include "chartitemmonthrect.h"
#include <QDate>

ChartDateScene::ChartDateScene(QObject *parent) :
    QGraphicsScene(parent)
{

}

void ChartDateScene::setDateRange(const QDate &d1, const QDate &d2)
{
    clear();
    int days = d1.daysTo(d2);
    setSceneRect(0, 0, days * srect_side, srect_side * 2);
    for (int i = 0; i < days; i++) {
        addItem(new ChartItemRect(d1.addDays(i), i * srect_side, srect_side, srect_side, srect_side));
    }
    QDate d = d1;
    int totalDays = 0;
    do {
        days = d.daysInMonth();
        if (d.month() == d1.month()) {
            days -= d1.day() - 1;
        } else if (d.month() == d2.month()) {
            days = d2.day();
        } else {
            days = d.daysInMonth();
        }
        addItem(new ChartItemMonthRect(days, d, totalDays * srect_side));
        totalDays += days;
        d = d.addDays(days);
    } while (d < d2);
}
