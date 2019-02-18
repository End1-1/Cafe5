#ifndef CR5SALEFROMSTORE_H
#define CR5SALEFROMSTORE_H

#include "c5reportwidget.h"

class CR5SaleFromStoreFilter;

class CR5SaleFromStore : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5SaleFromStore(const QStringList &dbParams, QWidget *parent = 0);

    QToolBar *toolBar();

private:
    CR5SaleFromStoreFilter *fFilter;

private slots:
    void makeOutput(bool v);

    void tblClick(int row, int column, const QList<QVariant> &values);
};

#endif // CR5SALEFROMSTORE_H
