#include "goodsgroupbutton.h"
#include <QLabel>
#include <QFontMetrics>
#include <QResizeEvent>
#include <QShowEvent>
#include <QVBoxLayout>
#include <QStyle>

GoodsGroupButton::GoodsGroupButton(const QString &text, QWidget *parent) :
    QFrame(parent)
{
    setObjectName("goodsGroupFrame");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMinimumHeight(48);
    setMaximumHeight(48);
    setMinimumWidth(0);
    mLabel = new QLabel(text, this);
    mLabel->setObjectName("goodsGroupName");
    mLabel->setWordWrap(true);
    mLabel->setAlignment(Qt::AlignCenter);
    mLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    mLabel->setMinimumWidth(0);
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(3, 3, 3, 3);
    lay->addWidget(mLabel);
}

void GoodsGroupButton::cacheBaseFontSize()
{
    if(mBasePointSize > 0) {
        return;
    }

    const QFont &f = mLabel->font();

    if(f.pointSize() > 0) {
        mBasePointSize = f.pointSize();
    } else if(f.pixelSize() > 0) {
        mBasePointSize = qMax(7, qRound(f.pixelSize() * 72.0 / qreal(logicalDpiY())));
    } else {
        mBasePointSize = 11;
    }
}

void GoodsGroupButton::scaleLabelFont()
{
    if(!mLabel || mLabel->text().isEmpty()) {
        return;
    }

    cacheBaseFontSize();

    const int w = mLabel->width() - 4;
    const int h = mLabel->height() - 4;

    if(w <= 0 || h <= 0) {
        return;
    }

    const QString t = mLabel->text();
    const int minPt = 7;
    const QFont baseFont = mLabel->font();
    QFont f = baseFont;
    int chosen = minPt;

    for(int pt = mBasePointSize; pt >= minPt; --pt) {
        f = baseFont;
        f.setPointSize(pt);
        QFontMetrics fm(f);
        const QRect br = fm.boundingRect(QRect(0, 0, w, h), Qt::AlignCenter | Qt::TextWordWrap, t);

        if(br.width() <= w && br.height() <= h) {
            chosen = pt;
            break;
        }
    }

    f.setPointSize(chosen);
    mLabel->setFont(f);
}

void GoodsGroupButton::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);
    scaleLabelFont();
}

void GoodsGroupButton::showEvent(QShowEvent *e)
{
    QFrame::showEvent(e);
    cacheBaseFontSize();
    scaleLabelFont();
}

void GoodsGroupButton::setColor(int c)
{
    QColor color = QColor::fromRgb(c);
    setStyleSheet(QString(
                      "QFrame#goodsGroupFrame {"
                      "background:%1;"
                      "}"
                      "QFrame#goodsGroupFrame:hover {"
                      "background:%2;"
                      "}"
                      "QFrame#goodsGroupFrame:pressed {"
                      "background:%3;"
                      "}"
                  )
                  .arg(color.name())
                  .arg(color.lighter(130).name())
                  .arg(color.darker(130).name())
                 );
}

void GoodsGroupButton::mousePressEvent(QMouseEvent *e)
{
    setProperty("pressed", true);
    style()->unpolish(this);
    style()->polish(this);
    QFrame::mousePressEvent(e);
}

void GoodsGroupButton::mouseReleaseEvent(QMouseEvent *e)
{
    setProperty("pressed", false);
    style()->unpolish(this);
    style()->polish(this);
    emit clicked();
    QFrame::mouseReleaseEvent(e);
}
