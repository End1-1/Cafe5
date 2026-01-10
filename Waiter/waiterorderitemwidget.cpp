#include "waiterorderitemwidget.h"
#include <QStyle>

WaiterOrderItemWidget::WaiterOrderItemWidget(const WaiterDish &wd, QWidget *parent) :
    QWidget(parent),
    mOrderItem(wd)
{
}

void WaiterOrderItemWidget::setFocused(bool value)
{
    fFocus = value;
    update();
}

bool WaiterOrderItemWidget::isFocused()
{
    return fFocus;
}

void WaiterOrderItemWidget::updateDish(WaiterDish value)
{
    mOrderItem = value;
}

void WaiterOrderItemWidget::checkFocus(const QString &id)
{
    if(fFocus && id != mOrderItem.id) {
        fFocus = false;
    }

    if(!fFocus && id != mOrderItem.id) {
    }

    fFocus = id == mOrderItem.id;
    container()->setProperty("dish_focused", fFocus ? "1" : "2");
    container()->style()->unpolish(container());
    container()->style()->polish(container());
}
