#ifndef CR5MFPRODUCT_H
#define CR5MFPRODUCT_H

#include "c5reportwidget.h"

class CR5MFProduct : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5MFProduct(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();
};

#endif // CR5MFPRODUCT_H
