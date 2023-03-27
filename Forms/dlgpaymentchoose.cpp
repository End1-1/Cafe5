#include "dlgpaymentchoose.h"
#include "ui_dlgpaymentchoose.h"
#include <QDoubleValidator>

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
    ui->leDebt->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->leCashIn->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->leChange->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->lePrepaid->setValidator(new QDoubleValidator(0, 1000000000, 2));
    adjustSize();

}

DlgPaymentChoose::~DlgPaymentChoose()
{
    delete ui;
}

bool DlgPaymentChoose::getValues(double total, double &cash, double &card, double &idram, double &telcell, double &bank,
                                 double &prepaid, double &debt, double &cashin, double &change, bool &fiscal)
{
    DlgPaymentChoose d(__c5config.dbParams());
    d.ui->leTotal->setDouble(total);
    d.ui->leCash->setDouble(cash);
    d.ui->leCard->setDouble(card);
    d.ui->leBankTransfer->setDouble(bank);
    d.ui->leIdram->setDouble(idram);
    d.ui->leTelcell->setDouble(telcell);
    d.ui->lePrepaid->setDouble(prepaid);
    d.ui->leDebt->setDouble(debt);
    d.ui->leCashIn->setDouble(cashin);
    d.ui->leChange->setDouble(change);
    d.ui->btnFiscal->setChecked(fiscal);
    if (d.exec() == QDialog::Accepted) {
        cash = d.ui->leCash->getDouble();
        card = d.ui->leCard->getDouble();
        bank = d.ui->leBankTransfer->getDouble();
        idram = d.ui->leIdram->getDouble();
        telcell = d.ui->leTelcell->getDouble();
        prepaid = d.ui->lePrepaid->getDouble();
        debt = d.ui->leDebt->getDouble();
        cashin = d.ui->leCashIn->getDouble();
        change = d.ui->leChange->getDouble();
        fiscal = d.ui->btnFiscal->isChecked();
        return true;
    }
    return false;
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
    le->setText(ui->leTotal->text());
}

void DlgPaymentChoose::on_btnCard_clicked()
{
    clearAll(ui->leCard);
}

void DlgPaymentChoose::on_btnIdram_clicked()
{
    clearAll(ui->leIdram);
}

void DlgPaymentChoose::on_btnTelcell_clicked()
{
    clearAll(ui->leTelcell);
}

void DlgPaymentChoose::on_btnPrepaid_clicked()
{
    clearAll(ui->lePrepaid);
}

void DlgPaymentChoose::on_btnDebt_clicked()
{
    clearAll(ui->leDebt);
}

void DlgPaymentChoose::on_leCashIn_textChanged(const QString &arg1)
{
    double diff = ui->leCashIn->getDouble()
            - ui->leCash->getDouble()
            - ui->leCard->getDouble()
            - ui->leIdram->getDouble()
            - ui->leTelcell->getDouble()
            - ui->lePrepaid->getDouble();
    ui->leChange->setDouble(diff > 0.001 ? diff : 0);
}

void DlgPaymentChoose::on_btnBankTransfer_clicked()
{
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
            + ui->leDebt->getDouble();
    if (value > ui->leTotal->getDouble() || value < ui->leTotal->getDouble()) {
        return;
    }
    accept();
}
