#ifndef CR5MATERIALINSTOREUNCOMPLECT_H
#define CR5MATERIALINSTOREUNCOMPLECT_H

#include "c5reportwidget.h"

class CR5MaterialInStoreUncomplectFilter;

class CR5MaterialInStoreUncomplect : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5MaterialInStoreUncomplect(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

    virtual void buildQuery() override;

private:
    CR5MaterialInStoreUncomplectFilter *fFilter;
};

#endif // CR5MATERIALINSTOREUNCOMPLECT_H
