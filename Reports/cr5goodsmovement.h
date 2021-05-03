#ifndef CR5GOODSMOVEMENT_H
#define CR5GOODSMOVEMENT_H

#include "c5reportwidget.h"

class CR5GoodsMovementFilter;

class CR5GoodsMovement : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsMovement(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

    void setDate(const QDate &d1, const QDate &d2);

    void setDocType(const QString &docType);

    void setStore(const QString &store);

    void setGoods(const QString &goods);

    void setReason(const QString &reason);

    void setInOut(int inout);

protected:
    void restoreColumnsWidths();

    virtual bool tblDoubleClicked(int row, int column, const QList<QVariant> &values) override;

private:
    CR5GoodsMovementFilter *fGoodsFilter;

private slots:
    void changePrice();
};

#endif // CR5GOODSMOVEMENT_H
