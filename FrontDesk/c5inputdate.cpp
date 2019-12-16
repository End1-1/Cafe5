#include "c5inputdate.h"
#include "ui_c5inputdate.h"
#include "c5cache.h"

C5InputDate::C5InputDate(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5InputDate)
{
    ui->setupUi(this);
    ui->leShift->setSelector(dbParams, ui->leShiftName, cache_salary_shift);
}

C5InputDate::~C5InputDate()
{
    delete ui;
}

bool C5InputDate::date(const QStringList &dbParams, QDate &d, int &shift, QString &shiftName)
{
    auto *id = new C5InputDate(dbParams);
    id->ui->deDate->setDate(d);
    bool result = id->exec() == QDialog::Accepted;
    if (result) {
        d = id->ui->deDate->date();
        shift = id->ui->leShift->getInteger();
        shiftName = id->ui->leShiftName->text();
    }
    return result;
}

void C5InputDate::on_btnCancel_clicked()
{
    reject();
}

void C5InputDate::on_btnOK_clicked()
{
    if (ui->leShift->getInteger() == 0) {
        C5Message::error(tr("Shift was not selected"));
        return;
    }
    if (ui->deDate->date().isValid()) {
        accept();
    }
}
