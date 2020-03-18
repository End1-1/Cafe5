#ifndef CR5CASHDETAILED_H
#define CR5CASHDETAILED_H

#include "c5reportwidget.h"

class CR5CashDetailedFilter;

class CR5CashDetailed : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5CashDetailed(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

public slots:
    virtual bool on_tblView_doubleClicked(const QModelIndex &index) override;

protected:
    virtual void buildQuery() override;

    virtual void refreshData() override;

    void restoreColumnsWidths() override;

protected slots:
    virtual int newRow() override;

private:
    CR5CashDetailedFilter *fFilter;

    QMap<QString, int> fColumnNameIndex;

    void sum();
};

#endif // CR5CASHDETAILED_H
