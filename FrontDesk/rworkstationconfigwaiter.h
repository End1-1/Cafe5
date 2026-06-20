#pragma once

#include "rworkstationconfigwidget.h"
#include <QCheckBox>
#include <QMap>

namespace Ui
{
class RWorkstationConfigWaiter;
}

class RWorkstationConfigWaiter : public RWorkstationConfigWidget
{
    Q_OBJECT
public:
    explicit RWorkstationConfigWaiter(QWidget *parent = nullptr);
    ~RWorkstationConfigWaiter() override;

protected:
    void applyConfig(const QJsonObject &config) override;

    QJsonObject collectConfig() const override;

private:
    void buildSetupButtons();

    Ui::RWorkstationConfigWaiter *ui = nullptr;

    QWidget *mForm = nullptr;

    QMap<QString, QCheckBox *> mSetupButtons;
};
