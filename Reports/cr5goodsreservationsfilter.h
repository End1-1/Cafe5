#ifndef CR5GOODSRESERVATIONSFILTER_H
#define CR5GOODSRESERVATIONSFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5GoodsReservationsFilter;
}

class CR5GoodsReservationsFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5GoodsReservationsFilter(QWidget *parent = nullptr);

    ~CR5GoodsReservationsFilter();

    virtual QString condition() override;

private:
    Ui::CR5GoodsReservationsFilter *ui;
};

#endif // CR5GOODSRESERVATIONSFILTER_H
