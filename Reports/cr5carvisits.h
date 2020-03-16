#ifndef CR5CARVISITS_H
#define CR5CARVISITS_H

#include "c5reportwidget.h"

class CR5CarVisits : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5CarVisits(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

protected:
    virtual void restoreColumnsWidths();

private slots:
    void openOrder(int row, int column, const QList<QVariant> &values);
};

#endif // CR5CARVISITS_H
