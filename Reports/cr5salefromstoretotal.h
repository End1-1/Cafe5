#ifndef CR5SALEFROMSTORETOTAL_H
#define CR5SALEFROMSTORETOTAL_H

#include "c5reportwidget.h"

class CR5SaleFromStoreTotalFilter;

class CR5SaleFromStoreTotal : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5SaleFromStoreTotal(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

protected:
    virtual void buildQuery() override;

    virtual void refreshData() override;

private:
    QHash<QString, int> fColumnNameIndex;

    CR5SaleFromStoreTotalFilter *fFilter;
};

#endif // CR5SALEFROMSTORETOTAL_H
