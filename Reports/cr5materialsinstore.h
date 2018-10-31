#ifndef CR5MATERIALSINSTORE_H
#define CR5MATERIALSINSTORE_H

#include "c5reportwidget.h"

class CR5MaterialsInStore : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5MaterialsInStore(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();
};

#endif // CR5MATERIALSINSTORE_H
