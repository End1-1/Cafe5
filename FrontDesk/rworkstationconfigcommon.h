#pragma once

#include "rworkstationconfigwidget.h"

namespace Ui
{
class RWorkstationConfigCommon;
}

class RWorkstationConfigCommon : public RWorkstationConfigWidget
{
    Q_OBJECT
public:
    explicit RWorkstationConfigCommon(QWidget *parent = nullptr);
    ~RWorkstationConfigCommon() override;

protected:
    void applyConfig(const QJsonObject &config) override;

    QJsonObject collectConfig() const override;

private:
    Ui::RWorkstationConfigCommon *ui = nullptr;
};
