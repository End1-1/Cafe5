#ifndef CR5TABLES_H
#define CR5TABLES_H

#include "c5reportwidget.h"

class CR5Tables : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Tables(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

private slots:
    void createHall();
};

#endif // CR5TABLES_H
