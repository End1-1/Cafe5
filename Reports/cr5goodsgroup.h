#ifndef CR5GOODSGROUP_H
#define CR5GOODSGROUP_H

#include "c5reportwidget.h"

class CR5GoodsGroup : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsGroup(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

protected:
    void removeWithId(int id, int row) override;
};

#endif // CR5GOODSGROUP_H
