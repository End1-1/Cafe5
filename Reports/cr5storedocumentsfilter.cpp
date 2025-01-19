#include "cr5storedocumentsfilter.h"
#include "ui_cr5storedocumentsfilter.h"
#include "c5cache.h"

CR5StoreDocumentsFilter::CR5StoreDocumentsFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5StoreDocumentsFilter)
{
    ui->setupUi(this);
    ui->leStore->setSelector(dbParams, ui->leStoreName, cache_goods_store).setMultiselection(true);
    ui->leType->setSelector(dbParams, ui->leTypeName, cache_doc_type).setMultiselection(true);
    ui->lePayment->setSelector(dbParams, ui->lePaymentName, cache_header_payment).setMultiselection(true);
    ui->lePartner->setSelector(dbParams, ui->lePartnerName, cache_goods_partners).setMultiselection(true);
    ui->leState->setSelector(dbParams, ui->leStateName, cache_doc_state);
    ui->leReason->setSelector(dbParams, ui->leReasonName, cache_store_reason);
}

CR5StoreDocumentsFilter::~CR5StoreDocumentsFilter()
{
    delete ui;
}

QString CR5StoreDocumentsFilter::condition()
{
    QString result = "where h.f_date between " + ui->deStart->toMySQLDate() + " and " + ui->deEnd->toMySQLDate();
    if (!ui->leType->isEmpty()) {
        result += " and h.f_type in (" + ui->leType->text() + ") ";
    } else {
        result += " and h.f_type in (1,2,3,6)";
    }
    if (!ui->lePayment->isEmpty()) {
        result += " and h.f_payment in (" + ui->lePayment->text() + ") ";
    }
    if (ui->rbpNo->isChecked()) {
        result += " and h.f_paid<h.f_amount ";
    }
    if (ui->rbpYes->isChecked()) {
        result += " and h.f_paid=h.f_amount ";
    }
    if (ui->rbpPartial->isChecked()) {
        result += " and (h.f_paid>0 and h.f_paid<h.f_amount) ";
    }
    if (!ui->lePartner->isEmpty()) {
        result += " and h.f_partner in (" + ui->lePartner->text() + ") ";
    }
    if (!ui->leStore->isEmpty()) {
        result += QString (" and (hs.f_storein in (%1) or hs.f_storeout in (%1)) ").arg(ui->leStore->text());
    }
    in(result, "h.f_state", ui->leState);
    in(result, "hs.f_reason", ui->leReason);
    return result;
}

QString CR5StoreDocumentsFilter::storages() const
{
    return ui->leStore->text();
}

void CR5StoreDocumentsFilter::setPartnerFilter(int partner)
{
    ui->lePartner->setValue(partner);
}

void CR5StoreDocumentsFilter::setDateFilter(const QDate &d1, const QDate &d2)
{
    ui->deStart->setDate(d1);
    ui->deEnd->setDate(d2);
}

void CR5StoreDocumentsFilter::setPaidFilter(int paid)
{
    switch (paid) {
        case 0 :
            ui->rbpAll->setChecked(true);
            break;
        case 1:
            ui->rbpNo->setChecked(true);
            break;
        case 2:
            ui->rbpYes->setChecked(true);
            break;
        case 4:
            ui->rbpPartial->setChecked(true);
            break;
        default:
            break;
    }
}
