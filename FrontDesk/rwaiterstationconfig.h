#pragma once

#include "rabstractspecialwidget.h"

namespace Ui
{
class RWaiterStationConfig;
}

class RWaiterStationConfig : public RAbstractSpecialWidget
{
    Q_OBJECT
public:
    explicit RWaiterStationConfig(QWidget *parent = nullptr);
    ~RWaiterStationConfig();
private:
    Ui::RWaiterStationConfig* ui;
};
