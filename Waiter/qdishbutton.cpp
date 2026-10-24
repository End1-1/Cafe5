#include "qdishbutton.h"
#include "c5utils.h"
#include "newlook.h"
#include "../WaiterDesigner/waitergoodsdishstyle.h"
#include <QPainter>
#include <QPainterPath>
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
    if (WaiterUi::newLook()) {
        setMinimumSize(QSize(minWidth, 156));
        setMaximumHeight(156);
    }
}

void QDishButton::paintNewLook(QPaintEvent *pe)
{
    Q_UNUSED(pe);
    if (!mCardPhotoDecoded) {
        mCardPhotoDecoded = true;
        const QString img = mDish->image.trimmed();
        if (!img.isEmpty()) {
            const QByteArray bytes = QByteArray::fromBase64(img.toLatin1());
            QPixmap px;
            if (px.loadFromData(bytes)) {
                mCardPhoto = px;
            }
        }
    }

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    const QRectF card = QRectF(rect()).adjusted(4, 4, -4, -4);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(QStringLiteral("#f7f7f7")));
    p.drawRoundedRect(card, 12, 12);

    const QRectF photo = card.adjusted(6, 6, -6, -42);
    p.setBrush(QColor(QStringLiteral("#d5e2df")));
    p.drawRoundedRect(photo, 8, 8);
    if (!mCardPhoto.isNull()) {
        QPainterPath clip;
        clip.addRoundedRect(photo, 8, 8);
        p.setClipPath(clip);
        p.drawPixmap(photo.toRect(), mCardPhoto);
        p.setClipping(false);
    } else {
        const QPixmap icon(QStringLiteral(":/dish.png"));
        if (!icon.isNull()) {
            QRect ir = photo.toRect();
            ir.adjust(ir.width() / 4, ir.height() / 4, -ir.width() / 4, -ir.height() / 4);
            p.drawPixmap(ir, icon);
        }
    }

    const QString priceText = float_str(mDish->price, 2);
    QFont pf = font();
    pf.setBold(true);
    pf.setPointSize(11);
    p.setFont(pf);
    const QFontMetrics fm(pf);
    const int badgeW = fm.horizontalAdvance(priceText) + 16;
    const int badgeH = fm.height() + 8;
    const QRectF badge(photo.right() - badgeW - 6, photo.top() + 6, badgeW, badgeH);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    p.drawRoundedRect(badge, 8, 8);
    p.setPen(QColor(QStringLiteral("#1a1a1a")));
    p.drawText(badge, Qt::AlignCenter, priceText);

    QFont nf = font();
    nf.setBold(true);
    nf.setPointSize(10);
    p.setFont(nf);
    p.setPen(QColor(QStringLiteral("#1c1c1c")));
    const QRectF nameRect(card.left() + 8, photo.bottom() + 2, card.width() - 16, 36);
    QString name = mDish->name;
    const bool stop = mDish->stoplist > -1;
    bool stopreached = false;
    if (stop) {
        if (mDish->stoplist == 0) {
            stopreached = true;
        } else {
            name = QStringLiteral("[%1] %2").arg(float_str(static_cast<double>(mDish->stoplist), 2), name);
        }
    }
    p.drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, name);
    if (stopreached) {
        const QPixmap px(QStringLiteral(":/stop.png"));
        if (!px.isNull()) {
            const QRect stopRect(photo.center().toPoint() - QPoint(24, 24), QSize(48, 48));
            p.setOpacity(0.55);
            p.drawPixmap(stopRect, px);
        }
    }
}

void QDishButton::paintEvent(QPaintEvent *pe)
{
    if (WaiterUi::newLook()) {
        paintNewLook(pe);
        return;
    }
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

    if(WaiterUi::newLook() || e->pos().x() < width() - noClickZone) {
        emit clicked();
    }

    QFrame::mouseReleaseEvent(e);
}
