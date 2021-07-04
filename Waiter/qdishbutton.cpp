#include "qdishbutton.h"
#include "c5menu.h"
#include "c5utils.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionButton>

QDishButton::QDishButton(QWidget *parent) :
    QPushButton(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setMinimumSize(QSize(120, 60));
    setMaximumHeight(60);
}

void QDishButton::paintEvent(QPaintEvent *pe)
{
    QPushButton::paintEvent(pe);

    const QJsonObject &o = property("dish").toJsonObject();
    bool stop = C5Menu::fStopList.contains(o["f_dish"].toString().toInt());
    bool stopreached = false;
    QString name = o["f_name"].toString();
    if (stop) {
        double qty = C5Menu::fStopList[o["f_dish"].toString().toInt()];
        if (qty < 0.001) {
            stopreached = true;
        } else {
            name = QString("[%1] %5").arg(float_str(qty, 2)).arg(name);
        }
    }

    QPainter p(this);
    QStyleOptionButton option;
    option.initFrom(this);
    QColor bgcolor = option.state == QStyle::State_Sunken ? Qt::white : QColor::fromRgb(o["dish_color"].toString().toInt());
    p.fillRect(pe->rect(), stopreached ? qRgba(0xee,0xee,0xee, 0) : bgcolor);
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
    QString price = o["f_price"].toString();
    rectPrice.adjust(rectPrice.width() - (fm.width(price) + 5), rectPrice.height() - (fm.height() + 5), -2, -2);
    p.drawText(rectPrice, price);

    if (stopreached) {
        QRect stopRect = pe->rect();
        stopRect.adjust((stopRect.width() / 2) - 28, (stopRect.height() / 2) - 28, 28 - (stopRect.width() / 2), 28 - (stopRect.height() / 2));
        QPixmap px(":/stop.png");
        p.drawPixmap(stopRect, px);
    }
}
