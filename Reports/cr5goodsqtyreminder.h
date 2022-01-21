#ifndef CR5GOODSQTYREMINDER_H
#define CR5GOODSQTYREMINDER_H

#include "c5reportwidget.h"

class CR5GoodsQtyReminder : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsQtyReminder(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;
};

#endif // CR5GOODSQTYREMINDER_H
