#include "cr5storedocumentsfilter.h"
#include "ui_cr5storedocumentsfilter.h"
#include "c5cache.h"

CR5StoreDocumentsFilter::CR5StoreDocumentsFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5StoreDocumentsFilter)
{
    ui->setupUi(this);
    ui->leStore->setSelector(ui->leStoreName, cache_goods_store).setMultiselection(true);
    ui->leType->setSelector(ui->leTypeName, cache_doc_type).setMultiselection(true);
    ui->lePayment->setSelector(ui->lePaymentName, cache_header_payment).setMultiselection(true);
    ui->lePartner->setSelector(ui->lePartnerName, cache_goods_partners).setMultiselection(true);
    ui->leState->setSelector(ui->leStateName, cache_doc_state);
    ui->leReason->setSelector(ui->leReasonName, cache_store_reason);
}

CR5StoreDocumentsFilter::~CR5StoreDocumentsFilter()
{
    delete ui;
}

QString CR5StoreDocumentsFilter::condition()
{
    QString result = "where h.f_date between " + ui->deStart->toMySQLDate() + " and " + ui->deEnd->toMySQLDate();

    if(!ui->leType->isEmpty()) {
        in(result, "h.f_type", ui->leType);
    } else {
        result += " and h.f_type in (1,2,3,6)";
    }

    in(result, "h.f_payment", ui->lePayment);

    if(ui->rbpNo->isChecked()) {
        result += " and h.f_paid<h.f_amount ";
    }

    if(ui->rbpYes->isChecked()) {
        result += " and h.f_paid=h.f_amount ";
    }

    if(ui->rbpPartial->isChecked()) {
        result += " and (h.f_paid>0 and h.f_paid<h.f_amount) ";
    }

    in(result, "h.f_partner", ui->lePartner);

    if(!ui->leStore->isEmpty()) {
        result += QString(" and (hs.f_storein in (%1) or hs.f_storeout in (%1)) ").arg(normalizeIntInList(ui->leStore->text()));
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
    switch(paid) {
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
