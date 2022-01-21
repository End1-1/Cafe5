#ifndef CR5GOODSQTYREMINDERFILTER_H
#define CR5GOODSQTYREMINDERFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5GoodsQtyReminderFilter;
}

class CR5GoodsQtyReminderFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5GoodsQtyReminderFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5GoodsQtyReminderFilter();

    virtual QString condition() override;

private:
    Ui::CR5GoodsQtyReminderFilter *ui;
};

#endif // CR5GOODSQTYREMINDERFILTER_H
