#include "c5daterange.h"
#include "ui_c5daterange.h"

C5DateRange::C5DateRange(C5User *user) :
    C5Dialog(user),
    ui(new Ui::C5DateRange)
{
    ui->setupUi(this);
}

C5DateRange::~C5DateRange()
{
    delete ui;
}

bool C5DateRange::dateRange(QDate &d1, QDate &d2, C5User *user)
{
    auto *dr = new C5DateRange(user);
    bool result = (dr->exec() == QDialog::Accepted);
    d1 = dr->ui->deStart->date();
    d2 = dr->ui->deEnd->date();
    dr->deleteLater();
    return result;
}

bool C5DateRange::date(QDate &d, C5User *user)
{
    auto *a = new C5DateRange(user);
    a->ui->deEnd->setVisible(false);
    a->ui->label_2->setVisible(false);
    a->adjustSize();
    bool result = a->exec() == QDialog::Accepted;
    d = a->ui->deStart->date();
    a->deleteLater();
    return result;
}

void C5DateRange::on_btnCancel_clicked()
{
    reject();
}

void C5DateRange::on_btnOk_clicked()
{
    accept();
}
