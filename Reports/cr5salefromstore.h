#ifndef CR5SALEFROMSTORE_H
#define CR5SALEFROMSTORE_H

#include "c5reportwidget.h"

class CR5SaleFromStoreFilter;

class CR5SaleFromStore : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5SaleFromStore(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

    virtual void restoreColumnsWidths();

protected:
    virtual bool tblDoubleClicked(int row, int column, const QVector<QJsonValue> &values);

private:
    CR5SaleFromStoreFilter *fFilter;
};

#endif // CR5SALEFROMSTORE_H
