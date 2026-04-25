#include "dlgstartstophourly.h"
#include "c5message.h"
#include "dlgdatetimepicker.h"
#include "ui_dlgstartstophourly.h"

DlgStartStopHourly::DlgStartStopHourly(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgStartStopHourly)
{
    ui->setupUi(this);
}

DlgStartStopHourly::~DlgStartStopHourly()
{
    delete ui;
}

void DlgStartStopHourly::on_btnStop_clicked()
{
    if (C5Message::question(tr("Do you want to stop counter?")) != QDialog::Accepted) {
        return;
    }
    accept();
}

void DlgStartStopHourly::on_btnCancel_clicked()
{
    reject();
}

void DlgStartStopHourly::on_btnSetEnd_clicked()
{
    DlgDateTimePicker d(this);
    d.setDateTime(mDateTime);
    if (d.exec() == QDialog::Accepted) {
        mDateTime = d.dateTime();
        mOption = 1;
        accept();
    }
}
