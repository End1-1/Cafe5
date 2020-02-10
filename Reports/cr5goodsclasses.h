#ifndef CR5GOODSCLASSES_H
#define CR5GOODSCLASSES_H

#include "c5reportwidget.h"

class CR5GoodsClasses : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsClasses(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();
};

#endif // CR5GOODSCLASSES_H
