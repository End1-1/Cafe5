#include "cr5storedocumentsfilter.h"
#include "ui_cr5storedocumentsfilter.h"
#include "c5cache.h"

CR5StoreDocumentsFilter::CR5StoreDocumentsFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5StoreDocumentsFilter)
{
    ui->setupUi(this);
    ui->leStore->setSelector(dbParams, ui->leStoreName, cache_goods_store);
    ui->leType->setSelector(dbParams, ui->leTypeName, cache_doc_type);
    ui->leReason->setSelector(dbParams, ui->leReasonName, cache_store_reason);
    ui->lePayment->setSelector(dbParams, ui->lePaymentName, cache_header_payment);
    ui->lePaid->setSelector(dbParams, ui->lePaidName, cache_header_paid);
    ui->lePartner->setSelector(dbParams, ui->lePartnerName, cache_goods_partners);
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
    }
    if (!ui->lePayment->isEmpty()) {
        result += " and h.f_payment in (" + ui->lePaymentName->text() + ") ";
    }
    if (!ui->lePaid->isEmpty()) {
        result += " and h.f_paid in (" + QString::number(ui->lePaid->getInteger() - 1) + ") ";
    }
    if (!ui->lePartner->isEmpty()) {
        result += " and h.f_partner in (" + ui->lePartner->text() + ") ";
    }
    return result;
}

QString CR5StoreDocumentsFilter::storages() const
{
    return ui->leStore->text();
}

QString CR5StoreDocumentsFilter::reason() const
{
    return ui->leReason->text();
}

void CR5StoreDocumentsFilter::setPartnerFilter(int partner)
{
    ui->lePartner->setValue(partner);
}
