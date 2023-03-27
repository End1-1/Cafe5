#ifndef CR5CONSUMPTIONBYSALESDRAFT_H
#define CR5CONSUMPTIONBYSALESDRAFT_H

#include "c5storedraftwriter.h"
#include "c5reportwidget.h"
#include "ogoods.h"

class CR5ConsumptionBySalesFilterDraft;
class C5StoreDoc;

class CR5ConsumptionBySalesDraft : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5ConsumptionBySalesDraft(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

    virtual void buildQuery();

    virtual void refreshData();

protected:
    virtual bool tblDoubleClicked(int row, int column, const QList<QVariant> &values);

private:
    CR5ConsumptionBySalesFilterDraft *fFilter;

    QHash<QString, int> fColumnNameIndex;

    QString documentForInventory();

    void countRowQty(int row);

    void setPriceVisible(int mode);

private slots:
    void makeOutput(bool v);

    void salesOutput(bool v);

    void rollbackGoodsOutput(bool v);

    void countOutputBasedOnRecipes();

    void changeOutputStore();

    C5StoreDoc *writeDocs(int doctype, int reason, const QList<OGoods> &data, const QString &comment);

signals:
    void updateProgressValue(int);
};

#endif // CR5CONSUMPTIONBYSALES_H
