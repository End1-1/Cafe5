#include "dlgnewcl.h"
#include "ui_dlgnewcl.h"
#include "doubledatabase.h"

DlgNewCL::DlgNewCL(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgNewCL)
{
    ui->setupUi(this);
}

DlgNewCL::~DlgNewCL()
{
    delete ui;
}

bool DlgNewCL::createCL(const QStringList &dbParams, QString &code, QString &name)
{
    DlgNewCL d(dbParams);
    if (d.exec() == QDialog::Accepted) {
        code = d.ui->leCode->text();
        name = d.ui->leName->text();
        return true;
    }
    return false;
}

void DlgNewCL::on_btnCancel_clicked()
{
    reject();
}

void DlgNewCL::on_btnOK_clicked()
{
    if (ui->leName->isEmpty()) {
        C5Message::error(tr("The name cannot be empty"));
        return;
    }
    DoubleDatabase fDD;
    fDD.open();
    fDD[":f_name"] = ui->leName->text();
    ui->leCode->setInteger(fDD.insert("f_city_ledger"));
    accept();
}
