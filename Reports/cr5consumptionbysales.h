#ifndef CR5CONSUMPTIONBYSALES_H
#define CR5CONSUMPTIONBYSALES_H

#include "c5storedraftwriter.h"
#include "c5reportwidget.h"
#include "ogoods.h"

class CR5ConsumptionBySalesFilter;
class C5ProgressDialog;
class C5StoreDoc;
class C5User;

class CR5ConsumptionBySales : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5ConsumptionBySales(QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

    virtual void buildQuery() override;

    virtual void refreshData() override;

protected:
    virtual bool tblDoubleClicked(int row, int column, const QJsonArray &values) override;

private:
    CR5ConsumptionBySalesFilter *fFilter;

    C5ProgressDialog *fPd;

    QList<QStringList> fIDs;

    QHash<QString, int> fColumnNameIndex;

    QString documentForInventory(int store);

    C5User *fUser;

    void countRowQty(int row);

    void writeInvQty(C5Database &db, double qty, int row, int column, int goods, bool checkSR = true);

private slots:
    void storeOutError(const QString &err);

    void storeoutResponse(const QJsonObject &jdoc);

    void makeOutput(bool v);

    void salesOutput(bool v);

    void rollbackGoodsOutput(bool v);

    void countOutputBasedOnRecipes();

    void changeOutputStore();

    void updateInventorizatinPrices();

    void semireadyInOut();

    C5StoreDoc *writeDocs(int doctype, int reason, const QList<OGoods> &data, const QString &comment);

signals:
    void updateProgressValue(int);
};

#endif // CR5CONSUMPTIONBYSALES_H
