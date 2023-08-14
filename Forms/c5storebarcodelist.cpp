#include "c5storebarcodelist.h"
#include "ui_c5storebarcodelist.h"
#include "c5mainwindow.h"

C5StoreBarcodeList::C5StoreBarcodeList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::C5StoreBarcodeList)
{
    ui->setupUi(this);
}

C5StoreBarcodeList::~C5StoreBarcodeList()
{
    delete ui;
}

bool C5StoreBarcodeList::getList(int &r1, int &r2)
{
    C5StoreBarcodeList *b = new C5StoreBarcodeList(__mainWindow);
    b->ui->sb2->setValue(r2);
    b->ui->sb2->setMaximum(r2);
    bool result = b->exec() == QDialog::Accepted;
    if (result) {
        r1 = b->ui->sb1->value();
        r2 = b->ui->sb2->value();
    }
    delete b;
    return result;
}

void C5StoreBarcodeList::on_btnReject_clicked()
{
    reject();
}

void C5StoreBarcodeList::on_btnOK_clicked()
{
    accept();
}
