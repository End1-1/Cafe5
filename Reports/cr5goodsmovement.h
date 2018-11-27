#ifndef CR5GOODSMOVEMENT_H
#define CR5GOODSMOVEMENT_H

#include "c5reportwidget.h"

class CR5GoodsMovement : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsMovement(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();

protected slots:
    virtual void tblDoubleClicked(int row, int column, const QList<QVariant> &values);
};

#endif // CR5GOODSMOVEMENT_H
