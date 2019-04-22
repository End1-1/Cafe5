#ifndef CR5GOODSWASTE_H
#define CR5GOODSWASTE_H

#include "c5reportwidget.h"

class CR5GoodsWaste : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsWaste(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

protected slots:
    virtual void removeRow();
};

#endif // CR5GOODSWASTE_H
