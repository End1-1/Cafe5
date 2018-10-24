#include "dlgpayment.h"
#include "ui_dlgpayment.h"
#include "dlgpassword.h"

DlgPayment::DlgPayment(QWidget *parent) :
    C5Dialog(C5Config::dbParams(), parent),
    ui(new Ui::DlgPayment)
{
    ui->setupUi(this);
}

DlgPayment::~DlgPayment()
{
    delete ui;
}

int DlgPayment::payment(C5Order *order)
{
    DlgPayment *d = new DlgPayment(C5Config::fParentWidget);
    d->fOrder = order;
    d->ui->leTable->setText(order->headerValue("f_tablename"));
    d->ui->leWaiter->setText(order->headerValue("f_currentstaffname"));
    d->ui->leAmountTotal->setText(order->headerValue("f_amounttotal"));
    d->ui->leCash->setText(order->headerValue("f_amountcash"));
    d->ui->leCard->setText(order->headerValue("f_amountcard"));
    d->ui->leBank->setText(order->headerValue("f_amountbank"));
    d->ui->leOther->setText(order->headerValue("f_amountother"));
    if (d->ui->leCard->getDouble()
            + d->ui->leCash->getDouble()
            + d->ui->leBank->getDouble()
            + d->ui->leOther->getDouble() < 0.01) {
        d->ui->leCash->setText(order->headerValue("f_amounttotal"));
        order->setHeaderValue("f_amountcash", order->headerValue("f_amounttotal"));
    }
    d->checkTotal();
    d->showFullScreen();
    d->hide();
    int result = d->exec();
    delete d;
    return result;
}

void DlgPayment::handleReceipt(const QJsonObject &obj)
{
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    fOrder->fHeader = obj["header"].toObject();
    fOrder->fItems = obj["body"].toArray();
    ui->btnCash->setEnabled(fOrder->headerValue("f_print").toInt() > 0);
}

void DlgPayment::handleCloseOrder(const QJsonObject &obj)
{
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    done(PAYDLG_ORDER_CLOSE);
}

void DlgPayment::handleError(int err, const QString &msg)
{
    Q_UNUSED(err);
    C5Message::error(msg);
}

void DlgPayment::on_btnReceipt_clicked()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handleReceipt(QJsonObject)));
    sh->bind("cmd", sm_printreceipt);
    sh->bind("station", hostinfo);
    sh->bind("printer", C5Config::localReceiptPrinter());
    QJsonObject o;
    fOrder->setHeaderValue("f_amountcash", ui->leCash->text());
    fOrder->setHeaderValue("f_amountcard", ui->leCard->text());
    fOrder->setHeaderValue("f_amountbank", ui->leBank->text());
    fOrder->setHeaderValue("f_amountother", ui->leOther->text());
    o["header"] = fOrder->fHeader;
    o["body"] = fOrder->fItems;
    sh->send(o);
}

void DlgPayment::on_btnCash_clicked()
{
    if (C5Message::question(tr("Confirm to close order")) != QDialog::Accepted) {
        return;
    }
    C5SocketHandler *sh = createSocketHandler(SLOT(handleCloseOrder(QJsonObject)));
    sh->bind("cmd", sm_closeorder);
    QJsonObject o;
    o["header"] = fOrder->fHeader;
    o["body"] = fOrder->fItems;
    sh->send(o);
}

void DlgPayment::on_btnCancel_clicked()
{
    done(PAYDLG_NONE);
}

void DlgPayment::on_btnClearCash_clicked()
{
    ui->leCash->setInteger(0);
    checkTotal();
}

void DlgPayment::on_btnClearCard_clicked()
{
    ui->leCard->setInteger(0);
    checkTotal();
}

void DlgPayment::on_btnClearBank_clicked()
{
    ui->leBank->setInteger(0);
    checkTotal();
}

void DlgPayment::on_btnClearOther_clicked()
{
    ui->leOther->setInteger(0);
    checkTotal();
}

void DlgPayment::on_btnSetCash_clicked()
{
    double v = ui->leAmountTotal->getInteger();
    if (!DlgPassword::getAmount(tr("CASH"), v)) {
        return;
    }
    ui->leCash->setInteger(v);
    checkTotal();
}

void DlgPayment::on_btnSetCard_clicked()
{
    double v = ui->leAmountTotal->getInteger();
    if (!DlgPassword::getAmount(tr("CARD"), v)) {
        return;
    }
    ui->leCard->setInteger(v);
    checkTotal();
}

void DlgPayment::checkTotal()
{
    if (ui->leCash->getDouble() + ui->leCard->getDouble() + ui->leBank->getDouble() + ui->leOther->getDouble() == ui->leAmountTotal->getDouble()) {
        ui->btnReceipt->setEnabled(true);
    } else {
        ui->btnReceipt->setEnabled(false);
    }
    ui->btnCash->setEnabled(fOrder->headerValue("f_print").toInt() > 0);
}

void DlgPayment::on_btnSetBank_clicked()
{
    double v = ui->leAmountTotal->getInteger();
    if (!DlgPassword::getAmount(tr("BANK"), v)) {
        return;
    }
    ui->leBank->setInteger(v);
    checkTotal();
}

void DlgPayment::on_btnSetOther_clicked()
{
    double v = ui->leAmountTotal->getInteger();
    if (!DlgPassword::getAmount(tr("OTHER"), v)) {
        return;
    }
    ui->leOther->setInteger(v);
    checkTotal();
}
