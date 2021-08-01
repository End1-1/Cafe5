#include "dishitembutton.h"
#include <QPaintEvent>
#include <QPainter>

DishItemButton::DishItemButton(QWidget *parent) :
    QPushButton(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

void DishItemButton::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    QRect r = e->rect();
    r.adjust(2, 2, -2, -2);
    QTextOption o;
    o.setWrapMode(QTextOption::WordWrap);
    p.drawText(r, text(), o);
}
