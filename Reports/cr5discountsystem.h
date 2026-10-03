#ifndef CR5DISCOUNTSYSTEM_H
#define CR5DISCOUNTSYSTEM_H

#include "c5reportwidget.h"

class CR5DiscountSystem : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5DiscountSystem(QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

public slots:
    virtual bool on_tblView_doubleClicked(const QModelIndex &index) override;

protected slots:
    virtual void completeRefresh() override;

    virtual int newRow() override;
};

#endif // CR5DISCOUNTSYSTEM_H
