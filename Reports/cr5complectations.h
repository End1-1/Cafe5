#ifndef CR5COMPLECTATIONS_H
#define CR5COMPLECTATIONS_H

#include "c5reportwidget.h"

class CR5Complectations : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Complectations(const QStringList &dbparams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

private:
    virtual void buildQuery() override;
};

#endif // CR5COMPLECTATIONS_H
