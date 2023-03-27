#ifndef CR5ROUTEDAILYFILTER_H
#define CR5ROUTEDAILYFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5RouteDailyFilter;
}

class CR5RouteDailyFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5RouteDailyFilter(const QStringList &dbParams, QWidget *parent = nullptr);
    ~CR5RouteDailyFilter();
    virtual QString condition() override;

private:
    Ui::CR5RouteDailyFilter *ui;
};

#endif // CR5ROUTEDAILYFILTER_H
