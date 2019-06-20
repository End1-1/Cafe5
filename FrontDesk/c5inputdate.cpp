#include "c5inputdate.h"
#include "ui_c5inputdate.h"

C5InputDate::C5InputDate(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5InputDate)
{
    ui->setupUi(this);
}

C5InputDate::~C5InputDate()
{
    delete ui;
}

bool C5InputDate::date(QDate &d)
{
    auto *id = new C5InputDate(QStringList());
    id->ui->deDate->setDate(d);
    bool result = id->exec() == QDialog::Accepted;
    if (result) {
        d = id->ui->deDate->date();
    }
    return result;
}

void C5InputDate::on_btnCancel_clicked()
{
    reject();
}

void C5InputDate::on_btnOK_clicked()
{
    if (ui->deDate->date().isValid()) {
        accept();
    }
}
