#include "qdishpart2button.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionButton>

QDishPart2Button::QDishPart2Button(QWidget *parent) :
    QPushButton(parent)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setMinimumSize(QSize(95, 60));
    setMaximumSize(QSize(9005, 60));
}

void QDishPart2Button::paintEvent(QPaintEvent *pe)
{
    QPushButton::paintEvent(pe);
    QPainter p(this);
    QPalette pal = palette();
    QStyleOptionButton option;
    option.initFrom(this);
    QColor bgcolor = option.state == QStyle::State_Sunken ? Qt::white : pal.color(QPalette::Base);
    QBrush b(bgcolor);
    QRect r = pe->rect();
    p.fillRect(r, b);
    p.setPen(QColor::fromRgb(44, 67, 131));
    p.drawRect(r);
    r.adjust(2, 2, -4, -4);
    QTextOption o;
    o.setWrapMode(QTextOption::WordWrap);
    o.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    p.setPen(Qt::black);
    p.drawText(r, text(), o);
}
