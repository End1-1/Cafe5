#ifndef CR5GOODSMOVEMENT_H
#define CR5GOODSMOVEMENT_H

#include "c5reportwidget.h"

class CR5GoodsMovementFilter;

class CR5GoodsMovement : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsMovement(QWidget *parent = nullptr);

    virtual QToolBar* toolBar() override;

    void setDate(const QDate &d1, const QDate &d2);

    void setDocType(const QString &docType);

    void setStore(const QString &store);

    void setGoods(const QString &goods);

    void setReason(const QString &reason);

    void setInOut(int inout);

protected:
    void restoreColumnsWidths() override;

    virtual bool tblDoubleClicked(int row, int column, const QJsonArray &values) override;

private:
    CR5GoodsMovementFilter* fFilter;

private slots:
    void changePrice();

    void importFromAS();

    void templates();
};

#endif // CR5GOODSMOVEMENT_H
