#include "dlgaskforprecheck.h"
#include "ui_dlgaskforprecheck.h"

DlgAskForPrecheck::DlgAskForPrecheck(C5User *user) :
    C5Dialog(user),
    ui(new Ui::DlgAskForPrecheck)
{
    ui->setupUi(this);
}

DlgAskForPrecheck::~DlgAskForPrecheck()
{
    delete ui;
}

int DlgAskForPrecheck::get(C5User *user)
{
    DlgAskForPrecheck d(user);
    return d.exec();
}

void DlgAskForPrecheck::on_btnPrintPrecheck_clicked()
{
    reject();
}

void DlgAskForPrecheck::on_btnDoNotPrintPrecheck_clicked()
{
    accept();
}
