#ifndef CR5CASHMOVEMENTFILTER_H
#define CR5CASHMOVEMENTFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5CashMovementFilter;
}

class CR5CashMovementFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5CashMovementFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5CashMovementFilter();

    virtual QString condition() override;

    virtual QString filterText() override;

private:
    Ui::CR5CashMovementFilter *ui;
};

#endif // CR5CASHMOVEMENTFILTER_H
