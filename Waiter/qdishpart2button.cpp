#include "qdishpart2button.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionButton>

QDishPart2Button::QDishPart2Button(QWidget *parent) :
    QPushButton(parent)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    setMinimumSize(QSize(122, 60));
    setMaximumSize(QSize(16777215, 60));
}

void QDishPart2Button::paintEvent(QPaintEvent *pe)
{
    QPushButton::paintEvent(pe);
    QPainter p(this);
    QStyleOptionButton option;
    option.initFrom(this);
    QColor bgcolor = QColor::fromRgb(property("bgcolor").toInt());
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
