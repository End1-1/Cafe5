#include "qdishbutton.h"
#include "c5tabledata.h"
#include "c5utils.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionButton>

QDishButton::QDishButton(int minWidth, QWidget *parent) :
    QPushButton(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setMinimumSize(QSize(minWidth, 60));
    setMaximumHeight(60);
}

void QDishButton::paintEvent(QPaintEvent *pe)
{
    QPushButton::paintEvent(pe);
    int id = property("id").toInt();
    bool stop = C5TableData::instance()->mStopList.contains(id);
    bool stopreached = false;
    QString name = property("name").toString();
    if (stop) {
        double qty = C5TableData::instance()->mStopList[id];
        if (qty < 0.001) {
            stopreached = true;
        } else {
            name = QString("[%1] %5").arg(float_str(qty, 2)).arg(name);
        }
    }
    QPainter p(this);
    QStyleOptionButton option;
    option.initFrom(this);
    QColor bgcolor = option.state == QStyle::State_Sunken ? Qt::white : QColor::fromRgb(property("color").toInt());
    p.fillRect(pe->rect(), stopreached ? qRgba(0xee, 0xee, 0xee, 0) : bgcolor);
    QRect r = pe->rect();
    p.setPen(QColor::fromRgb(44, 67, 131));
    p.drawRect(r);
    r.adjust(2, 2, -4, -4);
    QRect rectName = pe->rect();
    rectName.adjust(2, 2, -2, -2);
    QTextOption to;
    to.setWrapMode(QTextOption::WordWrap);
    to.setAlignment(Qt::AlignLeft);
    p.setPen(Qt::black);
    p.drawText(rectName, name, to);
    QRect rectPrice = option.rect;
    QFontMetrics fm(p.font());
    QString price = float_str(property("price").toDouble(), 2);
    rectPrice.adjust(rectPrice.width() - (fm.width(price) + 5), rectPrice.height() - (fm.height() + 5), -2, -2);
    p.drawText(rectPrice, price);
    if (stopreached) {
        QRect stopRect = pe->rect();
        stopRect.adjust((stopRect.width() / 2) - 28, (stopRect.height() / 2) - 28, 28 - (stopRect.width() / 2),
                        28 - (stopRect.height() / 2));
        QPixmap px(":/stop.png");
        p.drawPixmap(stopRect, px);
    }
}
