#ifndef CR5GOODSMOVEMENT_H
#define CR5GOODSMOVEMENT_H

#include "c5reportwidget.h"

class CR5GoodsMovement : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsMovement(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();
};

#endif // CR5GOODSMOVEMENT_H
