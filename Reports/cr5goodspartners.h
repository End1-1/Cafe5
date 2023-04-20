#ifndef CR5GOODSPARTNERS_H
#define CR5GOODSPARTNERS_H

#include "c5reportwidget.h"

class CR5GoodsPartners : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsPartners(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

public slots:
    void armSoftMap();
};

#endif // CR5GOODSPARTNERS_H
