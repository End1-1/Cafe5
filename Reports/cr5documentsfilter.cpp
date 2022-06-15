#include "cr5documentsfilter.h"
#include "ui_cr5documentsfilter.h"
#include "c5cache.h"

CR5DocumentsFilter::CR5DocumentsFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5DocumentsFilter)
{
    ui->setupUi(this);
    ui->leDocState->setSelector(dbParams, ui->leDocStateName, cache_doc_state);
    ui->leDocType->setSelector(dbParams, ui->leDocTypeName, cache_doc_type);
    ui->lePartner->setSelector(dbParams, ui->lePartnerName, cache_goods_partners);
    ui->leOperator->setSelector(dbParams, ui->leOperatorName, cache_users);
    ui->leReason->setSelector(dbParams, ui->leReasonName, cache_store_reason);
    ui->lePayment->setSelector(dbParams, ui->lePaymentName, cache_header_payment);
    ui->lePaid->setSelector(dbParams, ui->lePaidName, cache_header_paid);
}

CR5DocumentsFilter::~CR5DocumentsFilter()
{
    delete ui;
}

QString CR5DocumentsFilter::condition()
{
    QString result = " h.f_date between " + ui->deStart->toMySQLDate() + " and " + ui->deEnd->toMySQLDate();
    if (!ui->leDocType->isEmpty()) {
        result += " and h.f_type in (" + ui->leDocType->text() + ") ";
    }
    if (!ui->leDocState->isEmpty()) {
        result += " and h.f_state in (" + ui->leDocState->text() + ") ";
    }
    if (!ui->lePartner->isEmpty()) {
        result += " and h.f_partner in (" + ui->lePartner->text() + " ) ";
    }
    if (!ui->leOperator->isEmpty()) {
        result += " and h.f_operator in (" + ui->leOperator->text() + ") ";
    }
    if (!ui->leReason->isEmpty()) {
        result += " and sa.f_reason in (" + ui->leReason->text() + ") ";
    }
    if (!ui->lePayment->isEmpty()) {
        result += " and h.f_payment in (" + ui->lePaymentName->text() + ") ";
    }
    if (!ui->lePaid->isEmpty()) {
        result += " and h.f_paid in (" + QString::number(ui->lePaid->getInteger() - 1) + ") ";
    }
    return result;
}

QDate CR5DocumentsFilter::date1() const
{
    return ui->deStart->date();
}

QDate CR5DocumentsFilter::date2() const
{
    return ui->deEnd->date();
}
