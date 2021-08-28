#ifndef CR5TSTOREEXTRA_H
#define CR5TSTOREEXTRA_H

#include "c5reportwidget.h"

class CR5TStoreExtraFilter;

class CR5TStoreExtra : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5TStoreExtra(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

protected:
    virtual void buildQuery();

    virtual void refreshData();

    virtual bool tblDoubleClicked(int row, int column, const QList<QVariant> &values);

private:
    QHash<QString, int> fColumnNameIndex;

    QString documentForInventory();

    CR5TStoreExtraFilter *fFilter;
};

#endif // CR5TSTOREEXTRA_H
