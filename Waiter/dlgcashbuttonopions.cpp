#include "dlgcashbuttonopions.h"
#include "ui_dlgcashbuttonopions.h"
#include "c5user.h"
#include <QSettings>

DlgCashButtonOpions::DlgCashButtonOpions(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgCashButtonOpions)
{
    ui->setupUi(this);

    QSettings _ls(qApp->applicationDirPath() + "/ls.inf", QSettings::IniFormat);
    if (_ls.value("cashoptions/view", 0).toInt() > 0) {
        ui->btnViewReport->setEnabled(_ls.value("cashoptions/view", -1).toInt() == __user->id());
    }
    if (_ls.value("cashoptions/print", 0).toInt() > 0) {
        ui->btnPrintReport->setEnabled(_ls.value("cashoptions/print", -1).toInt() == __user->id());
    }
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
    done(3);
}

void DlgCashButtonOpions::on_btnViewReport_clicked()
{
    done(1);
}
