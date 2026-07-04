#include "dishcardanimhost.h"

#include "dishcardwidget.h"

#include <QResizeEvent>

DishCardAnimHost::DishCardAnimHost(DishCardWidget *card, QWidget *parent)
    : QWidget(parent)
    , m_card(card)
{
    setFixedSize(card->size());
    setSizePolicy(card->sizePolicy());
    setAttribute(Qt::WA_StyledBackground, false);
    card->setParent(this);
    syncCardGeometry();
}

void DishCardAnimHost::setSlideY(qreal y)
{
    m_slideY = y;
    syncCardGeometry();
}

void DishCardAnimHost::syncCardGeometry()
{
    if (!m_card) {
        return;
    }

    m_card->setGeometry(0, qRound(m_slideY), width(), height());
}

void DishCardAnimHost::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    syncCardGeometry();
}
