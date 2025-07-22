#ifndef CR5MATERIALMOVEUNCOMPLECT_H
#define CR5MATERIALMOVEUNCOMPLECT_H

#include "c5reportwidget.h"

class CR5MaterialmoveUncomplectFilter;

class CR5MaterialMoveUncomplect : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5MaterialMoveUncomplect(QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

    virtual void buildQuery() override;

private:
    CR5MaterialmoveUncomplectFilter *fFilter;
};

#endif // CR5MATERIALMOVEUNCOMPLECT_H
