#include "dlgpayment.h"
#include "ui_dlgpayment.h"
#include "dlgpassword.h"
#include "c5utils.h"
#include "c5cafecommon.h"
#include "wpaymentoptions.h"
#include "dlgcreditcardlist.h"
#include <QInputDialog>

#define paystart 4
#define p_cash 1
#define p_card 2
#define p_bank 3
#define p_other 4

DlgPayment::DlgPayment(QWidget *parent) :
    C5Dialog(C5Config::dbParams(), parent),
    ui(new Ui::DlgPayment)
{
    ui->setupUi(this);
    qApp->processEvents();
    ui->tblInfo->setRowCount(paystart);
    ui->tblInfo->setColumnCount(3);
    ui->tblInfo->setString(0, 0, tr("Table"));
    ui->tblInfo->setString(0, 1, tr("Waiter"));
    ui->tblInfo->setString(0, 2, tr("Amount to paid"));
    ui->tblInfo->setSpan(2, 0, 1, ui->tblInfo->columnCount());
    ui->tblInfo->setString(3, 0, tr("Payment"));
    ui->tblInfo->item(3, 0)->setData(Qt::TextAlignmentRole, Qt::AlignCenter);
    ui->tblInfo->setSpan(3, 0, 1, ui->tblInfo->columnCount());
}

DlgPayment::~DlgPayment()
{
    delete ui;
}

int DlgPayment::payment(C5Order *order)
{
    DlgPayment *d = new DlgPayment(C5Config::fParentWidget);
    d->fOrder = order;
    d->ui->tblInfo->setString(1, 0, order->headerValue("f_tablename"));
    d->ui->tblInfo->setString(1, 1, order->headerValue("f_currentstaffname"));
    d->ui->tblInfo->setString(1, 2, order->headerValue("f_amounttotal"));
    d->setPaymentInfo();
    d->checkTotal();
    d->showFullScreen();
    d->ui->tblInfo->fitColumnsToWidth();
    d->hide();
    int result = d->exec();
    delete d;
    return result;
}

void DlgPayment::handleReceipt(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    fOrder->fHeader = obj["header"].toObject();
    fOrder->fItems = obj["body"].toArray();
    ui->btnCloseOrder->setEnabled(fOrder->headerValue("f_print").toInt() > 0);
}

void DlgPayment::handleCloseOrder(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    done(PAYDLG_ORDER_CLOSE);
}

void DlgPayment::handleDiscount(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    switch (obj["type"].toInt()) {
    case CARD_TYPE_DISCOUNT:
        fOrder->setHeaderValue("f_bonustype", obj["card"].toObject()["f_type"].toString());
        fOrder->setHeaderValue("f_bonusid", obj["card"].toObject()["f_id"].toString());
        switch (obj["card"].toObject()["f_type"].toString().toInt()) {
        case CARD_TYPE_DISCOUNT:
            fOrder->setHeaderValue("f_discountfactor", obj["card"].toObject()["f_value"].toString().toDouble() / 100.0);
            for (int i = 0; i < fOrder->fItems.count(); i++) {
                fOrder->setItemValue(i, "f_discount", fOrder->headerValue("f_discountfactor"));
            }
            break;
        }
        fOrder->countTotal();
        break;
    }
    setPaymentInfo();
}

void DlgPayment::handleError(int err, const QString &msg)
{
    Q_UNUSED(err);
    C5Message::error(msg);
}

void DlgPayment::tableWidgetBtnClearClicked()
{
    for (int i = paystart; i < ui->tblInfo->rowCount(); i++) {
        WPaymentOptions *w = dynamic_cast<WPaymentOptions*>(ui->tblInfo->cellWidget(i, 2));
        if (w) {
            if (w == sender()) {
                switch (ui->tblInfo->item(i, 0)->data(Qt::UserRole).toInt()) {
                case p_cash:
                    fOrder->setHeaderValue("f_amountcash", 0);
                    break;
                case p_card:
                    fOrder->setHeaderValue("f_amountcard", 0);
                    break;
                case p_bank:
                    fOrder->setHeaderValue("f_amountbank", 0);
                    break;
                case p_other:
                    fOrder->setHeaderValue("f_amountother", 0);
                    break;
                }
                ui->tblInfo->removeRow(i);
                return;
            }
        }
    }
}

