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
    return result;
}