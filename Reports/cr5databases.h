#ifndef CR5DATABASES_H
#define CR5DATABASES_H

#include "c5reportwidget.h"

class CR5Databases : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Databases(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();

private slots:
    void actionAccess();
};

#endif // CR5DATABASES_H
