#include "goodsgroupbutton.h"
#include "../WaiterDesigner/waitergoodsgroupstyle.h"
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
    applyStyleFont();
}

void GoodsGroupButton::applyStyleFont()
{
    const WaiterGoodsGroupStyle &style = WaiterGoodsGroupStyle::cachedStyle();
    QFont f = mLabel->font();
    f.setPointSize(qBound(7, style.fontSize, 48));
    f.setBold(style.fontBold);
    mLabel->setFont(f);
    QPalette pal = mLabel->palette();
    pal.setColor(QPalette::WindowText, style.fontColor);
    pal.setColor(QPalette::Text, style.fontColor);
    mLabel->setPalette(pal);
    mLabel->setStyleSheet(QStringLiteral("QLabel#goodsGroupName { color: %1; background: transparent; }")
                              .arg(style.fontColor.name(QColor::HexRgb)));
    mBasePointSize = qBound(7, style.fontSize, 48);
}

void GoodsGroupButton::cacheBaseFontSize()
{
    if(mBasePointSize > 0) {
        return;
    }
    applyStyleFont();
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
    QFont baseFont = mLabel->font();
    baseFont.setBold(WaiterGoodsGroupStyle::cachedStyle().fontBold);
    QFont f = baseFont;
    int chosen = minPt;

    for(int pt = mBasePointSize; pt >= minPt; --pt) {
        f = baseFont;
        f.setPointSize(pt);
        f.setBold(baseFont.bold());
        QFontMetrics fm(f);
        const QRect br = fm.boundingRect(QRect(0, 0, w, h), Qt::AlignCenter | Qt::TextWordWrap, t);

        if(br.width() <= w && br.height() <= h) {
            chosen = pt;
            break;
        }
    }

    f.setPointSize(chosen);
    f.setBold(baseFont.bold());
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
    applyStyleFont();
    scaleLabelFont();
}

void GoodsGroupButton::setColor(int c)
{
    const WaiterGoodsGroupStyle &style = WaiterGoodsGroupStyle::cachedStyle();
    const QString border = style.borderWidth > 0
                               ? QStringLiteral("border:%1px solid %2;")
                                     .arg(style.borderWidth)
                                     .arg(QLatin1String(WaiterGoodsGroupStyle::borderColorCss))
                               : QStringLiteral("border:none;");
    QColor color = QColor::fromRgb(c);
    setStyleSheet(QStringLiteral(
                      "QFrame#goodsGroupFrame {"
                      "background:%1;"
                      "%4"
                      "}"
                      "QFrame#goodsGroupFrame:hover {"
                      "background:%2;"
                      "%4"
                      "}"
                      "QFrame#goodsGroupFrame[pressed=\"true\"] {"
                      "background:%3;"
                      "%4"
                      "}"
                  )
                  .arg(color.name(),
                       color.lighter(130).name(),
                       color.darker(130).name(),
                       border));
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
