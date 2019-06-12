#ifndef CR5STOREREASON_H
#define CR5STOREREASON_H

#include "c5reportwidget.h"

class CR5StoreReason : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5StoreReason(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;
};

#endif // CR5STOREREASON_H
