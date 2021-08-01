#include "dlgshiftrotation.h"
#include "ui_dlgshiftrotation.h"
#include "datadriver.h"
#include "c5user.h"

DlgShiftRotation::DlgShiftRotation(C5User *user) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgShiftRotation),
    fUser(user)
{
    ui->setupUi(this);
    ui->de->setDate(QDate::fromString(__c5config.dateCash(), FORMAT_DATE_TO_STR_MYSQL));
    if (!ui->de->date().isValid()) {
        ui->de->setDate(QDate::currentDate());
    }
    for (int id: dbshift->list()) {
        ui->cb->addItem(dbshift->name(id), id);
    }
    ui->cb->setCurrentIndex(ui->cb->findData(__c5config.dateShift()));
}

DlgShiftRotation::~DlgShiftRotation()
{
    delete ui;
}

void DlgShiftRotation::on_btnNextDate_clicked()
{
    ui->de->setDate(ui->de->date().addDays(1));
}

void DlgShiftRotation::on_btnPrevDate_clicked()
{
    ui->de->setDate(ui->de->date().addDays(-1));
}

void DlgShiftRotation::on_btnChange_clicked()
{
    if (C5Message::question(tr("Change the shift?")) != QDialog::Accepted) {
        return;
    }
    C5Database db(__c5config.dbParams());
    db[":f_value"] = ui->de->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    db[":f_key"] = param_date_cash;
    db.exec("update s_settings_values set f_value=:f_value where f_key=:f_key");
    db[":f_value"] = ui->cb->currentData().toString();
    db[":f_key"] = param_date_cash_shift;
    db.exec("update s_settings_values set f_value=:f_value where f_key=:f_key");
    C5Message::info(tr("Session was changed"));
}

void DlgShiftRotation::on_btnCancel_clicked()
{
    reject();
}
