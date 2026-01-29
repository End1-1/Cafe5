#pragma once

#include "rabstractspecialwidget.h"
#include "struct_waiter_order.h"

namespace Ui
{
class RWaiterOrder;
}

class RWaiterOrder : public RAbstractSpecialWidget
{
    Q_OBJECT
public:
    explicit RWaiterOrder(QWidget *parent = nullptr);

    ~RWaiterOrder();

    virtual void setup(const QJsonObject &jdoc) override;

private:
    Ui::RWaiterOrder* ui;

    WaiterOrder mOrder;
};
