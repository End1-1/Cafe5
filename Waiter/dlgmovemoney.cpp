#include "dlgmovemoney.h"
#include "dlgqty.h"
#include "dlgtext.h"
#include "ui_dlgmovemoney.h"
#include "c5message.h"

DlgMoveMoney::DlgMoveMoney(C5User *user)
    : C5Dialog(user),
      ui(new Ui::DlgMoveMoney)
{
    ui->setupUi(this);
}

DlgMoveMoney::~DlgMoveMoney()
{
    delete ui;
}

void DlgMoveMoney::setMode(bool isCashIn)
{
    ui->lbTitle->setText(isCashIn ? tr("Cash in") : tr("Cash out"));
}

double DlgMoveMoney::amount() const
{
    return ui->leAmount->getDouble();
}

QString DlgMoveMoney::comment() const
{
    return ui->leComment->text();
}

void DlgMoveMoney::on_btnAmountKbd_clicked()
{
    double v = ui->leAmount->getDouble();

    if(!DlgQty::getQty(v, tr("Amount"), mUser)) {
        return;
    }

    ui->leAmount->setDouble(v);
}

void DlgMoveMoney::on_btnCommentKbd_clicked()
{
    QString txt = ui->leComment->text();

    if(!DlgText::getText(mUser, tr("Comment"), txt)) {
        return;
    }

    ui->leComment->setText(txt);
}

void DlgMoveMoney::on_btnSave_clicked()
{
    if(amount() < 0.001) {
        C5Message::error(tr("Amount is not valid"));
        return;
    }

    accept();
}

void DlgMoveMoney::on_btnCancel_clicked()
{
    reject();
}
