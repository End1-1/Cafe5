#include "dlgcashbuttonopions.h"
#include "ui_dlgcashbuttonopions.h"

DlgCashButtonOpions::DlgCashButtonOpions(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgCashButtonOpions)
{
    ui->setupUi(this);
}

DlgCashButtonOpions::~DlgCashButtonOpions()
{
    delete ui;
}

int DlgCashButtonOpions::getOptions()
{
    DlgCashButtonOpions d(C5Config::dbParams());
    return d.exec();
}

void DlgCashButtonOpions::on_btnCancel_clicked()
{
    done(0);
}

void DlgCashButtonOpions::on_btnPrintReport_clicked()
{
    done(2);
}

void DlgCashButtonOpions::on_btnCloseCash_clicked()
{
    if (C5Message::question(tr("Confirm cash closing")) != QDialog::Accepted) {
        done(0);
        return;
    }
    done(1);
}
