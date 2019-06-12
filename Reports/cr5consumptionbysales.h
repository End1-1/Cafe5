#ifndef CR5CONSUMPTIONBYSALES_H
#define CR5CONSUMPTIONBYSALES_H

#include "c5reportwidget.h"

class CR5ConsumptionBySalesFilter;

class CR5ConsumptionBySales : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5ConsumptionBySales(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

    virtual void buildQuery();

    virtual void refreshData();

private:
    CR5ConsumptionBySalesFilter *fFilter;

    QMap<QString, int> fColumnNameIndex;

    QString documentForInventory();

    void countRowQty(int row);

private slots:
    void tblDoubleClicked(int row, int column, const QList<QVariant> &values);

    void makeOutput(bool v);

    void salesOutput(bool v);

    void countOutputBasedOnRecipes();

    void writeDocs(int doctype, int reason, const QMap<int, double> &data, const QString &comment);
};

#endif // CR5CONSUMPTIONBYSALES_H
