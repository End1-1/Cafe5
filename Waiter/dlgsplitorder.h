#ifndef DLGSPLITORDER_H
#define DLGSPLITORDER_H

#include "c5dialog.h"

namespace Ui {
class DlgSplitOrder;
}

class C5WaiterOrderDoc;

class DlgSplitOrder : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSplitOrder(const QStringList &dbParams);

    ~DlgSplitOrder();

    void configOrder(C5WaiterOrderDoc *o);

private:
    Ui::DlgSplitOrder *ui;

    C5WaiterOrderDoc *fOrder;
};

#endif // DLGSPLITORDER_H
