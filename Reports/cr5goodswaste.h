#ifndef CR5GOODSWASTE_H
#define CR5GOODSWASTE_H

#include "c5reportwidget.h"

class CR5GoodsWaste : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsWaste(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();

private slots:
    void dblClick(int row, int column, const QList<QVariant> &values);
};

#endif // CR5GOODSWASTE_H
