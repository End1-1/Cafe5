#include "dlgnewcl.h"
#include "ui_dlgnewcl.h"
#include "c5message.h"
#include "c5database.h"
#include "c5config.h"

DlgNewCL::DlgNewCL(C5User *user) :
    C5Dialog(user),
    ui(new Ui::DlgNewCL)
{
    ui->setupUi(this);
}

DlgNewCL::~DlgNewCL()
{
    delete ui;
}

bool DlgNewCL::createCL(C5User *user, QString &code, QString &name)
{
    DlgNewCL d(user);

    if(d.exec() == QDialog::Accepted) {
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
    if(ui->leName->isEmpty()) {
        C5Message::error(tr("The name cannot be empty"));
        return;
    }

    C5Database db;
    db[":f_name"] = ui->leName->text();
    ui->leCode->setInteger(db.insert(QString("%1.f_city_ledger").arg(__c5config.hotelDatabase())));
    accept();
}
