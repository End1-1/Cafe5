#include "qdishbutton.h"
#include "c5utils.h"
#include "../WaiterDesigner/waitergoodsdishstyle.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionButton>

QDishButton::QDishButton(DishAItem *dish, int minWidth, QWidget *parent) :
    QFrame(parent),
    mDish(dish)
{
    setObjectName(QStringLiteral("goodsDishFrame"));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    const WaiterGoodsDishStyle &style = WaiterGoodsDishStyle::cachedStyle();
    const int h = qBound(40, style.height, 200);
    setMinimumSize(QSize(minWidth, h));
    setMaximumHeight(h);
    QFont f = font();
    f.setPointSize(qBound(7, style.fontSize, 48));
    f.setBold(style.fontBold);
    setFont(f);
}

void QDishButton::paintEvent(QPaintEvent *pe)
{
    QFrame::paintEvent(pe);
    const WaiterGoodsDishStyle &style = WaiterGoodsDishStyle::cachedStyle();
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
    const QColor defaultBg = style.colorDefault;
    // Goods color has priority over WaiterGoodsDishStyle default.
    // Sentinel: 0 / -1 = unset. Opaque QRgb values are usually negative (alpha 0xFF).
    // Positive values are treated as 24-bit RGB without alpha.
    QColor bgcolor = defaultBg;
    if(mDish->color != 0 && mDish->color != -1) {
        QRgb rgb = static_cast<QRgb>(mDish->color);
        if((rgb & 0xff000000u) == 0) {
            rgb |= 0xff000000u;
        }
        bgcolor = QColor::fromRgb(rgb);
    }
    if(option.state & QStyle::State_Sunken) {
        bgcolor = bgcolor.darker(115);
    }
    p.fillRect(pe->rect(), stopreached ? qRgba(0xee, 0xee, 0xee, 0) : bgcolor);
    QRect r = pe->rect();
    if(style.borderWidth > 0) {
        p.setPen(QPen(QColor(QLatin1String(WaiterGoodsDishStyle::borderColorCss)), style.borderWidth));
        p.drawRect(r.adjusted(0, 0, -1, -1));
    }
    r.adjust(2, 2, -4, -4);
    QRect rectName = pe->rect();
    rectName.adjust(2, 2, -2, -2);
    QTextOption to;
    to.setWrapMode(QTextOption::WordWrap);
    to.setAlignment(Qt::AlignLeft);
    QFont f = p.font();
    f.setPointSize(qBound(7, style.fontSize, 48));
    f.setBold(style.fontBold);
    p.setFont(f);
    p.setPen(style.fontColor);
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
        QRect emarkRect = pe->rect();
        emarkRect.adjust(emarkRect.width() - 2 - emarkRect.height() + fm.height(), 2, - 2, - (fm.height() + 5));
        QPixmap px(":/qrcode.png");
        p.drawPixmap(emarkRect, px.scaled(emarkRect.width(), emarkRect.height()));
    }
}

void QDishButton::mouseReleaseEvent(QMouseEvent *e)
{
    const int noClickZone = 50;

    if(e->pos().x() < width() - noClickZone) {
        emit clicked();
    }

    QFrame::mouseReleaseEvent(e);
}
