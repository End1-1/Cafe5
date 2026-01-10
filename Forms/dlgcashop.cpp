#include "dlgcashop.h"
#include "ui_dlgcashop.h"
#include "c5storedraftwriter.h"
#include "c5message.h"
#include "c5config.h"
#include "c5utils.h"
#include "c5user.h"
#include <QStyle>
#include <QDoubleValidator>

DlgCashOp::DlgCashOp(bool in, C5User *u) :
    C5Dialog(u),
    ui(new Ui::DlgCashOp)
{
    ui->setupUi(this);
    fUser = u;
    ui->lbInOut->setStyleSheet(in ? "color:green;" : "color:red;");
    ui->lbInOut->style()->polish(ui->lbInOut);
    fSign = in;
    ui->lbInOut->setText(in ? tr("In") : tr("Out"));
    ui->leAmount->setValidator(new QDoubleValidator(0, 999999999, 2));
}

DlgCashOp::~DlgCashOp()
{
    delete ui;
}

void DlgCashOp::on_btnCancel_clicked()
{
    reject();
}

void DlgCashOp::on_btnSave_clicked()
{
    if(ui->leAmount->getDouble() < 0.01) {
        C5Message::error(tr("Amount cannot be zero"));
        return;
    }

    if(ui->lePurpose->isEmpty()) {
        C5Message::error(tr("Purpose must be defined"));
        return;
    }

    C5Database db;
    C5StoreDraftWriter dw(db);
    int counter = dw.counterAType(DOC_TYPE_CASH);
    QString cashdoc;

    if(!dw.writeAHeader(cashdoc, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH,
                        fUser->id(), QDate::currentDate(), QDate::currentDate(), QTime::currentTime(),
                        0, ui->leAmount->getDouble(), ui->lePurpose->text(), 1, __c5config.getValue(param_default_currency).toInt())) {
        C5Message::error(dw.fErrorMsg);
        return;
    }

    if(!dw.writeAHeaderCash(cashdoc, fSign ? __c5config.cashId() : 0, fSign ? 0 : __c5config.cashId(),
                            0, "", "")) {
        C5Message::error(dw.fErrorMsg);
        return;
    }

    QString cashUUID;

    if(!dw.writeECash(cashUUID, cashdoc, __c5config.cashId(), fSign ? 1 : -1, ui->lePurpose->text(),
                      ui->leAmount->getDouble(), cashUUID, 1)) {
        C5Message::error(dw.fErrorMsg);
        return;
    }

    C5Message::info(QString("%1<br>%2<br>%3").arg(tr("Saved"), ui->lePurpose->text(), ui->leAmount->text()));
    accept();
}
