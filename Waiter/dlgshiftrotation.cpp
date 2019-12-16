#include "dlgshiftrotation.h"
#include "ui_dlgshiftrotation.h"
#include "c5cafecommon.h"

DlgShiftRotation::DlgShiftRotation(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgShiftRotation)
{
    ui->setupUi(this);
    ui->de->setDate(QDate::fromString(__c5config.dateCash(), FORMAT_DATE_TO_STR_MYSQL));
    for (int i = 0; i < C5CafeCommon::fShifts.count(); i++) {
        QJsonObject o = C5CafeCommon::fShifts.at(i).toObject();
        ui->cb->addItem(o["f_name"].toString(), o["f_id"].toString().toInt());
    }
}

DlgShiftRotation::~DlgShiftRotation()
{
    delete ui;
}

void DlgShiftRotation::handleShift(const QJsonObject &obj)
{
    if (obj["reply"].toInt() == 1) {
        C5Message::info(tr("Shift was rotated"));
        accept();
    } else {
        C5Message::error(obj["msg"].toString());
    }
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
    C5SocketHandler *sh = createSocketHandler(SLOT(handleShift(QJsonObject)));
    sh->bind("cmd", sm_rotate_shift);
    sh->bind("date", ui->de->toMySQLDate(false));
    sh->bind("shift", ui->cb->currentData().toString());
    sh->send();
}

void DlgShiftRotation::on_btnCancel_clicked()
{
    reject();
}
