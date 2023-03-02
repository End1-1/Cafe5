#ifndef CR5CONSUMPTIONBYSALES_H
#define CR5CONSUMPTIONBYSALES_H

#include "c5storedraftwriter.h"
#include "c5reportwidget.h"

class CR5ConsumptionBySalesFilter;
class C5StoreDoc;
class C5User;

class CR5ConsumptionBySales : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5ConsumptionBySales(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

    virtual void buildQuery();

    virtual void refreshData();

protected:
    virtual bool tblDoubleClicked(int row, int column, const QList<QVariant> &values);

private:
    CR5ConsumptionBySalesFilter *fFilter;

    QHash<QString, int> fColumnNameIndex;

    QString documentForInventory(int store);

    C5User *fUser;

    void countRowQty(int row);

private slots:
    void makeOutput(bool v);

    void salesOutput(bool v);

    void rollbackGoodsOutput(bool v);

    void countOutputBasedOnRecipes();

    void changeOutputStore();

    void updateInventorizatinPrices();

    void semireadyInOut();

    C5StoreDoc *writeDocs(int doctype, int reason, const QList<IGoods> &data, const QString &comment);

signals:
    void updateProgressValue(int);
};

#endif // CR5CONSUMPTIONBYSALES_H
