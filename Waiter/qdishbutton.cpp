#include "qdishbutton.h"
#include "c5utils.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionButton>

QDishButton::QDishButton(DishAItem *dish, int minWidth, QWidget *parent) :
    QFrame(parent),
    mDish(dish)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setMinimumSize(QSize(minWidth, 60));
    setMaximumHeight(60);
}

void QDishButton::paintEvent(QPaintEvent *pe)
{
    QFrame::paintEvent(pe);
    bool stop = mDish->stoplist > -1;
    bool stopreached = false;
    QString name = mDish->name;

    if(stop) {
        if(mDish->stoplist == 0) {
            stopreached = true;
        } else {
            name = QString("[%1] %5").arg(float_str((double) mDish->stoplist, 2)).arg(name);
        }
    }

    QPainter p(this);
    QStyleOptionButton option;
    option.initFrom(this);
    QColor bgcolor = option.state == QStyle::State_Sunken ? Qt::white : QColor::fromRgb(mDish->color < -1 ? mDish->color : -1);
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
    QString price = float_str(mDish->price, 2);
    rectPrice.adjust(rectPrice.width() - (fm.horizontalAdvance(price) + 5), rectPrice.height() - (fm.height() + 5), -2, -2);
    p.drawText(rectPrice, price);

    if(stopreached) {
        QRect stopRect = pe->rect();
        stopRect.adjust((stopRect.width() / 2) - 28, (stopRect.height() / 2) - 28, 28 - (stopRect.width() / 2),
                        28 - (stopRect.height() / 2));
        QPixmap px(":/stop.png");
        QPixmap transparent(px.size());
        transparent.fill(Qt::transparent);
        QPainter pp(&transparent);
        pp.setOpacity(0.5);
        pp.drawPixmap(0, 0, px);
        pp.end();
        p.drawPixmap(stopRect, transparent);
    }

    if(mDish->emarkRequired) {
        qDebug() << fm.height();
        QRect emarkRect = pe->rect();
        emarkRect.adjust(emarkRect.width() - 2 - emarkRect.height() + fm.height(), 2, - 2, - (fm.height() + 5));
        QPixmap px(":/qrcode.png");
        p.drawPixmap(emarkRect, px.scaled(emarkRect.width(), emarkRect.height()));
    }
}

void QDishButton::mouseReleaseEvent(QMouseEvent *e)
{
    emit clicked();
    QFrame::mouseReleaseEvent(e);
}
