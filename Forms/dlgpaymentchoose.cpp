#include "dlgpaymentchoose.h"
#include "ui_dlgpaymentchoose.h"
#include "c5message.h"
#include "c5config.h"
#include "c5utils.h"
#include <QDoubleValidator>
#include <QShortcut>
#include <QStyle>

DlgPaymentChoose::DlgPaymentChoose(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgPaymentChoose)
{
    ui->setupUi(this);
    ui->leCard->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->leCash->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->leIdram->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->leTelcell->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->leBankTransfer->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->leCredit->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->leDebt->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->leCashIn->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->leChange->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->lePrepaid->setValidator(new QDoubleValidator(0, 1000000000, 2));
    fMaxPrepaid = 0;
#ifdef QT_DEBUG
    QShortcut *s = new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(checkFiscal()));
#else
    QShortcut *s = new QShortcut(QKeySequence(Qt::Key_F12), this, SLOT(checkFiscal()));
#endif
    Q_UNUSED(s);
    adjustSize();
}

DlgPaymentChoose::~DlgPaymentChoose()
{
    delete ui;
}

bool DlgPaymentChoose::getValues(double total, double &cash, double &card, double &idram, double &telcell, double &bank,
                                 double &credit,
                                 double &prepaid, double &debt, double &cashin, double &change, bool &fiscal,
                                 bool readOnlyPrepaid, double maxPrepaid)
{
    DlgPaymentChoose d(__c5config.dbParams());
    d.ui->leTotal->setDouble(total);
    d.ui->leCash->setDouble(cash);
    d.ui->leCard->setDouble(card);
    d.ui->leBankTransfer->setDouble(bank);
    d.ui->leCredit->setDouble(credit);
    d.ui->leIdram->setDouble(idram);
    d.ui->leTelcell->setDouble(telcell);
    d.ui->lePrepaid->setDouble(prepaid);
    d.ui->leDebt->setDouble(debt);
    d.ui->leCashIn->setDouble(cashin);
    d.ui->leChange->setDouble(change);
    d.fFiscal = fiscal;
    d.setFiscalStyle();
    d.fFiscal = fiscal;
    d.fMaxPrepaid = maxPrepaid;
    if (maxPrepaid < 1 && __c5config.fMainJson["prepaid_only_by_gift"].toBool()) {
        d.ui->lePrepaid->setEnabled(false);
        d.ui->btnPrepaid->setEnabled(false);
    }
    if (d.exec() == QDialog::Accepted) {
        cash = d.ui->leCash->getDouble();
        card = d.ui->leCard->getDouble();
        bank = d.ui->leBankTransfer->getDouble();
        credit = d.ui->leCredit->getDouble();
        idram = d.ui->leIdram->getDouble();
        telcell = d.ui->leTelcell->getDouble();
        prepaid = d.ui->lePrepaid->getDouble();
        debt = d.ui->leDebt->getDouble();
        cashin = d.ui->leCashIn->getDouble();
        change = d.ui->leChange->getDouble();
        fiscal = d.fFiscal;
        return true;
    }
    d.ui->lePrepaid->setReadOnly(readOnlyPrepaid);
    d.ui->btnPrepaid->setEnabled(!readOnlyPrepaid);
    return false;
}

void DlgPaymentChoose::keyEnter()
{
    on_btnPay_clicked();
}

void DlgPaymentChoose::setFiscalStyle()
{
    if (fFiscal) {
        setStyleSheet("");
    } else {
        setStyleSheet("background:qlineargradient(spread:reflect, x1:0.449, y1:0, x2:0.511, y2:1, stop:0 rgba(210, 255, 197, 255), stop:1 rgba(238, 238, 238, 255))");
    }
    style()->polish(this);
}

void DlgPaymentChoose::checkFiscal()
{
    fFiscal = !fFiscal;
    if (ui->leCard->getDouble() > 0.01) {
        fFiscal = true;
    }
    setFiscalStyle();
}

void DlgPaymentChoose::on_btnBack_clicked()
{
    reject();
}

void DlgPaymentChoose::on_btnCash_clicked()
{
    clearAll(ui->leCash);
}

void DlgPaymentChoose::clearAll(QLineEdit *le)
{
    ui->leCash->clear();
    ui->leCard->clear();
    ui->leBankTransfer->clear();
    ui->leIdram->clear();
    ui->leTelcell->clear();
    ui->lePrepaid->clear();
    ui->leDebt->clear();
    ui->leCredit->clear();
    le->setText(ui->leTotal->text());
    if (ui->lePrepaid == le) {
        if (ui->lePrepaid->getDouble() > fMaxPrepaid && fMaxPrepaid > 1) {
            ui->lePrepaid->setDouble(fMaxPrepaid);
        }
    }
}

