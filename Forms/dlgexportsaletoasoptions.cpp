#include "dlgexportsaletoasoptions.h"
#include "ui_dlgexportsaletoasoptions.h"
#include "c5config.h"
#include <QDoubleValidator>

DlgExportSaleToAsOptions::DlgExportSaleToAsOptions(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgExportSaleToAsOptions)
{
    ui->setupUi(this);
    ui->leServiceItem->setText(__c5config.getRegValue("asserviceitem").toString());
    ui->lePartnerCode->setText(__c5config.getRegValue("aspartnercode").toString());
    ui->leStoreCode->setText(__c5config.getRegValue("asstorecode").toString());
    ui->leServiceInAccount->setText(__c5config.getRegValue("asserviceinaccount").toString());
    ui->leServiceOutAccount->setText(__c5config.getRegValue("asserviceoutaccount").toString());
    ui->leItemInAccount->setText(__c5config.getRegValue("asiteminaccount").toString());
    ui->leItemOutAccount->setText(__c5config.getRegValue("asitemoutaccount").toString());
    ui->leBankAcc->setText(__c5config.getRegValue("asbankacc").toString());
    ui->leVATValue->setText(__c5config.getRegValue("vatvalue").toString());
    ui->leVATValue->setValidator(new QDoubleValidator(0, 100, 2));
    ui->leSimpleItem->setText(__c5config.getRegValue("simpleitem").toString());
    ui->chExportSimple->setChecked(__c5config.getRegValue("simplemode").toBool());
}

DlgExportSaleToAsOptions::~DlgExportSaleToAsOptions()
{
    delete ui;
}

int DlgExportSaleToAsOptions::getOption(const QStringList &dbParams, QString &partner, QString &store, QString &service,
                                        QString &srvinacc,
                                        QString &srvoutacc, QString &iteminacc, QString &itemoutacc, QString &bankacc, QString &vat,
                                        bool &exportSimple, QString &simpleItem)
{
    DlgExportSaleToAsOptions d(dbParams);
    if (d.exec() == QDialog::Accepted) {
        partner = d.ui->lePartnerCode->text();
        store = d.ui->leStoreCode->text();
        service = d.ui->leServiceItem->text();
        srvinacc = d.ui->leServiceInAccount->text();
        srvoutacc = d.ui->leServiceOutAccount->text();
        iteminacc = d.ui->leItemInAccount->text();
        itemoutacc = d.ui->leItemOutAccount->text();
        bankacc = d.ui->leBankAcc->text();
        vat = d.ui->leVATValue->text();
        simpleItem = d.ui->leSimpleItem->text();
        exportSimple = d.ui->chExportSimple->isChecked();
        if (d.ui->rbTax->isChecked()) {
            return 1;
        } else if (d.ui->rbNoTax->isChecked()) {
            return 2;
        } else {
            return 3;
        }
    } else {
        return 0;
    }
}

void DlgExportSaleToAsOptions::on_btnOK_clicked()
{
    accept();
}

void DlgExportSaleToAsOptions::on_btnCancel_clicked()
{
    reject();
}

void DlgExportSaleToAsOptions::on_lePartnerCode_textEdited(const QString &arg1)
{
    __c5config.setRegValue("aspartnercode", arg1);
}

void DlgExportSaleToAsOptions::on_leStoreCode_textEdited(const QString &arg1)
{
    __c5config.setRegValue("asstorecode", arg1);
}

void DlgExportSaleToAsOptions::on_leServiceItem_textEdited(const QString &arg1)
{
    __c5config.setRegValue("asserviceitem", arg1);
}

void DlgExportSaleToAsOptions::on_leServiceOutAccount_textEdited(const QString &arg1)
{
    __c5config.setRegValue("asserviceoutaccount", arg1);
}

void DlgExportSaleToAsOptions::on_leServiceInAccount_textEdited(const QString &arg1)
{
    __c5config.setRegValue("asserviceinaccount", arg1);
}

void DlgExportSaleToAsOptions::on_leItemOutAccount_textEdited(const QString &arg1)
{
    __c5config.setRegValue("asitemoutaccount", arg1);
}

void DlgExportSaleToAsOptions::on_leItemInAccount_textEdited(const QString &arg1)
{
    __c5config.setRegValue("asiteminaccount", arg1);
}

void DlgExportSaleToAsOptions::on_leBankAcc_textEdited(const QString &arg1)
{
    __c5config.setRegValue("asbankacc", arg1);
}

void DlgExportSaleToAsOptions::on_leVATValue_textEdited(const QString &arg1)
{
    __c5config.setRegValue("vatvalue", arg1);
}

void DlgExportSaleToAsOptions::on_chExportSimple_clicked(bool checked)
{
    __c5config.setRegValue("simplemode", checked);
}

void DlgExportSaleToAsOptions::on_leSimpleItem_textChanged(const QString &arg1)
{
    __c5config.setRegValue("simpleitem", arg1);
}
