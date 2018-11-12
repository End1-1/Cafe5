#ifndef CR5GOODSMOVEMENTFILTER_H
#define CR5GOODSMOVEMENTFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5GoodsMovementFilter;
}

class CR5GoodsMovementFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5GoodsMovementFilter(const QStringList &dbParams, QWidget *parent = 0);

    ~CR5GoodsMovementFilter();

    virtual QString condition();

private:
    Ui::CR5GoodsMovementFilter *ui;
};

#endif // CR5GOODSMOVEMENTFILTER_H
