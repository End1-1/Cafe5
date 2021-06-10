#ifndef CR5CUSTOM_H
#define CR5CUSTOM_H

#include "c5reportwidget.h"

class CR5Custom : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Custom(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

protected slots:
    virtual void setSearchParameters() override;
};

#endif // CR5CUSTOM_H
