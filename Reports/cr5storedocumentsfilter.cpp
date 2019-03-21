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
    return result;
}

QString CR5StoreDocumentsFilter::storages()
{
    return ui->leStore->text();
}
