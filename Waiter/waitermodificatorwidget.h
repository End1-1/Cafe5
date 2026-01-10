#pragma once

#include "waiterorderitemwidget.h"

class WaiterModificatorWidget: public WaiterOrderItemWidget
{
    Q_OBJECT

public:
    WaiterModificatorWidget(WaiterDish d, QWidget *parent = nullptr);
};
