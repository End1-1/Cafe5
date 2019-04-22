#ifndef CR5GOODS_H
#define CR5GOODS_H

#include "c5reportwidget.h"

class CR5Goods : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Goods(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();
};

#endif // CR5GOODS_H