void DlgPayment::tableWidgetBtnCalcClicked()
{
    int row = 0;
    for (int i = ui->tblInfo->rowCount() - 1; i >= paystart; i--) {
        WPaymentOptions *w = dynamic_cast<WPaymentOptions*>(ui->tblInfo->cellWidget(i, 2));
        if (w) {
            if (w == sender()) {
                row = i;
                break;
            }
        }
    }
    if (row == 0) {
        return;
    }
    QString title;
    switch (ui->tblInfo->item(row, 0)->data(Qt::UserRole).toInt()) {
    case p_cash:
        title = tr("Cash");
        break;
    case p_card:
        title = tr("Card");
        break;
    case p_bank:
        title = tr("Bank");
        break;
    case p_other:
        title = tr("Other");
        break;
    }
    double max = fOrder->headerValue("f_amounttotal").toDouble();
    if (!DlgPassword::getAmount(title, max)) {
        return;
    }
    if (ui->tblInfo->item(row - 1, 0)->data(Qt::UserRole).toInt() > 0) {
        double o = ui->tblInfo->getDouble(row - 1, 1);
        if (o + max > fOrder->headerValue("f_amounttotal").toDouble()) {
            o = fOrder->headerValue("f_amounttotal").toDouble() - max;
            if (o < 0) {
                o = 0;
            }
        }
        if (o + max > fOrder->headerValue("f_amounttotal").toDouble()) {
            max = fOrder->headerValue("f_amounttotal").toDouble();
        }
        ui->tblInfo->setDouble(row - 1, 1, o);
        switch (ui->tblInfo->item(row, 0)->data(Qt::UserRole).toInt()) {
        case p_cash:
            fOrder->setHeaderValue("f_amountcash", o);
            break;
        case p_card:
            fOrder->setHeaderValue("f_amountcard", o);
            break;
        case p_bank:
            fOrder->setHeaderValue("f_amountbank", o);
            break;
        case p_other:
            fOrder->setHeaderValue("f_amountother", o);
            break;
        }
    } else {
        if (row < ui->tblInfo->rowCount() - 1) {
            double o = ui->tblInfo->getDouble(row + 1, 1);
            if (o + max > fOrder->headerValue("f_amounttotal").toDouble()) {
                o = fOrder->headerValue("f_amounttotal").toDouble() - max;
                if (o < 0) {
                    o = 0;
                }
            }
            if (o + max > fOrder->headerValue("f_amounttotal").toDouble()) {
                max = fOrder->headerValue("f_amounttotal").toDouble();
            }
            ui->tblInfo->setDouble(row + 1, 1, o);
            switch (ui->tblInfo->item(row, 0)->data(Qt::UserRole).toInt()) {
            case p_cash:
                fOrder->setHeaderValue("f_amountcash", o);
                break;
            case p_card:
                fOrder->setHeaderValue("f_amountcard", o);
                break;
            case p_bank:
                fOrder->setHeaderValue("f_amountbank", o);
                break;
            case p_other:
                fOrder->setHeaderValue("f_amountother", o);
                break;
            }
        }
    }
    ui->tblInfo->setDouble(row, 1, max);
    switch (ui->tblInfo->item(row, 0)->data(Qt::UserRole).toInt()) {
    case p_cash:
        fOrder->setHeaderValue("f_amountcash", max);
        break;
    case p_card:
        fOrder->setHeaderValue("f_amountcard", max);
        break;
    case p_bank:
        fOrder->setHeaderValue("f_amountbank", max);
        break;
    case p_other:
        fOrder->setHeaderValue("f_amountother", max);
        break;
    }
}

