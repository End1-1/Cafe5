#ifndef CR5GOODSPARTNERS_H
#define CR5GOODSPARTNERS_H

#include "c5reportwidget.h"

class CR5GoodsPartners : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsPartners(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();
};

#endif // CR5GOODSPARTNERS_H