#ifndef CR5GOODSRESERVATIONS_H
#define CR5GOODSRESERVATIONS_H

#include "c5reportwidget.h"

class CR5GoodsReservations : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5GoodsReservations(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual void buildQuery() override;

    virtual QToolBar *toolBar() override;

protected:
    virtual int newRow() override;

    virtual bool tblDoubleClicked(int row, int column, const QJsonArray &values) override;
};

#endif // CR5GOODSRESERVATIONS_H
