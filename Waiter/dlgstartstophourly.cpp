#include "dlgstartstophourly.h"
#include "c5message.h"
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
