#include "dlgaskforprecheck.h"
#include "ui_dlgaskforprecheck.h"

DlgAskForPrecheck::DlgAskForPrecheck() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgAskForPrecheck)
{
    ui->setupUi(this);
}

DlgAskForPrecheck::~DlgAskForPrecheck()
{
    delete ui;
}

int DlgAskForPrecheck::get()
{
    DlgAskForPrecheck d;
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
