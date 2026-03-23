#ifndef CR5TSTOREEXTRA_H
#define CR5TSTOREEXTRA_H

#include "c5reportwidget.h"

class CR5TStoreExtraFilter;

class CR5TStoreExtra : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5TStoreExtra(QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

protected:
    virtual void buildQuery() override;

    virtual void refreshData() override;

private:
    QHash<QString, int> fColumnNameIndex;

    CR5TStoreExtraFilter *fFilter;
};

#endif // CR5TSTOREEXTRA_H