void DlgPayment::on_btnReceipt_clicked()
{
    if (ui->tblInfo->rowCount() == paystart) {
        C5Message::error(tr("Payment mode is not selected"));
        return;
    }
    C5SocketHandler *sh = createSocketHandler(SLOT(handleReceipt(QJsonObject)));
    sh->bind("cmd", sm_printreceipt);
    sh->bind("station", hostinfo);
    sh->bind("printer", C5Config::localReceiptPrinter());
    QJsonObject o;
    o["header"] = fOrder->fHeader;
    o["body"] = fOrder->fItems;
    sh->send(o);
}

void DlgPayment::on_btnCancel_clicked()
{
    done(PAYDLG_NONE);
}

void DlgPayment::checkTotal()
{
    ui->btnCloseOrder->setEnabled(fOrder->headerValue("f_print").toInt() > 0);
}

bool DlgPayment::checkForPaymentMethod(int mode, double &total)
{
    total = 0.0;
    bool result = false;
    for (int i = paystart; i < ui->tblInfo->rowCount(); i++) {
        if (ui->tblInfo->item(i, 0)->data(Qt::UserRole).toInt() == mode) {
            result = true;
        }
        total += ui->tblInfo->getDouble(i, 1);
    }
    return result;
}

void DlgPayment::setPaymentInfo()
{
    for (int i = ui->tblInfo->rowCount() - 1; i > paystart - 1; i--) {
        ui->tblInfo->removeRow(i);
    }
    double cash = fOrder->headerValue("f_amountcash").toDouble();
    if (cash > 0.001) {
        addPaymentMode(p_cash, tr("Cash"), cash);
    }
    double card = fOrder->headerValue("f_amountcard").toDouble();
    if (card > 0.001) {
        addPaymentMode(p_card, tr("Card"), card);
    }
    double bank = fOrder->headerValue("f_amountbank").toDouble();
    if (bank > 0.001) {
        addPaymentMode(p_bank, tr("Bank"), bank);
    }
    double other = fOrder->headerValue("f_amountother").toDouble();
    if (other > 0.001) {
        addPaymentMode(p_other, tr("Other"), other);
    }
}

void DlgPayment::addPaymentMode(int mode, QString text, double amount)
{
    int row = ui->tblInfo->rowCount();
    ui->tblInfo->setRowCount(ui->tblInfo->rowCount() + 1);
    switch (mode) {
    case p_card: {
        text = text + " - " + C5CafeCommon::creditCardName(fOrder->headerValue("f_creditcardid").toInt());
        break;
    }
    }
    ui->tblInfo->setString(row, 0, text);
    ui->tblInfo->item(row, 0)->setData(Qt::UserRole, mode);
    ui->tblInfo->setDouble(row, 1, amount);
    WPaymentOptions *w = new WPaymentOptions();
    ui->tblInfo->setCellWidget(row, 2, w);
    ui->tblInfo->setRowHeight(row, 50);
    connect(w, SIGNAL(clearClicked()), this, SLOT(tableWidgetBtnClearClicked()));
    connect(w, SIGNAL(calc()), this, SLOT(tableWidgetBtnCalcClicked()));
    switch (ui->tblInfo->item(row, 0)->data(Qt::UserRole).toInt()) {
    case p_cash:
        fOrder->setHeaderValue("f_amountcash", amount);
        break;
    case p_card:
        fOrder->setHeaderValue("f_amountcard", amount);
        break;
    case p_bank:
        fOrder->setHeaderValue("f_amountbank", amount);
        break;
    case p_other:
        fOrder->setHeaderValue("f_amountother", amount);
        break;
    }
}

void DlgPayment::on_btnCloseOrder_clicked()
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

