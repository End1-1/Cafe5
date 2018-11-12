#ifndef CR5TSTOREEXTRA_H
#define CR5TSTOREEXTRA_H

#include "c5reportwidget.h"

class CR5TStoreExtra : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5TStoreExtra(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();

protected:
    virtual void buildQuery();

    virtual void refreshData();

private:
    QMap<QString, int> fColumnNameIndex;
};

#endif // CR5TSTOREEXTRA_H
