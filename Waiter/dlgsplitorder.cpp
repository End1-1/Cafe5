#include "dlgsplitorder.h"
#include "ui_dlgsplitorder.h"

DlgSplitOrder::DlgSplitOrder(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgSplitOrder)
{
    ui->setupUi(this);
    showMaximized();
    hide();
}

DlgSplitOrder::~DlgSplitOrder()
{
    delete ui;
}

void DlgSplitOrder::configOrder(C5WaiterOrderDoc *o)
{
    fOrder = o;
    ui->o1->config(o);
}

