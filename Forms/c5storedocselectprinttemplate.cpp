#include "c5storedocselectprinttemplate.h"
#include "ui_c5storedocselectprinttemplate.h"

C5StoreDocSelectPrintTemplate::C5StoreDocSelectPrintTemplate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::C5StoreDocSelectPrintTemplate)
{
    ui->setupUi(this);
}

C5StoreDocSelectPrintTemplate::~C5StoreDocSelectPrintTemplate()
{
    delete ui;
}

void C5StoreDocSelectPrintTemplate::on_btnCancel_clicked()
{
    reject();
}

void C5StoreDocSelectPrintTemplate::on_btnPrintOneDoc_clicked()
{
    done(1);
}

void C5StoreDocSelectPrintTemplate::on_btnPrintTwoDoc_clicked()
{
    done(2);
}
