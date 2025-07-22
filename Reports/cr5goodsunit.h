#ifndef CR5GOODSUNIT_H
#define CR5GOODSUNIT_H

#include "c5reportwidget.h"

class CR5GoodsUnit : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsUnit(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();
};

#endif // CR5GOODSUNIT_H