void DlgPaymentChoose::countChange()
{
    double diff = ui->leCashIn->getDouble()
                  - ui->leCash->getDouble();
    if (diff > ui->leCashIn->getDouble()) {
        diff = ui->leCashIn->getDouble();
    }
    ui->leChange->setDouble(diff > 0.001 ? diff : 0);
}

void DlgPaymentChoose::on_btnCard_clicked()
{
    fFiscal = true;
    setFiscalStyle();
    clearAll(ui->leCard);
}

void DlgPaymentChoose::on_btnIdram_clicked()
{
    fFiscal = true;
    setFiscalStyle();
    clearAll(ui->leIdram);
}

void DlgPaymentChoose::on_btnTelcell_clicked()
{
    fFiscal = true;
    setFiscalStyle();
    clearAll(ui->leTelcell);
}

void DlgPaymentChoose::on_btnPrepaid_clicked()
{
    fFiscal = true;
    setFiscalStyle();
    clearAll(ui->lePrepaid);
}

void DlgPaymentChoose::on_btnDebt_clicked()
{
    fFiscal = false;
    setFiscalStyle();
    clearAll(ui->leDebt);
}

void DlgPaymentChoose::on_leCashIn_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    countChange();
}

void DlgPaymentChoose::on_btnBankTransfer_clicked()
{
    fFiscal = false;
    setFiscalStyle();
    clearAll(ui->leBankTransfer);
}

void DlgPaymentChoose::on_btnPay_clicked()
{
    double value =
        ui->leCash->getDouble()
        + ui->leCard->getDouble()
        + ui->leIdram->getDouble()
        + ui->leTelcell->getDouble()
        + ui->leBankTransfer->getDouble()
        + ui->lePrepaid->getDouble()
        + ui->leDebt->getDouble()
        + ui->leCredit->getDouble();
    if (value > ui->leTotal->getDouble() || value < ui->leTotal->getDouble()) {
        C5Message::error(tr("Check amounts"));
        return;
    }
    accept();
}

void DlgPaymentChoose::on_leCash_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    countChange();
}

void DlgPaymentChoose::on_leCard_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    countChange();
    if (str_float(arg1) > 0.001) {
        fFiscal = true;
        setFiscalStyle();
    }
}

void DlgPaymentChoose::on_leBankTransfer_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if (str_float(arg1) > 0.001) {
        fFiscal = false;
        setFiscalStyle();
    }
    countChange();
}

void DlgPaymentChoose::on_leIdram_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    countChange();
    if (str_float(arg1) > 0.001) {
        fFiscal = true;
        setFiscalStyle();
    }
}

void DlgPaymentChoose::on_leTelcell_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    countChange();
    if (str_float(arg1) > 0.001) {
        fFiscal = true;
        setFiscalStyle();
    }
}

void DlgPaymentChoose::on_lePrepaid_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if (arg1.toDouble() > fMaxPrepaid && fMaxPrepaid > 1) {
        ui->lePrepaid->setDouble(fMaxPrepaid);
    }
    countChange();
}

void DlgPaymentChoose::on_leDebt_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    countChange();
}

void DlgPaymentChoose::on_leCash_returnPressed()
{
    focusNextChild();
}

void DlgPaymentChoose::on_leCard_returnPressed()
{
    focusNextChild();
}

void DlgPaymentChoose::on_leBankTransfer_returnPressed()
{
    focusNextChild();
}

void DlgPaymentChoose::on_leIdram_returnPressed()
{
    focusNextChild();
}

void DlgPaymentChoose::on_leTelcell_returnPressed()
{
    focusNextChild();
}

void DlgPaymentChoose::on_lePrepaid_returnPressed()
{
    focusNextChild();
}

void DlgPaymentChoose::on_leDebt_returnPressed()
{
    focusNextChild();
}

void DlgPaymentChoose::on_leCashIn_returnPressed()
{
    focusNextChild();
}

void DlgPaymentChoose::on_leChange_returnPressed()
{
    focusNextChild();
}

void DlgPaymentChoose::on_btnCredit_clicked()
{
    clearAll(ui->leCredit);
}

void DlgPaymentChoose::on_leCredit_returnPressed()
{
    focusNextChild();
}
