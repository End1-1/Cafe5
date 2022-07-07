#include "cr5consumptionbysalesfilterdraft.h"
#include "ui_cr5consumptionbysalesfilterdraft.h"
#include "c5cache.h"

CR5ConsumptionBySalesFilterDraft::CR5ConsumptionBySalesFilterDraft(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5ConsumptionBySalesFilterDraft)
{
    ui->setupUi(this);
    ui->leStore->setSelector(dbParams, ui->leStoreName, cache_goods_store);
    ui->leGroup->setSelector(dbParams, ui->leGroupName, cache_goods_group);
    ui->leClass1->setSelector(dbParams, ui->leClassName1, cache_goods_classes);
    ui->leClass2->setSelector(dbParams, ui->leClassName2, cache_goods_classes);
    ui->leClass3->setSelector(dbParams, ui->leClassName3, cache_goods_classes);
    ui->leClass4->setSelector(dbParams, ui->leClassName4, cache_goods_classes);
}

CR5ConsumptionBySalesFilterDraft::~CR5ConsumptionBySalesFilterDraft()
{
    delete ui;
}

QString CR5ConsumptionBySalesFilterDraft::condition()
{
    return "";
}

QString CR5ConsumptionBySalesFilterDraft::filterText()
{
    QString s = QString("%1 %2 - %3").arg(tr("Date range"), ui->deStart->text(), ui->deEnd->text());
    inFilterText(s, ui->leGroupName);
    inFilterText(s, ui->leClassName1);
    return s;
}

int CR5ConsumptionBySalesFilterDraft::store()
{
    return ui->leStore->getInteger();
}

bool CR5ConsumptionBySalesFilterDraft::salesCounted()
{
    return ui->chSalesCounted->isChecked();
}

QDate CR5ConsumptionBySalesFilterDraft::date1()
{
    return ui->deStart->date();
}

QDate CR5ConsumptionBySalesFilterDraft::date2()
{
    return ui->deEnd->date();
}

QString CR5ConsumptionBySalesFilterDraft::group() const
{
    return ui->leGroup->text();
}

QString CR5ConsumptionBySalesFilterDraft::class1() const
{
    return ui->leClass1->text();
}

QString CR5ConsumptionBySalesFilterDraft::class2() const
{
    return ui->leClass2->text();
}

QString CR5ConsumptionBySalesFilterDraft::class3() const
{
    return ui->leClass3->text();
}

QString CR5ConsumptionBySalesFilterDraft::class4() const
{
    return ui->leClass4->text();
}
