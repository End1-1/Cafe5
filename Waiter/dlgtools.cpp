#include "dlgtools.h"
#include "ui_dlgtools.h"
#include "c5waiterorderdoc.h"
#include "dlgpreorder.h"
#include "dlgorder.h"

DlgTools::DlgTools(C5WaiterOrderDoc *order, DlgOrder *dlg, const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgTools)
{
    ui->setupUi(this);
    fOrder = order;
    fDlg = dlg;
}

DlgTools::~DlgTools()
{
    delete ui;
}

void DlgTools::on_btnSetPreorder_clicked()
{
    accept();
    DlgPreorder p(fOrder, fDBParams);
    if (p.exec() == QDialog::Accepted) {
        fDlg->saveOrder();
    }
}