void DlgPayment::on_btnPaymentCash_clicked()
{
    double total = 0.0;
    for (int i = ui->tblInfo->rowCount() - 1; i >= paystart; i--) {
        if (ui->tblInfo->item(i, 0)->data(Qt::UserRole).toInt() == p_bank
                || ui->tblInfo->item(i, 0)->data(Qt::UserRole).toInt() == p_other) {
            ui->tblInfo->removeRow(i);
        }
    }
    if (checkForPaymentMethod(p_cash, total)) {
        C5Message::error(tr("Cash method already exists"));
        return;
    }
    fOrder->setHeaderValue("f_amountbank", 0);
    fOrder->setHeaderValue("f_amountother", 0);
    total = fOrder->headerValue("f_amounttotal").toDouble() - total;
    addPaymentMode(p_cash, tr("Cash"), total);
}

void DlgPayment::on_btnPaymentCard_clicked()
{
    int cardid;
    QString cardname;
    if (!DlgCreditCardList::getCardInfo(cardid, cardname)) {
        return;
    }
    double total = 0.0;
    for (int i = ui->tblInfo->rowCount() - 1; i >= paystart; i--) {
        if (ui->tblInfo->item(i, 0)->data(Qt::UserRole).toInt() == p_bank
                || ui->tblInfo->item(i, 0)->data(Qt::UserRole).toInt() == p_other) {
            ui->tblInfo->removeRow(i);
        }
    }
    if (checkForPaymentMethod(p_card, total)) {
        C5Message::error(tr("Card method already exists"));
        return;
    }
    fOrder->setHeaderValue("f_creditcardid", cardid);
    fOrder->setHeaderValue("f_amountbank", 0);
    fOrder->setHeaderValue("f_amountother", 0);
    total = fOrder->headerValue("f_amounttotal").toDouble() - total;
    addPaymentMode(p_card, tr("Credit card"), total);
}

void DlgPayment::on_btnPaymentBank_clicked()
{
    double total = 0.0;
    if (checkForPaymentMethod(p_bank, total)) {
        C5Message::error(tr("Bankh method already exists"));
        return;
    }
    total = fOrder->headerValue("f_amounttotal").toDouble();
    for (int i = ui->tblInfo->rowCount() - 1; i >= paystart; i--) {
        ui->tblInfo->removeRow(i);
    }
    addPaymentMode(p_bank, tr("Bank transfer"), total);
    fOrder->setHeaderValue("f_amountcash", 0);
    fOrder->setHeaderValue("f_amountcard", 0);
    fOrder->setHeaderValue("f_amountother", 0);
    fOrder->setHeaderValue("f_amountbank", total);
}

void DlgPayment::on_btnPaymentOther_clicked()
{
    double total = 0.0;
    if (checkForPaymentMethod(p_other, total)) {
        C5Message::error(tr("Other method already exists"));
        return;
    }
    total = fOrder->headerValue("f_amounttotal").toDouble();
    for (int i = ui->tblInfo->rowCount() - 1; i >= paystart; i--) {
        ui->tblInfo->removeRow(i);
    }
    addPaymentMode(p_other, tr("Other transfer"), total);
    fOrder->setHeaderValue("f_amountcash", 0);
    fOrder->setHeaderValue("f_amountcard", 0);
    fOrder->setHeaderValue("f_amountbank", 0);
    fOrder->setHeaderValue("f_amountother", total);
}

void DlgPayment::on_btnDiscount_clicked()
{
    bool ok;
    QString code = QInputDialog::getText(this, tr("Card"), tr("Card code"), QLineEdit::Password, "", &ok);
    if (!ok) {
        return;
    }
    if (code.isEmpty()) {
        C5Message::error(tr("Card code is empty"));
        return;
    }
    C5SocketHandler *sh = createSocketHandler(SLOT(handleDiscount(QJsonObject)));
    sh->bind("cmd", sm_discount);
    sh->bind("order", fOrder->headerValue("f_id"));
    sh->bind("code", code);
    sh->send();
}
