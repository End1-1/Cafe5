#include "c5daterange.h"
#include "ui_c5daterange.h"

C5DateRange::C5DateRange(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5DateRange)
{
    ui->setupUi(this);
}

C5DateRange::~C5DateRange()
{
    delete ui;
}

bool C5DateRange::dateRange(QDate &d1, QDate &d2)
{
    auto *dr = new C5DateRange(QStringList());
    bool result = (dr->exec() == QDialog::Accepted);
    d1 = dr->ui->deStart->date();
    d2 = dr->ui->deEnd->date();
    dr->deleteLater();
    return result;
}

bool C5DateRange::date(QDate &d)
{
    auto *a = new C5DateRange(QStringList());
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
