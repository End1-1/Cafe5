#ifndef CR5SALESANDSTORE_H
#define CR5SALESANDSTORE_H

#include "c5reportwidget.h"

class CR5SaleAndStoreFilter;

class CR5SalesEffectiveness : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5SalesEffectiveness(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

    virtual void buildQuery();

    virtual void refreshData() override;

private:
    QHash<QString, int> fColumnNameIndex;

    CR5SaleAndStoreFilter *fFilter;

    void rep1();

    void rep2();
};

#endif // CR5SALESANDSTORE_H
