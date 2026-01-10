#pragma once

#include "waiterorderitemwidget.h"

class WaiterGuestWidget : public WaiterOrderItemWidget
{
    Q_OBJECT

public:
    WaiterGuestWidget(WaiterDish d, QWidget *parent = nullptr);
};
