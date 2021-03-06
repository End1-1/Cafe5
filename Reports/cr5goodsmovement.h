#ifndef CR5GOODSMOVEMENT_H
#define CR5GOODSMOVEMENT_H

#include "c5reportwidget.h"

class CR5GoodsMovement : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsMovement(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

protected:
    void restoreColumnsWidths();

    virtual bool tblDoubleClicked(int row, int column, const QList<QVariant> &values) override;

private slots:
    void changePrice();
};

#endif // CR5GOODSMOVEMENT_H
