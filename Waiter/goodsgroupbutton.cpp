#include "goodsgroupbutton.h"
#include <QLabel>
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
