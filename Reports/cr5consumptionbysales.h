#ifndef CR5CONSUMPTIONBYSALES_H
#define CR5CONSUMPTIONBYSALES_H

#include "c5reportwidget.h"

class CR5ConsumptionBySales : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5ConsumptionBySales(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();

    virtual void buildQuery();

    virtual void refreshData();

private:
    QMap<QString, int> fColumnNameIndex;

    QString documentForInventory();

private slots:
    void tblDoubleClicked(int row, int column, const QList<QVariant> &values);

    void makeOutput(bool v);
};

#endif // CR5CONSUMPTIONBYSALES_H
