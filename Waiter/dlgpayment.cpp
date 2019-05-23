#include "dlgpayment.h"
#include "ui_dlgpayment.h"
#include "dlgpassword.h"
#include "c5utils.h"
#include "c5cafecommon.h"
#include "wpaymentoptions.h"
#include "dlgguest.h"
#include "dlgcl.h"
#include "c5user.h"
#include "c5logtoserverthread.h"
#include "dlgreceiptlanguage.h"
#include "dlgcreditcardlist.h"
#include "c5translator.h"
#include <QInputDialog>

#define paystart 4
#define p_cash 1
#define p_card 2
#define p_bank 3
#define p_other 4

DlgPayment::DlgPayment() :
    C5Dialog(C5Config::dbParams()),
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
    ui->tblInfo->setString(3, 0, "");
    ui->tblInfo->item(3, 0)->setData(Qt::TextAlignmentRole, Qt::AlignCenter);
    ui->tblInfo->setSpan(3, 0, 1, ui->tblInfo->columnCount());
    ui->wPayOther->setVisible(false);
    setLangIcon();
    setTaxState();
}

DlgPayment::~DlgPayment()
{
    delete ui;
}

int DlgPayment::payment(C5User *user, C5WaiterOrderDoc *order)
{
    DlgPayment *d = new DlgPayment();
    d->fUser = user;
    d->ui->btnPayTransferToRoom->setVisible(d->fUser->check(cp_t5_pay_transfer_to_room));
    d->ui->btnPayComplimentary->setVisible(d->fUser->check(cp_t5_pay_complimentary));
    d->ui->btnPayCityLedger->setVisible(d->fUser->check(cp_t5_pay_cityledger));
    d->ui->btnSelfCost->setVisible(d->fUser->check(cp_t5_pay_breakfast));
    d->fOrder = order;
    d->setRoomComment();
    d->setComplimentary();
    d->setSelfCost();
    d->ui->tblInfo->setString(1, 0, order->hString("f_tablename"));
    d->ui->tblInfo->setString(1, 1, order->hString("f_currentstaffname"));
    d->ui->tblInfo->setString(1, 2, order->hString("f_amounttotal"));
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
    if (fOrder->hInt("f_otherid") == PAYOTHER_SELFCOST) {
        ui->tblInfo->setDouble(ui->tblInfo->rowCount() - 1, 1, fOrder->hDouble("f_amountother"));
    }
    ui->btnCloseOrder->setEnabled(fOrder->hInt("f_print") > 0);
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
        fOrder->hSetString("f_bonustype", obj["card"].toObject()["f_type"].toString());
        fOrder->hSetString("f_bonusid", obj["card"].toObject()["f_id"].toString());
        switch (obj["card"].toObject()["f_type"].toString().toInt()) {
        case CARD_TYPE_DISCOUNT:
            fOrder->hSetDouble("f_discountfactor", obj["card"].toObject()["f_value"].toString().toDouble() / 100.0);
            for (int i = 0; i < fOrder->fItems.count(); i++) {
                fOrder->iSetString("f_discount", fOrder->hString("f_discountfactor"), i);
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
                    fOrder->hSetDouble("f_amountcash", 0);
                    break;
                case p_card:
                    fOrder->hSetDouble("f_amountcard", 0);
                    break;
                case p_bank:
                    fOrder->hSetDouble("f_amountbank", 0);
                    break;
                case p_other:
                    fOrder->hSetDouble("f_amountother", 0);
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
    double max = fOrder->hDouble("f_amounttotal");
    if (!DlgPassword::getAmount(title, max)) {
        return;
    }
    if (ui->tblInfo->item(row - 1, 0)->data(Qt::UserRole).toInt() > 0) {
        double o = ui->tblInfo->getDouble(row - 1, 1);
        if (o + max > fOrder->hDouble("f_amounttotal")) {
            o = fOrder->hDouble("f_amounttotal") - max;
            if (o < 0) {
                o = 0;
            }
        }
        if (o + max > fOrder->hDouble("f_amounttotal")) {
            max = fOrder->hDouble("f_amounttotal");
        }
        ui->tblInfo->setDouble(row - 1, 1, o);
        switch (ui->tblInfo->item(row, 0)->data(Qt::UserRole).toInt()) {
        case p_cash:
            fOrder->hSetDouble("f_amountcash", o);
            break;
        case p_card:
            fOrder->hSetDouble("f_amountcard", o);
            break;
        case p_bank:
            fOrder->hSetDouble("f_amountbank", o);
            break;
        case p_other:
            fOrder->hSetDouble("f_amountother", o);
            break;
        }
    } else {
        if (row < ui->tblInfo->rowCount() - 1) {
            double o = ui->tblInfo->getDouble(row + 1, 1);
            if (o + max > fOrder->hDouble("f_amounttotal")) {
                o = fOrder->hDouble("f_amounttotal") - max;
                if (o < 0) {
                    o = 0;
                }
            }
            if (o + max > fOrder->hDouble("f_amounttotal")) {
                max = fOrder->hDouble("f_amounttotal");
            }
            ui->tblInfo->setDouble(row + 1, 1, o);
            switch (ui->tblInfo->item(row, 0)->data(Qt::UserRole).toInt()) {
            case p_cash:
                fOrder->hSetDouble("f_amountcash", o);
                break;
            case p_card:
                fOrder->hSetDouble("f_amountcard", o);
                break;
            case p_bank:
                fOrder->hSetDouble("f_amountbank", o);
                break;
            case p_other:
                fOrder->hSetDouble("f_amountother", o);
                break;
            }
        }
    }
    ui->tblInfo->setDouble(row, 1, max);
    switch (ui->tblInfo->item(row, 0)->data(Qt::UserRole).toInt()) {
    case p_cash:
        fOrder->hSetDouble("f_amountcash", max);
        break;
    case p_card:
        fOrder->hSetDouble("f_amountcard", max);
        break;
    case p_bank:
        fOrder->hSetDouble("f_amountbank", max);
        break;
    case p_other:
        fOrder->hSetDouble("f_amountother", max);
        break;
    }
}

void DlgPayment::on_btnReceipt_clicked()
{
    if (ui->tblInfo->rowCount() == paystart) {
        C5Message::error(tr("Payment mode is not selected"));
        return;
    }
    if (fOrder->hDouble("f_amountother") > 0.001) {
        if (fOrder->hInt("f_otherid") == 0) {
            C5Message::error(tr("Other method is not selected"));
            return;
        }
    }
    C5SocketHandler *sh = createSocketHandler(SLOT(handleReceipt(QJsonObject)));
    sh->bind("cmd", sm_printreceipt);
    sh->bind("station", hostinfo);
    sh->bind("printer", C5Config::localReceiptPrinter());
    QJsonObject o;
    fOrder->hSetInt("f_receiptlanguage", C5Config::getRegValue("receipt_language").toInt());
    fOrder->hSetString("f_printtax", ui->btnTax->isChecked() ? "1" : "0");
    fOrder->hSetString("receipt_printer",  C5Config::fSettingsName);
    o["header"] = fOrder->fHeader;
    o["body"] = fOrder->fItems;
    sh->send(o);

    //LOG
    QString payMethods;
    QString amounts;
    if (fOrder->hDouble("f_amountcash") > 0.001) {
        payMethods += payMethods.isEmpty() ? "Cash" : payMethods + " / Cash";
        amounts += amounts.isEmpty() ? fOrder->hString("f_amountcash") : amounts + " / " + fOrder->hString("f_amountcash");
    }
    if (fOrder->hDouble("f_amountcard") > 0.001) {
        payMethods += payMethods.isEmpty() ? "Cash" : payMethods + " / Card";
        amounts += amounts.isEmpty() ? fOrder->hString("f_amountcard") : amounts + " / " + fOrder->hString("f_amountcard");
    }
    if (fOrder->hDouble("f_amountbank") > 0.001) {
        payMethods = "Bank";
        amounts = fOrder->hString("f_amountbank");
    }
    if (fOrder->hDouble("f_amountother") > 0.001) {
        switch (fOrder->hInt("f_otherid")) {
        case PAYOTHER_TRANSFER_TO_ROOM:
            payMethods = fOrder->hString("f_other_room") + "," + fOrder->hString("f_other_inv") + "," + fOrder->hString("f_other_guest");
            break;
        case PAYOTHER_CL:
            payMethods = fOrder->hString("f_other_clcode") + "," + fOrder->hString("f_other_clname");
            break;
        case PAYOTHER_COMPLIMENTARY:
            payMethods = "Complimentary";
            break;
        case PAYOTHER_SELFCOST:
            payMethods = "Selfcost";
            break;
        default:
            payMethods = QString("%1 (%2)").arg("Unknown").arg(fOrder->hString("f_otherid"));
            break;
        }
        amounts = fOrder->hString("f_amountother");
    }
    amounts += fOrder->hString("f_printtax") == "1" ? " Tax: yes" : " Tax: no";
    C5LogToServerThread::remember(LOG_WAITER, __username, "", fOrder->hString("f_id"), "", "Receipt", payMethods, amounts);
    //END LOG
}

void DlgPayment::on_btnCancel_clicked()
{
    done(PAYDLG_NONE);
}

void DlgPayment::checkTotal()
{
    ui->btnCloseOrder->setEnabled(fOrder->hInt("f_print") > 0);
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

void DlgPayment::setLangIcon()
{
    switch (C5Config::getRegValue("receipt_language").toInt()) {
    case 0:
        ui->btnReceiptLanguage->setIcon(QIcon(":/armenia.png"));
        C5Config::setRegValue("receipt_language", LANG_AM);
        break;
    case 1:
        ui->btnReceiptLanguage->setIcon(QIcon(":/usa.png"));
        C5Config::setRegValue("receipt_language", LANG_EN);
        break;
    case 2:
        ui->btnReceiptLanguage->setIcon(QIcon(":/russia.png"));
        C5Config::setRegValue("receipt_language", LANG_RU);
        break;
    }
}

void DlgPayment::setRoomComment()
{
    bool v = !fOrder->hString("f_other_room").isEmpty();
    ui->leRoomComment->setVisible(v);
    if (v) {
        ui->leRoomComment->setText(fOrder->hString("f_other_room") + ", " + fOrder->hString("f_other_guest"));
    }
}

void DlgPayment::setCLComment()
{
    bool v = !fOrder->hString("f_other_clcode").isEmpty();
    ui->leRoomComment->setVisible(v);
    if (v) {
        ui->leRoomComment->setText(fOrder->hString("f_other_clcode") + ", " + fOrder->hString("f_other_clname"));
    }
}

void DlgPayment::setComplimentary()
{
    if (fOrder->hInt("f_otherid") == PAYOTHER_COMPLIMENTARY) {
        ui->leRoomComment->setVisible(true);
        ui->leRoomComment->setText(tr("Complimentary"));
    }
}

void DlgPayment::setTaxState()
{
    switch (C5Config::getRegValue("btn_tax_state").toInt()) {
    case 0:
    case 1:
        ui->btnTax->setChecked(true);
        break;
    case 2:
        ui->btnTax->setChecked(false);
        break;
    }
}

void DlgPayment::clearOther()
{
    fOrder->hSetString("f_otherid", "0");
    fOrder->hSetString("f_other_res", "");
    fOrder->hSetString("f_other_inv", "");
    fOrder->hSetString("f_other_room", "");
    fOrder->hSetString("f_other_guest", "");
    fOrder->hSetString("f_other_clcode", "");
    fOrder->hSetString("f_other_clname", "");
    ui->leRoomComment->clear();
    ui->leRoomComment->setVisible(false);
}

void DlgPayment::setSelfCost()
{
    if (fOrder->hInt("f_otherid") == PAYOTHER_SELFCOST) {
        ui->leRoomComment->setVisible(true);
        ui->leRoomComment->setText(tr("Selfcost"));
    }
}

void DlgPayment::setPaymentInfo()
{
    for (int i = ui->tblInfo->rowCount() - 1; i > paystart - 1; i--) {
        ui->tblInfo->removeRow(i);
    }
    double cash = fOrder->hDouble("f_amountcash");
    if (cash > 0.001) {
        addPaymentMode(p_cash, tr("Cash"), cash);
    }
    double card = fOrder->hDouble("f_amountcard");
    if (card > 0.001) {
        addPaymentMode(p_card, tr("Card"), card);
    }
    double bank = fOrder->hDouble("f_amountbank");
    if (bank > 0.001) {
        addPaymentMode(p_bank, tr("Bank"), bank);
    }
    double other = fOrder->hDouble("f_amountother");
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
        text = text + " - " + C5CafeCommon::creditCardName(fOrder->hInt("f_creditcardid"));
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
        fOrder->hSetDouble("f_amountcash", amount);
        break;
    case p_card:
        fOrder->hSetDouble("f_amountcard", amount);
        break;
    case p_bank:
        fOrder->hSetDouble("f_amountbank", amount);
        break;
    case p_other:
        fOrder->hSetDouble("f_amountother", amount);
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
    C5LogToServerThread::remember(LOG_WAITER, __username, "", fOrder->hString("f_id"), "", "Close order", "", "");
}

void DlgPayment::on_btnPaymentCash_clicked()
{
    clearOther();
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
    fOrder->hSetDouble("f_amountbank", 0);
    fOrder->hSetDouble("f_amountother", 0);
    total = fOrder->hDouble("f_amounttotal") - total;
    addPaymentMode(p_cash, tr("Cash"), total);
    ui->btnTax->setChecked(true);
}

void DlgPayment::on_btnPaymentCard_clicked()
{
    clearOther();
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
    fOrder->hSetDouble("f_creditcardid", cardid);
    fOrder->hSetDouble("f_amountbank", 0);
    fOrder->hSetDouble("f_amountother", 0);
    total = fOrder->hDouble("f_amounttotal") - total;
    addPaymentMode(p_card, tr("Credit card"), total);
    ui->btnTax->setChecked(true);
}

void DlgPayment::on_btnPaymentBank_clicked()
{
    clearOther();
    double total = 0.0;
    if (checkForPaymentMethod(p_bank, total)) {
        C5Message::error(tr("Bankh method already exists"));
        return;
    }
    total = fOrder->hDouble("f_amounttotal");
    for (int i = ui->tblInfo->rowCount() - 1; i >= paystart; i--) {
        ui->tblInfo->removeRow(i);
    }
    addPaymentMode(p_bank, tr("Bank transfer"), total);
    fOrder->hSetDouble("f_amountcash", 0);
    fOrder->hSetDouble("f_amountcard", 0);
    fOrder->hSetDouble("f_amountother", 0);
    fOrder->hSetDouble("f_amountbank", total);
    ui->btnTax->setChecked(false);
}

void DlgPayment::on_btnPaymentOther_clicked()
{
    clearOther();
    double total = 0.0;
    if (checkForPaymentMethod(p_other, total)) {
        C5Message::error(tr("Other method already exists"));
        return;
    }
    total = fOrder->hDouble("f_amounttotal");
    for (int i = ui->tblInfo->rowCount() - 1; i >= paystart; i--) {
        ui->tblInfo->removeRow(i);
    }
    addPaymentMode(p_other, tr("Other transfer"), total);
    fOrder->hSetDouble("f_amountcash", 0);
    fOrder->hSetDouble("f_amountcard", 0);
    fOrder->hSetDouble("f_amountbank", 0);
    fOrder->hSetDouble("f_amountother", total);
    ui->wPayOther->setVisible(true);
    ui->btnTax->setChecked(false);
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
    sh->bind("order", fOrder->hString("f_id"));
    sh->bind("code", code);
    sh->send();
}

void DlgPayment::on_btnPayComplimentary_clicked()
{
    clearOther();
    fOrder->hSetString("f_otherid", QString::number(PAYOTHER_COMPLIMENTARY));
    setComplimentary();
}

void DlgPayment::on_btnPayTransferToRoom_clicked()
{
    QString res, inv, room, guest;
    if (DlgGuest::getGuest(res, inv, room, guest)) {
        fOrder->hSetString("f_otherid", QString::number(PAYOTHER_TRANSFER_TO_ROOM));
        fOrder->hSetString("f_other_res", res);
        fOrder->hSetString("f_other_inv", inv);
        fOrder->hSetString("f_other_room", room);
        fOrder->hSetString("f_other_guest", guest);
        setRoomComment();
    }
}

void DlgPayment::on_btnReceiptLanguage_clicked()
{
    int r = DlgReceiptLanguage::receipLanguage();
    if (r > -1) {
        C5Config::setRegValue("receipt_language", r);
        setLangIcon();
    }
}

void DlgPayment::on_btnTax_clicked()
{
    C5Config::setRegValue("btn_tax_state", ui->btnTax->isChecked() ? 2 : 1);
}

void DlgPayment::on_btnPayCityLedger_clicked()
{
    QString clCode, clName;
    if (DlgCL::getCL(clCode, clName)) {
        fOrder->hSetString("f_otherid", QString::number(PAYOTHER_CL));
        fOrder->hSetString("f_other_clcode", clCode);
        fOrder->hSetString("f_other_clname", clName);
        setCLComment();
    }
}

void DlgPayment::on_btnSelfCost_clicked()
{
    clearOther();
    fOrder->hSetString("f_otherid", QString::number(PAYOTHER_SELFCOST));
    setSelfCost();
}

void DlgPayment::on_btnBill_clicked()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handleReceipt(QJsonObject)));
    sh->bind("cmd", sm_bill);
    sh->bind("station", hostinfo);
    sh->bind("printer", C5Config::localReceiptPrinter());
    QJsonObject o;
    fOrder->hSetInt("f_receiptlanguage", C5Config::getRegValue("receipt_language").toInt());
    fOrder->hSetString("f_printtax", ui->btnTax->isChecked() ? "1" : "0");
    o["header"] = fOrder->fHeader;
    o["body"] = fOrder->fItems;
    sh->send(o);
}
