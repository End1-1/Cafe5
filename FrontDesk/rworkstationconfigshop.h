#pragma once

#include "rworkstationconfigwidget.h"

namespace Ui
{
class RWorkstationConfigShop;
}

class RWorkstationConfigShop : public RWorkstationConfigWidget
{
    Q_OBJECT
public:
    explicit RWorkstationConfigShop(QWidget *parent = nullptr);
    ~RWorkstationConfigShop() override;

protected:
    void applyConfig(const QJsonObject &config) override;

    QJsonObject collectConfig() const override;

private:
    Ui::RWorkstationConfigShop *ui = nullptr;

    QWidget *mForm = nullptr;
};
