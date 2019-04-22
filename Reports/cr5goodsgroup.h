#ifndef CR5GOODSGROUP_H
#define CR5GOODSGROUP_H

#include "c5reportwidget.h"

class CR5GoodsGroup : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsGroup(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();
};

#endif // CR5GOODSGROUP_H
