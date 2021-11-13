#include "dlgdate.h"
#include "ui_dlgdate.h"

DlgDate::DlgDate() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgDate)
{
    ui->setupUi(this);
}

DlgDate::~DlgDate()
{
    delete ui;
}

bool DlgDate::getDate(QDate &date)
{
    DlgDate d;
    if (d.exec() == QDialog::Accepted) {
        date = d.ui->leDate->date();
        return true;
    }
    return false;
}

void DlgDate::on_btnCancel_clicked()
{
    reject();
}

void DlgDate::on_btnOK_clicked()
{
    if (ui->leDate->date().isValid()) {
        accept();
    }
}
