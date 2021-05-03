#ifndef CR5ORDERMARKS_H
#define CR5ORDERMARKS_H

#include "c5reportwidget.h"

class CR5OrderMarks : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5OrderMarks(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;
};

#endif // CR5ORDERMARKS_H
